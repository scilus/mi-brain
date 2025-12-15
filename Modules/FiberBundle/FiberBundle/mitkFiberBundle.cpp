#include "mitkFiberBundle.h"

#include "mitkImagePixelReadAccessor.h"
#include <mitkPixelTypeMultiplex.h>

#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkUnsignedCharArray.h>
#include <vtkPolyLine.h>
#include <vtkGenericCell.h>
#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkClipPolyData.h>
#include <vtkPlane.h>
#include <vtkDoubleArray.h>
#include <vtkKochanekSpline.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkPolygon.h>
#include <boost/progress.hpp>
#include <vtkTransformPolyDataFilter.h>
#include <mitkTransferFunction.h>
#include <vtkLookupTable.h>
#include <mitkLookupTable.h>
#include <vtkCardinalSpline.h>
#include <vtkAppendPolyData.h>

const char* mitk::FiberBundle::FIBER_ID_ARRAY = "Fiber_IDs";

mitk::FiberBundle::FiberBundle( vtkPolyData* fiberPolyData )
  : m_NumFibers(0)
  , m_IsInOrientationColoring(true)
{
  m_FiberPolyData = vtkSmartPointer<vtkPolyData>::New();
  if (fiberPolyData != nullptr)
    m_FiberPolyData = fiberPolyData;
  else
  {
    this->m_FiberPolyData->SetPoints(vtkSmartPointer<vtkPoints>::New());
    this->m_FiberPolyData->SetLines(vtkSmartPointer<vtkCellArray>::New());
  }

  this->UpdateFiberGeometry();

  // We always set the color ourself in MI-Brain, don't bother
  // this->ColorFibersByOrientation();
}

mitk::FiberBundle::~FiberBundle()
{}

mitk::FiberBundle::Pointer mitk::FiberBundle::GetDeepCopy()
{
  mitk::FiberBundle::Pointer newFib = mitk::FiberBundle::New(m_FiberPolyData);
  newFib->SetFiberColors(this->m_FiberColors);
  return newFib;
}

vtkSmartPointer<vtkPolyData> mitk::FiberBundle::GeneratePolyDataByIds(std::vector<long>)
{
  return nullptr;
}

itk::Point<float, 3> mitk::FiberBundle::GetItkPoint(double point[3])
{
  itk::Point<float, 3> itkPoint;
  itkPoint[0] = point[0];
  itkPoint[1] = point[1];
  itkPoint[2] = point[2];
  return itkPoint;
}

/*
 * set PolyData (additional flag to recompute fiber geometry, default = true)
 */
void mitk::FiberBundle::SetFiberPolyData(vtkSmartPointer<vtkPolyData> fiberPD, bool updateGeometry)
{
  if (fiberPD == nullptr)
    this->m_FiberPolyData = vtkSmartPointer<vtkPolyData>::New();
  else
    m_FiberPolyData->DeepCopy(fiberPD);

  m_NumFibers = m_FiberPolyData->GetNumberOfLines();

  if (updateGeometry)
    UpdateFiberGeometry();
}

/*
 * return vtkPolyData
 */
vtkSmartPointer<vtkPolyData> mitk::FiberBundle::GetFiberPolyData() const
{
  return m_FiberPolyData;
}

vtkSmartPointer<vtkUnsignedCharArray>
mitk::FiberBundle::GetNewColorArray(const unsigned int nbPoints)
{
  auto newColors = vtkSmartPointer<vtkUnsignedCharArray>::New();
  if (nbPoints > 0)
  {
    newColors->Allocate(nbPoints * 4);
  }
  newColors->SetNumberOfComponents(4);
  newColors->SetName("FIBER_COLORS");
  return newColors;
}

void mitk::FiberBundle::ColorFibersByLength(const bool opacity, const bool normalize)
{
  if (m_MaxFiberLength<=0)
    return;

  int numOfPoints = this->GetNumberOfPoints();

  //colors and alpha value for each single point, RGBA = 4 components
  unsigned char rgba[4] = {0,0,0,0};
  if (!m_FiberColors) { m_FiberColors = GetNewColorArray(numOfPoints); }

  int numOfFibers = m_FiberPolyData->GetNumberOfLines();
  if (numOfFibers < 1)
    return;

  mitk::LookupTable::Pointer mitkLookup = mitk::LookupTable::New();
  vtkSmartPointer<vtkLookupTable> lookupTable = vtkSmartPointer<vtkLookupTable>::New();
  lookupTable->SetTableRange(0.0, 0.8);
  lookupTable->Build();
  mitkLookup->SetVtkLookupTable(lookupTable);
  mitkLookup->SetType(mitk::LookupTable::JET);

  unsigned int count = 0;
  for (int i=0; i<m_FiberPolyData->GetNumberOfCells(); i++)
  {
    vtkCell* cell = m_FiberPolyData->GetCell(i);
    int numPoints = cell->GetNumberOfPoints();

    float l = m_FiberLengths.at(i)/m_MaxFiberLength;
    if (!normalize)
    {
      l = m_FiberLengths.at(i)/255.0;
      if (l > 1.0)
        l = 1.0;
    }
    for (int j=0; j<numPoints; j++)
    {
      double color[3];
      lookupTable->GetColor(1.0 - l, color);

      rgba[0] = (unsigned char) (255.0 * color[0]);
      rgba[1] = (unsigned char) (255.0 * color[1]);
      rgba[2] = (unsigned char) (255.0 * color[2]);
      if (opacity)
        rgba[3] = (unsigned char) (255.0 * l);
      else
        rgba[3] = (unsigned char) (255.0);
      m_FiberColors->InsertTypedTuple(cell->GetPointId(j), rgba);
      count++;
    }
  }
  m_UpdateTime3D.Modified();
  m_UpdateTime2D.Modified();
}

