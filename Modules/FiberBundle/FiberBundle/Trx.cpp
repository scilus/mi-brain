#include "Trx.h"

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <set>
#include <vector>

#include <mitkLogMacros.h>

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

  template <typename T>
  void AppendIndices(
    const trx::TypedArray::ByteView& bytes, std::vector<vtkIdType>& out)
  {
    // Same alignment concern as for the positions
    const size_t count = bytes.size / sizeof(T);
    out.reserve(count);
    for (size_t i = 0; i < count; ++i)
    {
      T value;
      std::memcpy(&value, bytes.data + i * sizeof(T), sizeof(T));
      out.push_back(static_cast<vtkIdType>(value));
    }
  }

  // TRX groups are lists of streamline indices, usually uint32 but any
  // integer type is allowed by the spec.
  mitk::FilteredFiberBundle::StreamlineGroups ReadGroups(
    const trx::AnyTrxFile& trxFile)
  {
    const auto nbStreamlines = static_cast<vtkIdType>(trxFile.num_streamlines());

    mitk::FilteredFiberBundle::StreamlineGroups groups;
    for (const auto& nameAndArray : trxFile.groups)
    {
      const auto& name = nameAndArray.first;
      const auto& array = nameAndArray.second;

      std::vector<vtkIdType> indices;
      const auto bytes = array.to_bytes();
      const auto& dtype = array.dtype;
      if      (dtype == "uint8")  { AppendIndices<uint8_t>(bytes, indices); }
      else if (dtype == "uint16") { AppendIndices<uint16_t>(bytes, indices); }
      else if (dtype == "uint32") { AppendIndices<uint32_t>(bytes, indices); }
      else if (dtype == "uint64") { AppendIndices<uint64_t>(bytes, indices); }
      else if (dtype == "int8")   { AppendIndices<int8_t>(bytes, indices); }
      else if (dtype == "int16")  { AppendIndices<int16_t>(bytes, indices); }
      else if (dtype == "int32")  { AppendIndices<int32_t>(bytes, indices); }
      else if (dtype == "int64")  { AppendIndices<int64_t>(bytes, indices); }
      else
      {
        MITK_WARN << "TRX group '" << name << "' ignored: unsupported dtype "
                  << dtype;
        continue;
      }

      const auto invalid = std::remove_if(indices.begin(), indices.end(),
        [nbStreamlines](vtkIdType i) { return i < 0 || i >= nbStreamlines; });
      if (invalid != indices.end())
      {
        MITK_WARN << "TRX group '" << name << "': "
                  << std::distance(invalid, indices.end())
                  << " out of range streamline indices ignored";
        indices.erase(invalid, indices.end());
      }

      groups[name] = std::move(indices);
    }
    return groups;
  }

  // TRX groups are stored as files named "groups/<name>.uint32", so the name
  // can't contain a dot (it would be read as a dimension or a dtype) nor
  // anything that isn't allowed in a file name.
  std::string TrxGroupName(const std::string& name)
  {
    std::string trxName = name;
    for (auto& c : trxName)
    {
      if (c == '.' || c == '/' || c == '\\' || c == ':' || c == '*' ||
          c == '?' || c == '"' || c == '<' || c == '>' || c == '|' ||
          static_cast<unsigned char>(c) < 0x20)
      {
        c = '_';
      }
    }
    if (trxName.empty()) { trxName = "group"; }
    return trxName;
  }

  void WriteGroups(
    trx::TrxStream& stream,
    const std::map<std::string, std::vector<vtkIdType>>& groups,
    const vtkIdType nbStreamlines)
  {
    std::set<std::string> usedNames;
    for (const auto& nameAndIndices : groups)
    {
      std::vector<uint32_t> indices;
      indices.reserve(nameAndIndices.second.size());
      for (const auto idx : nameAndIndices.second)
      {
        if (idx >= 0 && idx < nbStreamlines)
        {
          indices.push_back(static_cast<uint32_t>(idx));
        }
      }
      std::sort(indices.begin(), indices.end());
      indices.erase(std::unique(indices.begin(), indices.end()), indices.end());
      if (indices.empty()) { continue; }

      // Two names can become identical once sanitized
      const auto baseName = TrxGroupName(nameAndIndices.first);
      auto trxName = baseName;
      for (int i = 2; usedNames.count(trxName); ++i)
      {
        trxName = baseName + "_" + std::to_string(i);
      }
      usedNames.insert(trxName);
      if (trxName != nameAndIndices.first)
      {
        MITK_INFO << "TRX group '" << nameAndIndices.first
                  << "' written as '" << trxName << "'";
      }

      stream.push_group_from_indices(trxName, indices);
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
  auto groups = ReadGroups(trxFile);

  // SetFiberPolyData deep copies, so the mapped file (and the temporary
  // directory created for zipped .trx files) can be released now.
  trxFile.close();

  fiber->SetFiberPolyData(polyData);
  fiber->SetStreamlineGroups(groups);

  return polyData->GetNumberOfPoints();
}

void FiberBundle_EXPORT WriteTrx(
  const std::string& filename,
  vtkPolyData* polydata,
  const std::map<std::string, std::vector<vtkIdType>>& groups)
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

  WriteGroups(stream, groups, lines->GetNumberOfCells());

  stream.finalize<float>(filename);
}
