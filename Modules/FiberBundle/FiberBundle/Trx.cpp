#include "Trx.h"

#include <cstring>
#include <stdexcept>
#include <vector>

#include "FilteredFiberBundle.hpp"

#include <vtkCellArray.h>
#include <vtkFloatArray.h>
#include <vtkIdTypeArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>

#include <trx/trx.h>

namespace
{
  template <typename T>
  void ConvertPositions(const void* data, size_t count, float* out)
  {
    // Positions inside an uncompressed .trx zip are not guaranteed to be
    // aligned, so copy bytes instead of dereferencing a T pointer.
    const auto* src = static_cast<const unsigned char*>(data);
    for (size_t i = 0; i < 3 * count; i += 3)
    {
      T p[3];
      std::memcpy(p, src + i * sizeof(T), sizeof(p));

      // TRX positions are in RAS+ mm, MITK world is LPS mm
      out[i + 0] = -static_cast<float>(p[0]);
      out[i + 1] = -static_cast<float>(p[1]);
      out[i + 2] =  static_cast<float>(p[2]);
    }
  }

  vtkSmartPointer<vtkPolyData> TrxToPolyData(const trx::AnyTrxFile& trxFile)
  {
    const auto nbPoints = static_cast<vtkIdType>(trxFile.num_vertices());

    auto coords = vtkSmartPointer<vtkFloatArray>::New();
    coords->SetNumberOfComponents(3);
    coords->SetNumberOfTuples(nbPoints);
    float* dst = coords->GetPointer(0);

    // Read by chunks to avoid materializing a converted copy of the whole file
    constexpr size_t chunkBytes = 64 * 1024 * 1024;
    trxFile.for_each_positions_chunk(chunkBytes,
      [dst](trx::TrxScalarType dtype, const void* data, size_t offset, size_t count)
      {
        float* out = dst + 3 * offset;
        switch (dtype)
        {
          case trx::TrxScalarType::Float16: ConvertPositions<Eigen::half>(data, count, out); break;
          case trx::TrxScalarType::Float32: ConvertPositions<float>(data, count, out); break;
          case trx::TrxScalarType::Float64: ConvertPositions<double>(data, count, out); break;
        }
      });

    auto points = vtkSmartPointer<vtkPoints>::New();
    points->SetData(coords);

    // TRX offsets map directly onto the VTK offsets + connectivity layout.
    // Some files omit the trailing sentinel, so add it if it is missing.
    std::vector<uint64_t> trxOffsets = trxFile.offsets_u64;
    if (trxOffsets.empty() || trxOffsets.back() != static_cast<uint64_t>(nbPoints))
    {
      trxOffsets.push_back(static_cast<uint64_t>(nbPoints));
    }
    const auto nbOffsets = static_cast<vtkIdType>(trxOffsets.size());

    auto offsets = vtkSmartPointer<vtkIdTypeArray>::New();
    offsets->SetNumberOfValues(nbOffsets);
    for (vtkIdType i = 0; i < nbOffsets; ++i)
    {
      offsets->SetValue(i, static_cast<vtkIdType>(trxOffsets[i]));
    }

    // Streamlines are stored contiguously in TRX
    auto connectivity = vtkSmartPointer<vtkIdTypeArray>::New();
    connectivity->SetNumberOfValues(nbPoints);
    for (vtkIdType i = 0; i < nbPoints; ++i)
    {
      connectivity->SetValue(i, i);
    }

    auto lines = vtkSmartPointer<vtkCellArray>::New();
    lines->SetData(offsets, connectivity);

    auto polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points);
    polyData->SetLines(lines);
    polyData->ComputeBounds();
    return polyData;
  }
}

vtkIdType FiberBundle_EXPORT ReadTrx(
  const std::string& filename, mitk::FilteredFiberBundle* fiber)
{
  auto trxFile = trx::load_any(filename);
  if (trxFile.num_vertices() == 0)
  {
    return 0;
  }

  auto polyData = TrxToPolyData(trxFile);

  // SetFiberPolyData deep copies, so the mapped file (and the temporary
  // directory created for zipped .trx files) can be released now.
  trxFile.close();

  fiber->SetFiberPolyData(polyData);

  return polyData->GetNumberOfPoints();
}

void FiberBundle_EXPORT WriteTrx(
  const std::string& filename, vtkPolyData* polydata)
{
  vtkCellArray* lines = polydata->GetLines();
  vtkPoints* points = polydata->GetPoints();
  if (lines == nullptr || points == nullptr || lines->GetNumberOfCells() == 0)
  {
    throw std::runtime_error("Cannot write an empty fiber bundle as TRX.");
  }

  // TrxStream spools positions to a temporary file, so we never hold a
  // second full copy of the bundle in memory.
  trx::TrxStream stream("float32");

  // VOXEL_TO_RASMM and DIMENSIONS are mandatory in the TRX header but
  // TrxStream does not set them. No reference image is known here, so use
  // the same defaults as trx::TrxFile.
  stream.set_voxel_to_rasmm(Eigen::Matrix4f::Identity());
  stream.set_dimensions({1, 1, 1});

  // Reused for all streamlines, re-allocated only for longer ones
  std::vector<float> pointsOnLine;
  pointsOnLine.reserve(3 * 500);

  vtkIdType nbPoints;
  const vtkIdType* ids;
  lines->InitTraversal();
  while (lines->GetNextCell(nbPoints, ids))
  {
    pointsOnLine.clear(); // Not supposed to free memory
    for (vtkIdType i = 0; i < nbPoints; ++i)
    {
      double p[3];
      points->GetPoint(ids[i], p);

      // MITK world is LPS mm, TRX positions are in RAS+ mm
      pointsOnLine.push_back(-static_cast<float>(p[0]));
      pointsOnLine.push_back(-static_cast<float>(p[1]));
      pointsOnLine.push_back( static_cast<float>(p[2]));
    }
    stream.push_streamline(pointsOnLine.data(), static_cast<size_t>(nbPoints));
  }

  stream.finalize<float>(filename);
}