void mitk::FiberBundle::ColorFibersByOrientation()
{
  //===== FOR WRITING A TEST ========================
  //  colorT size == tupelComponents * tupelElements
  //  compare color results
  //  to cover this code 100% also PolyData needed, where colorarray already exists
  //  + one fiber with exactly 1 point
  //  + one fiber with 0 points
  //=================================================

  vtkPoints* extrPoints = nullptr;
  extrPoints = m_FiberPolyData->GetPoints();
  int numOfPoints = 0;
  if (extrPoints!=nullptr)
    numOfPoints = extrPoints->GetNumberOfPoints();

  //colors and alpha value for each single point, RGBA = 4 components
  unsigned char rgba[4] = {0,0,0,0};
  if (!m_FiberColors) { m_FiberColors = GetNewColorArray(numOfPoints); }

  int numOfFibers = m_FiberPolyData->GetNumberOfLines();
  if (numOfFibers < 1)
    return;

  /* extract single fibers of fiberBundle */
  vtkCellArray* fiberList = m_FiberPolyData->GetLines();
  fiberList->InitTraversal();
  for (int fi=0; fi<numOfFibers; ++fi) {

    vtkIdType* idList; // contains the point id's of the line
    vtkIdType pointsPerFiber; // number of points for current line
    fiberList->GetNextCell(pointsPerFiber, idList);

    /* single fiber checkpoints: is number of points valid */
    if (pointsPerFiber > 1)
    {
      /* operate on points of single fiber */
      for (int i=0; i <pointsPerFiber; ++i)
      {
        /* process all points elastV[0]ept starting and endpoint for calculating color value take current point, previous point and next point */
        if (i<pointsPerFiber-1 && i > 0)
        {
          /* The color value of the current point is influenced by the previous point and next point. */
          vnl_vector_fixed< double, 3 > currentPntvtk(extrPoints->GetPoint(idList[i])[0], extrPoints->GetPoint(idList[i])[1],extrPoints->GetPoint(idList[i])[2]);
          vnl_vector_fixed< double, 3 > nextPntvtk(extrPoints->GetPoint(idList[i+1])[0], extrPoints->GetPoint(idList[i+1])[1], extrPoints->GetPoint(idList[i+1])[2]);
          vnl_vector_fixed< double, 3 > prevPntvtk(extrPoints->GetPoint(idList[i-1])[0], extrPoints->GetPoint(idList[i-1])[1], extrPoints->GetPoint(idList[i-1])[2]);

          vnl_vector_fixed< double, 3 > diff1;
          diff1 = currentPntvtk - nextPntvtk;

          vnl_vector_fixed< double, 3 > diff2;
          diff2 = currentPntvtk - prevPntvtk;

          vnl_vector_fixed< double, 3 > diff;
          diff = (diff1 - diff2) / 2.0;
          diff.normalize();

          rgba[0] = (unsigned char) (255.0 * std::fabs(diff[0]));
          rgba[1] = (unsigned char) (255.0 * std::fabs(diff[1]));
          rgba[2] = (unsigned char) (255.0 * std::fabs(diff[2]));
          rgba[3] = (unsigned char) (255.0);
        }
        else if (i==0)
        {
          /* First point has no previous point, therefore only diff1 is taken */

          vnl_vector_fixed< double, 3 > currentPntvtk(extrPoints->GetPoint(idList[i])[0], extrPoints->GetPoint(idList[i])[1],extrPoints->GetPoint(idList[i])[2]);
          vnl_vector_fixed< double, 3 > nextPntvtk(extrPoints->GetPoint(idList[i+1])[0], extrPoints->GetPoint(idList[i+1])[1], extrPoints->GetPoint(idList[i+1])[2]);

          vnl_vector_fixed< double, 3 > diff1;
          diff1 = currentPntvtk - nextPntvtk;
          diff1.normalize();

          rgba[0] = (unsigned char) (255.0 * std::fabs(diff1[0]));
          rgba[1] = (unsigned char) (255.0 * std::fabs(diff1[1]));
          rgba[2] = (unsigned char) (255.0 * std::fabs(diff1[2]));
          rgba[3] = (unsigned char) (255.0);
        }
        else if (i==pointsPerFiber-1)
        {
          /* Last point has no next point, therefore only diff2 is taken */
          vnl_vector_fixed< double, 3 > currentPntvtk(extrPoints->GetPoint(idList[i])[0], extrPoints->GetPoint(idList[i])[1],extrPoints->GetPoint(idList[i])[2]);
          vnl_vector_fixed< double, 3 > prevPntvtk(extrPoints->GetPoint(idList[i-1])[0], extrPoints->GetPoint(idList[i-1])[1], extrPoints->GetPoint(idList[i-1])[2]);

          vnl_vector_fixed< double, 3 > diff2;
          diff2 = currentPntvtk - prevPntvtk;
          diff2.normalize();

          rgba[0] = (unsigned char) (255.0 * std::fabs(diff2[0]));
          rgba[1] = (unsigned char) (255.0 * std::fabs(diff2[1]));
          rgba[2] = (unsigned char) (255.0 * std::fabs(diff2[2]));
          rgba[3] = (unsigned char) (255.0);
        }
        m_FiberColors->InsertTypedTuple(idList[i], rgba);
      }
    }
    else if (pointsPerFiber == 1)
    {
      /* a single point does not define a fiber (use vertex mechanisms instead */
      continue;
    }
    else
    {
      MITK_DEBUG << "Fiber with 0 points detected... please check your tractography algorithm!" ;
      continue;
    }
  }
  m_UpdateTime3D.Modified();
  m_UpdateTime2D.Modified();
}

