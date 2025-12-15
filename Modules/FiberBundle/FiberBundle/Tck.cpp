#include "Tck.hpp"

#include <limits>
#include <math.h>

#include "FilteredFiberBundle.hpp"

#include <vtkCellArray.h>
#include <vtkGenericCell.h>
#include <vtkPolyData.h>
#include <vtkPolyLine.h>

#include <mitkProgressBar.h>

int FiberBundle_EXPORT ReadTck(
  const std::string& filename, mitk::FilteredFiberBundle* fiber)
{
  std::ifstream f(filename);
  long firstByteOfData = 0;
  vtkIdType nbStreamlines = 0;
  std::string line;
  do
  {
    std::string dump;
    std::getline(f, line);
    if (line.find("file:") != std::string::npos)
    {
      // Example:   file: . 605
      std::stringstream s(line);
      s >> dump >> dump >> firstByteOfData;
    }
    if (!nbStreamlines && line.find("count:") != std::string::npos)
    {
      // Example    count: 125000
      std::stringstream s(line);
      s >> dump >> nbStreamlines;
    }
  } while (line.find("END") == std::string::npos);
  f.close();

  const bool needProgressBar = nbStreamlines >= 10000;
  const unsigned int step = nbStreamlines / 100;
  if (needProgressBar)
  {
    mitk::ProgressBar::GetInstance()->Reset();
    mitk::ProgressBar::GetInstance()->AddStepsToDo(102);
  }

  auto cells = vtkSmartPointer<vtkCellArray>::New();
  auto points = vtkSmartPointer<vtkPoints>::New();

  f.open(filename, std::ios::binary);
  f.seekg(firstByteOfData);
  for (vtkIdType i = 0; i < nbStreamlines; ++i)
  {
    if (needProgressBar && i % step == 0)
    {
      mitk::ProgressBar::GetInstance()->Progress();
    }

    auto container = vtkSmartPointer<vtkPolyLine>::New();
    do
    {
      float p[3];
      f.read((char*)p, 3 * sizeof(float));
      if (std::isnan(p[0])) { break; }

      // Can happen with Marco's tck branch which can create 2 'count:'
      // We can probably remove this in the next port.
      if (std::isinf(p[0])) { return 0; }

      const vtkIdType id = points->InsertNextPoint(-p[0], -p[1], p[2]);
      container->GetPointIds()->InsertNextId(id);
    } while (true);

    cells->InsertNextCell(container);
  }

  auto polyData = vtkSmartPointer<vtkPolyData>::New();
  polyData->SetPoints(points);
  polyData->SetLines(cells);

  if (needProgressBar)
  {
    mitk::ProgressBar::GetInstance()->Progress();
  }

  fiber->SetFiberPolyData(polyData);

  // 2 to avoid floating imprecision
  mitk::ProgressBar::GetInstance()->Progress(2);

  return polyData->GetNumberOfPoints();
}

std::string GetHeader(vtkPolyData* polydata)
{
  vtkCellArray* lines = polydata->GetLines();
  const vtkIdType nbFibers = lines->GetNumberOfCells();
  const std::string placeHolder = "##@*@##";

  // We need to write the exact place where the data begins, so we write
  // evrything in a string before
  std::ostringstream ss;
  ss << "mrtrix tracks\n";
  ss << "max_num_tracks: " << nbFibers << "\n";
  ss << "datatype: Float32LE\n";
  ss << "file: . " << placeHolder << "\n";
  ss << "count: " << nbFibers << "\n";
  ss << "END\n";

  std::ostringstream toString;
  toString << std::setfill(' ')
    << std::setw(placeHolder.size()) << ss.str().size();

  std::string header = ss.str();
  auto pos = header.find(placeHolder);
  return header.replace(
    pos, placeHolder.size(),
    toString.str());
}

void FiberBundle_EXPORT WriteTck(
  const std::string& filename, vtkPolyData* polydata)
{
  std::ofstream f(filename, std::ios::binary);

  const std::string header = GetHeader(polydata);
  f.write(header.c_str(), header.size());

  // vtkPolyData::GetCell is supposed to return a pointer to a vtkPolyLine that
  // it owns but I had memory corruptions in my tests.
  auto cell = vtkSmartPointer<vtkGenericCell>::New();

  // Let's keep the same vector for all streamlines instead of allocating a
  // new vector for each streamline. This way, we might re-allocate 2 or 3
  // times max.
  std::vector<float> pointsOnLine;
  pointsOnLine.reserve(3 * 500);

  const vtkIdType nbFibers = polydata->GetNumberOfLines();
  for (vtkIdType idxFiber = 0; idxFiber < nbFibers; ++idxFiber)
  {
    polydata->GetCell(idxFiber, cell);
    const vtkIdType nbPoints = cell->GetNumberOfPoints();
    vtkPoints* points = cell->GetPoints();

    pointsOnLine.clear(); // Not supposed to free memory
    pointsOnLine.reserve(3 * nbPoints);

    for (vtkIdType i = 0; i < nbPoints; ++i)
    {
      double p[3];
      points->GetPoint(i, p);
      pointsOnLine.push_back(-static_cast<float>(p[0]));
      pointsOnLine.push_back(-static_cast<float>(p[1]));
      pointsOnLine.push_back( static_cast<float>(p[2]));
    }

    // 3 NaN at the end of all streamlines
    pointsOnLine.insert(
      pointsOnLine.end(), 3, std::numeric_limits<float>::quiet_NaN());

    f.write(
      reinterpret_cast<char*>(pointsOnLine.data()),
      sizeof(float) * pointsOnLine.size());
  }

  // The tck standard requires 3 infinity at EOF
  pointsOnLine.clear(); // Not supposed to free memory
  pointsOnLine.insert(
    pointsOnLine.end(), 3, std::numeric_limits<float>::infinity());
  f.write(
    reinterpret_cast<char*>(pointsOnLine.data()),
    sizeof(float) * pointsOnLine.size());
}