void mitk::FiberBundle::ColorFibersByCurvature(bool minMaxNorm)
{
  double window = 75;

  //colors and alpha value for each single point, RGBA = 4 components
  unsigned char rgba[4] = {0,0,0,0};
  if (!m_FiberColors) { m_FiberColors = GetNewColorArray(m_FiberPolyData->GetNumberOfPoints()); }

  mitk::LookupTable::Pointer mitkLookup = mitk::LookupTable::New();
  vtkSmartPointer<vtkLookupTable> lookupTable = vtkSmartPointer<vtkLookupTable>::New();
  lookupTable->SetTableRange(0.0, 0.8);
  lookupTable->Build();
  mitkLookup->SetVtkLookupTable(lookupTable);
  mitkLookup->SetType(mitk::LookupTable::JET);

  std::vector< double > values;
  double min = 1;
  double max = 0;
  MITK_INFO << "Coloring fibers by curvature";
  boost::progress_display disp(m_FiberPolyData->GetNumberOfCells());
  for (int i=0; i<m_FiberPolyData->GetNumberOfCells(); i++)
  {
    ++disp;
    vtkCell* cell = m_FiberPolyData->GetCell(i);
    int numPoints = cell->GetNumberOfPoints();
    vtkPoints* points = cell->GetPoints();

    // calculate curvatures
    for (int j=0; j<numPoints; j++)
    {
      double dist = 0;
      int c = j;
      std::vector< vnl_vector_fixed< float, 3 > > vectors;
      vnl_vector_fixed< float, 3 > meanV; meanV.fill(0.0);
      while(dist<window/2 && c>1)
      {
        double p1[3];
        points->GetPoint(c-1, p1);
        double p2[3];
        points->GetPoint(c, p2);

        vnl_vector_fixed< float, 3 > v;
        v[0] = p2[0]-p1[0];
        v[1] = p2[1]-p1[1];
        v[2] = p2[2]-p1[2];
        dist += v.magnitude();
        v.normalize();
        vectors.push_back(v);
        meanV += v;
        c--;
      }
      c = j;
      dist = 0;
      while(dist<window/2 && c<numPoints-1)
      {
        double p1[3];
        points->GetPoint(c, p1);
        double p2[3];
        points->GetPoint(c+1, p2);

        vnl_vector_fixed< float, 3 > v;
        v[0] = p2[0]-p1[0];
        v[1] = p2[1]-p1[1];
        v[2] = p2[2]-p1[2];
        dist += v.magnitude();
        v.normalize();
        vectors.push_back(v);
        meanV += v;
        c++;
      }
      meanV.normalize();

      double dev = 0;
      for (unsigned int c=0; c<vectors.size(); c++)
      {
        double angle = dot_product(meanV, vectors.at(c));
        if (angle>1.0)
          angle = 1.0;
        if (angle<-1.0)
          angle = -1.0;
        dev += acos(angle)*180/itk::Math::pi;
      }
      if (vectors.size()>0)
        dev /= vectors.size();

      dev = 1.0-dev/180.0;
      values.push_back(dev);
      if (dev<min)
        min = dev;
      if (dev>max)
        max = dev;
    }
  }
  unsigned int count = 0;
  for (int i=0; i<m_FiberPolyData->GetNumberOfCells(); i++)
  {
    vtkCell* cell = m_FiberPolyData->GetCell(i);
    int numPoints = cell->GetNumberOfPoints();
    for (int j=0; j<numPoints; j++)
    {
      double color[3];
      double dev = values.at(count);
      if (minMaxNorm)
        dev = (dev-min)/(max-min);
      else if (dev>1)
        dev = 1;
      lookupTable->GetColor(dev, color);

      rgba[0] = (unsigned char) (255.0 * color[0]);
      rgba[1] = (unsigned char) (255.0 * color[1]);
      rgba[2] = (unsigned char) (255.0 * color[2]);
      rgba[3] = (unsigned char) (255.0);
      m_FiberColors->InsertTypedTuple(cell->GetPointId(j), rgba);
      count++;
    }
  }
  m_UpdateTime3D.Modified();
  m_UpdateTime2D.Modified();
}

void mitk::FiberBundle::SetFiberOpacity(vtkDoubleArray* FAValArray)
{
  for(long i=0; i<m_FiberColors->GetNumberOfTuples(); i++)
  {
    double faValue = FAValArray->GetValue(i);
    faValue = faValue * 255.0;
    m_FiberColors->SetComponent(i,3, (unsigned char) faValue );
  }
  m_UpdateTime3D.Modified();
  m_UpdateTime2D.Modified();
}

void mitk::FiberBundle::ResetFiberOpacity()
{
  for(long i=0; i<m_FiberColors->GetNumberOfTuples(); i++)
    m_FiberColors->SetComponent(i,3, 255.0 );
  m_UpdateTime3D.Modified();
  m_UpdateTime2D.Modified();
}

void mitk::FiberBundle::ColorFibersByScalarMap(mitk::Image::Pointer FAimage, const bool opacity, const bool normalize)
{
  mitkPixelTypeMultiplex3( ColorFibersByScalarMap, FAimage->GetPixelType(), FAimage, opacity, normalize );
  m_UpdateTime3D.Modified();
  m_UpdateTime2D.Modified();
}

template <typename TPixel>
void mitk::FiberBundle::ColorFibersByScalarMap(
  const mitk::PixelType,
  mitk::Image::Pointer image,
  const bool opacity,
  const bool normalize)
{
  if (!m_FiberColors)
  {
    m_FiberColors = GetNewColorArray(m_FiberPolyData->GetNumberOfPoints());
  }

  mitk::ImagePixelReadAccessor<TPixel,3> readimage(image, image->GetVolumeData(0));

  unsigned char rgba[4] = {0,0,0,0};
  vtkPoints* pointSet = m_FiberPolyData->GetPoints();

  mitk::LookupTable::Pointer mitkLookup = mitk::LookupTable::New();
  vtkSmartPointer<vtkLookupTable> lookupTable = vtkSmartPointer<vtkLookupTable>::New();
  lookupTable->SetTableRange(0.0, 0.8);
  lookupTable->Build();
  mitkLookup->SetVtkLookupTable(lookupTable);
  mitkLookup->SetType(mitk::LookupTable::JET);

  double min = 9999999;
  double max = -9999999;
  for(long i=0; i<m_FiberPolyData->GetNumberOfPoints(); ++i)
  {
    Point3D px;
    px[0] = pointSet->GetPoint(i)[0];
    px[1] = pointSet->GetPoint(i)[1];
    px[2] = pointSet->GetPoint(i)[2];
    double pixelValue = readimage.GetPixelByWorldCoordinates(px);
    if (pixelValue>max)
      max = pixelValue;
    if (pixelValue<min)
      min = pixelValue;
  }

  for(long i=0; i<m_FiberPolyData->GetNumberOfPoints(); ++i)
  {
    Point3D px;
    px[0] = pointSet->GetPoint(i)[0];
    px[1] = pointSet->GetPoint(i)[1];
    px[2] = pointSet->GetPoint(i)[2];
    double pixelValue = readimage.GetPixelByWorldCoordinates(px);

    if (normalize)
      pixelValue = (pixelValue-min)/(max-min);
    else if (pixelValue>1)
      pixelValue = 1;

    double color[3];
    lookupTable->GetColor(1-pixelValue, color);

    rgba[0] = (unsigned char) (255.0 * color[0]);
    rgba[1] = (unsigned char) (255.0 * color[1]);
    rgba[2] = (unsigned char) (255.0 * color[2]);
    if (opacity)
      rgba[3] = (unsigned char) (255.0 * pixelValue);
    else
      rgba[3] = (unsigned char) (255.0);
    m_FiberColors->InsertTypedTuple(i, rgba);
  }
  m_UpdateTime3D.Modified();
  m_UpdateTime2D.Modified();
}

void mitk::FiberBundle::SetFiberColors(float r, float g, float b, float alpha)
{
  if (!m_FiberColors)
  {
    m_FiberColors = GetNewColorArray(m_FiberPolyData->GetNumberOfPoints());
  }

  unsigned char rgba[4] = {0,0,0,0};
  for(long i=0; i<m_FiberPolyData->GetNumberOfPoints(); ++i)
  {
    rgba[0] = (unsigned char) r;
    rgba[1] = (unsigned char) g;
    rgba[2] = (unsigned char) b;
    rgba[3] = (unsigned char) alpha;
    m_FiberColors->InsertTypedTuple(i, rgba);
  }
  m_UpdateTime3D.Modified();
  m_UpdateTime2D.Modified();
}

void mitk::FiberBundle::UpdateFiberGeometry()
{
  m_FiberLengths.clear();
  m_MeanFiberLength = 0;
  m_MedianFiberLength = 0;
  m_LengthStDev = 0;
  m_NumFibers = m_FiberPolyData->GetNumberOfCells();

  if (m_FiberColors==nullptr || m_FiberColors->GetNumberOfTuples()!=m_FiberPolyData->GetNumberOfPoints())
    this->ColorFibersByOrientation();

  if (m_NumFibers<=0) // no fibers present; apply default geometry
  {
    m_MinFiberLength = 0;
    m_MaxFiberLength = 0;
    mitk::Geometry3D::Pointer geometry = mitk::Geometry3D::New();
    geometry->SetImageGeometry(false);
    float b[] = {0, 1, 0, 1, 0, 1};
    geometry->SetFloatBounds(b);
    SetGeometry(geometry);
    return;
  }
  double b[6];
  m_FiberPolyData->GetBounds(b);

  // calculate statistics
  for (int i=0; i<m_FiberPolyData->GetNumberOfCells(); i++)
  {
    vtkCell* cell = m_FiberPolyData->GetCell(i);
    int p = cell->GetNumberOfPoints();
    vtkPoints* points = cell->GetPoints();
    float length = 0;
    for (int j=0; j<p-1; j++)
    {
      double p1[3];
      points->GetPoint(j, p1);
      double p2[3];
      points->GetPoint(j+1, p2);

      float dist = std::sqrt((p1[0]-p2[0])*(p1[0]-p2[0])+(p1[1]-p2[1])*(p1[1]-p2[1])+(p1[2]-p2[2])*(p1[2]-p2[2]));
      length += dist;
    }
    m_FiberLengths.push_back(length);
    m_MeanFiberLength += length;
    if (i==0)
    {
      m_MinFiberLength = length;
      m_MaxFiberLength = length;
    }
    else
    {
      if (length<m_MinFiberLength)
        m_MinFiberLength = length;
      if (length>m_MaxFiberLength)
        m_MaxFiberLength = length;
    }
  }
  m_MeanFiberLength /= m_NumFibers;

  std::vector< float > sortedLengths = m_FiberLengths;
  std::sort(sortedLengths.begin(), sortedLengths.end());
  for (int i=0; i<m_NumFibers; i++)
    m_LengthStDev += (m_MeanFiberLength-sortedLengths.at(i))*(m_MeanFiberLength-sortedLengths.at(i));
  if (m_NumFibers>1)
    m_LengthStDev /= (m_NumFibers-1);
  else
    m_LengthStDev = 0;
  m_LengthStDev = std::sqrt(m_LengthStDev);
  m_MedianFiberLength = sortedLengths.at(m_NumFibers/2);

  mitk::Geometry3D::Pointer geometry = mitk::Geometry3D::New();
  geometry->SetFloatBounds(b);
  this->SetGeometry(geometry);

  m_UpdateTime3D.Modified();
  m_UpdateTime2D.Modified();
}

void mitk::FiberBundle::SetFiberColors(vtkSmartPointer<vtkUnsignedCharArray> fiberColors)
{
  for(long i=0; i<m_FiberPolyData->GetNumberOfPoints(); ++i)
  {
    unsigned char source[4] = {0,0,0,0};
    fiberColors->GetTypedTuple(i, source);

    unsigned char target[4] = {0,0,0,0};
    target[0] = source[0];
    target[1] = source[1];
    target[2] = source[2];
    target[3] = source[3];
    m_FiberColors->InsertTypedTuple(i, target);
  }
  m_UpdateTime3D.Modified();
  m_UpdateTime2D.Modified();
}

void mitk::FiberBundle::MirrorFibers(unsigned int axis)
{
  if (axis>2)
    return;

  MITK_INFO << "Mirroring fibers";
  boost::progress_display disp(m_NumFibers);

  vtkSmartPointer<vtkPoints> vtkNewPoints = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkCellArray> vtkNewCells = vtkSmartPointer<vtkCellArray>::New();

  for (int i=0; i<m_NumFibers; i++)
  {
    ++disp;
    vtkCell* cell = m_FiberPolyData->GetCell(i);
    int numPoints = cell->GetNumberOfPoints();
    vtkPoints* points = cell->GetPoints();

    vtkSmartPointer<vtkPolyLine> container = vtkSmartPointer<vtkPolyLine>::New();
    for (int j=0; j<numPoints; j++)
    {
      double* p = points->GetPoint(j);
      p[axis] = -p[axis];
      vtkIdType id = vtkNewPoints->InsertNextPoint(p);
      container->GetPointIds()->InsertNextId(id);
    }
    vtkNewCells->InsertNextCell(container);
  }

  m_FiberPolyData = vtkSmartPointer<vtkPolyData>::New();
  m_FiberPolyData->SetPoints(vtkNewPoints);
  m_FiberPolyData->SetLines(vtkNewCells);
  this->SetFiberPolyData(m_FiberPolyData, true);
}

void mitk::FiberBundle::ResampleSpline(float pointDistance, double tension, double continuity, double bias)
{
  if (pointDistance <= 0)
    return;

  vtkSmartPointer<vtkPoints> vtkSmoothPoints = vtkSmartPointer<vtkPoints>::New(); //in smoothpoints the interpolated points representing a fiber are stored.

  //in vtkcells all polylines are stored, actually all id's of them are stored
  vtkSmartPointer<vtkCellArray> vtkSmoothCells = vtkSmartPointer<vtkCellArray>::New(); //cellcontainer for smoothed lines

  MITK_INFO << "Smoothing fibers";
  vtkSmartPointer<vtkFloatArray> newFiberWeights = vtkSmartPointer<vtkFloatArray>::New();
  newFiberWeights->SetName("FIBER_WEIGHTS");
  newFiberWeights->SetNumberOfValues(m_NumFibers);

  std::vector< vtkSmartPointer<vtkPolyLine> > resampled_streamlines;
  resampled_streamlines.resize(m_NumFibers);

  boost::progress_display disp(m_NumFibers);
#pragma omp parallel for
  for (int i = 0; i < m_NumFibers; i++)
  {
    vtkSmartPointer<vtkPoints> newPoints = vtkSmartPointer<vtkPoints>::New();
    float length = 0;
#pragma omp critical
    {
      length = m_FiberLengths.at(i);
      ++disp;
      vtkCell* cell = m_FiberPolyData->GetCell(i);
      int numPoints = cell->GetNumberOfPoints();
      vtkPoints* points = cell->GetPoints();
      for (int j = 0; j < numPoints; j++)
        newPoints->InsertNextPoint(points->GetPoint(j));
    }

    int sampling = std::ceil(length / pointDistance);

    vtkSmartPointer<vtkKochanekSpline> xSpline = vtkSmartPointer<vtkKochanekSpline>::New();
    vtkSmartPointer<vtkKochanekSpline> ySpline = vtkSmartPointer<vtkKochanekSpline>::New();
    vtkSmartPointer<vtkKochanekSpline> zSpline = vtkSmartPointer<vtkKochanekSpline>::New();
    xSpline->SetDefaultBias(bias); xSpline->SetDefaultTension(tension); xSpline->SetDefaultContinuity(continuity);
    ySpline->SetDefaultBias(bias); ySpline->SetDefaultTension(tension); ySpline->SetDefaultContinuity(continuity);
    zSpline->SetDefaultBias(bias); zSpline->SetDefaultTension(tension); zSpline->SetDefaultContinuity(continuity);

    vtkSmartPointer<vtkParametricSpline> spline = vtkSmartPointer<vtkParametricSpline>::New();
    spline->SetXSpline(xSpline);
    spline->SetYSpline(ySpline);
    spline->SetZSpline(zSpline);
    spline->SetPoints(newPoints);

    vtkSmartPointer<vtkParametricFunctionSource> functionSource = vtkSmartPointer<vtkParametricFunctionSource>::New();
    functionSource->SetParametricFunction(spline);
    functionSource->SetUResolution(sampling);
    functionSource->SetVResolution(sampling);
    functionSource->SetWResolution(sampling);
    functionSource->Update();

    vtkPolyData* outputFunction = functionSource->GetOutput();
    vtkPoints* tmpSmoothPnts = outputFunction->GetPoints(); //smoothPoints of current fiber

    vtkSmartPointer<vtkPolyLine> smoothLine = vtkSmartPointer<vtkPolyLine>::New();

#pragma omp critical
    {
      for (int j = 0; j < tmpSmoothPnts->GetNumberOfPoints(); j++)
      {
        vtkIdType id = vtkSmoothPoints->InsertNextPoint(tmpSmoothPnts->GetPoint(j));
        smoothLine->GetPointIds()->InsertNextId(id);
      }

      resampled_streamlines[i] = smoothLine;
    }
  }

  for (auto container : resampled_streamlines)
  {
    vtkSmoothCells->InsertNextCell(container);
  }

  m_FiberPolyData = vtkSmartPointer<vtkPolyData>::New();
  m_FiberPolyData->SetPoints(vtkSmoothPoints);
  m_FiberPolyData->SetLines(vtkSmoothCells);
  this->SetFiberPolyData(m_FiberPolyData, true);
}

void mitk::FiberBundle::ResampleSpline(float pointDistance)
{
  ResampleSpline(pointDistance, 0, 0, 0);
}

unsigned long mitk::FiberBundle::GetNumberOfPoints() const
{
  unsigned long points = 0;
  for (int i=0; i<m_FiberPolyData->GetNumberOfCells(); i++)
  {
    vtkCell* cell = m_FiberPolyData->GetCell(i);
    points += cell->GetNumberOfPoints();
  }
  return points;
}

void mitk::FiberBundle::ResampleLinear(double pointDistance)
{
  vtkSmartPointer<vtkPoints> vtkNewPoints = vtkSmartPointer<vtkPoints>::New();
  vtkSmartPointer<vtkCellArray> vtkNewCells = vtkSmartPointer<vtkCellArray>::New();

  MITK_INFO << "Resampling fibers (linear)";
  boost::progress_display disp(m_FiberPolyData->GetNumberOfCells());
  vtkSmartPointer<vtkFloatArray> newFiberWeights = vtkSmartPointer<vtkFloatArray>::New();
  newFiberWeights->SetName("FIBER_WEIGHTS");
  newFiberWeights->SetNumberOfValues(m_NumFibers);

  std::vector< vtkSmartPointer<vtkPolyLine> > resampled_streamlines;
  resampled_streamlines.resize(m_FiberPolyData->GetNumberOfCells());

#pragma omp parallel for
  for (int i = 0; i < m_FiberPolyData->GetNumberOfCells(); i++)
  {

    std::vector< vnl_vector_fixed< double, 3 > > vertices;

#pragma omp critical
    {
      ++disp;
      vtkCell* cell = m_FiberPolyData->GetCell(i);
      int numPoints = cell->GetNumberOfPoints();
      vtkPoints* points = cell->GetPoints();

      for (int j = 0; j < numPoints; j++)
      {
        double cand[3];
        points->GetPoint(j, cand);
        vnl_vector_fixed< double, 3 > candV;
        candV[0] = cand[0]; candV[1] = cand[1]; candV[2] = cand[2];
        vertices.push_back(candV);
      }
    }

    vtkSmartPointer<vtkPolyLine> container = vtkSmartPointer<vtkPolyLine>::New();
    vnl_vector_fixed< double, 3 > lastV = vertices.at(0);

#pragma omp critical
    {
      vtkIdType id = vtkNewPoints->InsertNextPoint(lastV.data_block());
      container->GetPointIds()->InsertNextId(id);
    }
    for (unsigned int j = 1; j < vertices.size(); j++)
    {
      vnl_vector_fixed< double, 3 > vec = vertices.at(j) - lastV;
      double new_dist = vec.magnitude();

      if (new_dist >= pointDistance)
      {
        vnl_vector_fixed< double, 3 > newV = lastV;
        if (new_dist - pointDistance <= mitk::eps)
        {
          vec.normalize();
          newV += vec * pointDistance;
        }
        else
        {
          // intersection between sphere (radius 'pointDistance', center 'lastV') and line (direction 'd' and point 'p')
          vnl_vector_fixed< double, 3 > p = vertices.at(j - 1);
          vnl_vector_fixed< double, 3 > d = vertices.at(j) - p;

          double a = d[0] * d[0] + d[1] * d[1] + d[2] * d[2];
          double b = 2 * (d[0] * (p[0] - lastV[0]) + d[1] * (p[1] - lastV[1]) + d[2] * (p[2] - lastV[2]));
          double c = (p[0] - lastV[0])*(p[0] - lastV[0]) + (p[1] - lastV[1])*(p[1] - lastV[1]) + (p[2] - lastV[2])*(p[2] - lastV[2]) - pointDistance * pointDistance;

          double v1 = (-b + std::sqrt(b*b - 4 * a*c)) / (2 * a);
          double v2 = (-b - std::sqrt(b*b - 4 * a*c)) / (2 * a);

          if (v1 > 0)
            newV = p + d * v1;
          else if (v2 > 0)
            newV = p + d * v2;
          else
            MITK_INFO << "ERROR1 - linear resampling";

          j--;
        }

#pragma omp critical
        {
          vtkIdType id = vtkNewPoints->InsertNextPoint(newV.data_block());
          container->GetPointIds()->InsertNextId(id);
        }
        lastV = newV;
      }
      else if (j == vertices.size() - 1 && new_dist > 0.0001)
      {
#pragma omp critical
        {
          vtkIdType id = vtkNewPoints->InsertNextPoint(vertices.at(j).data_block());
          container->GetPointIds()->InsertNextId(id);
        }
      }
    }

#pragma omp critical
    {
      resampled_streamlines[i] = container;
    }
  }

  for (auto container : resampled_streamlines)
  {
    vtkNewCells->InsertNextCell(container);
  }

  if (vtkNewCells->GetNumberOfCells() > 0)
  {
    m_FiberPolyData = vtkSmartPointer<vtkPolyData>::New();
    m_FiberPolyData->SetPoints(vtkNewPoints);
    m_FiberPolyData->SetLines(vtkNewCells);
    this->SetFiberPolyData(m_FiberPolyData, true);
  }
}

// reapply selected colorcoding in case PolyData structure has changed
bool mitk::FiberBundle::Equals(mitk::FiberBundle* fib, double eps)
{
  if (fib==nullptr)
  {
    MITK_INFO << "Reference bundle is nullptr!";
    return false;
  }

  if (m_NumFibers!=fib->GetNumFibers())
  {
    MITK_INFO << "Unequal number of fibers!";
    MITK_INFO << m_NumFibers << " vs. " << fib->GetNumFibers();
    return false;
  }

  auto cell1 = vtkSmartPointer<vtkGenericCell>::New();
  auto cell2 = vtkSmartPointer<vtkGenericCell>::New();
  for (int i=0; i<m_NumFibers; i++)
  {
    m_FiberPolyData->GetCell(i, cell1);
    int numPoints = cell1->GetNumberOfPoints();
    vtkPoints *points = cell1->GetPoints();

    fib->GetFiberPolyData()->GetCell(i, cell2);
    int numPoints2 = cell2->GetNumberOfPoints();
    vtkPoints* points2 = cell2->GetPoints();

    if (numPoints2!=numPoints)
    {
      MITK_INFO << "Unequal number of points in fiber " << i << "!";
      MITK_INFO << numPoints2 << " vs. " << numPoints;
      return false;
    }

    for (int j=0; j<numPoints; j++)
    {
      double* p1 = points->GetPoint(j);
      double* p2 = points2->GetPoint(j);
      if (fabs(p1[0]-p2[0])>eps || fabs(p1[1]-p2[1])>eps || fabs(p1[2]-p2[2])>eps)
      {
        MITK_INFO << "Unequal points in fiber " << i << " at position " << j << "!";
        MITK_INFO << "p1: " << p1[0] << ", " << p1[1] << ", " << p1[2];
        MITK_INFO << "p2: " << p2[0] << ", " << p2[1] << ", " << p2[2];
        return false;
      }
    }
  }

  return true;
}

void mitk::FiberBundle::PrintSelf(std::ostream &os, itk::Indent indent) const
{
  os << this->GetNameOfClass() << ":\n";
  os << indent << "Number of fibers: " << this->GetNumFibers() << std::endl;
  os << indent << "Min. fiber length: " << this->GetMinFiberLength() << std::endl;
  os << indent << "Max. fiber length: " << this->GetMaxFiberLength() << std::endl;
  os << indent << "Mean fiber length: " << this->GetMeanFiberLength() << std::endl;
  os << indent << "Median fiber length: " << this->GetMedianFiberLength() << std::endl;
  os << indent << "STDEV fiber length: " << this->GetLengthStDev() << std::endl;
  os << indent << "Number of points: " << this->GetNumberOfPoints() << std::endl;

  os << indent << "Extent x: " << this->GetGeometry()->GetExtentInMM(0) << "mm" << std::endl;
  os << indent << "Extent y: " << this->GetGeometry()->GetExtentInMM(1) << "mm" << std::endl;
  os << indent << "Extent z: " << this->GetGeometry()->GetExtentInMM(2) << "mm" << std::endl;
  os << indent << "Diagonal: " << this->GetGeometry()->GetDiagonalLength()  << "mm" << std::endl;

  Superclass::PrintSelf(os, indent);
}

/* ESSENTIAL IMPLEMENTATION OF SUPERCLASS METHODS */
void mitk::FiberBundle::UpdateOutputInformation()
{

}
void mitk::FiberBundle::SetRequestedRegionToLargestPossibleRegion()
{

}
bool mitk::FiberBundle::RequestedRegionIsOutsideOfTheBufferedRegion()
{
  return false;
}
bool mitk::FiberBundle::VerifyRequestedRegion()
{
  return true;
}
void mitk::FiberBundle::SetRequestedRegion(const itk::DataObject* )
{

}
