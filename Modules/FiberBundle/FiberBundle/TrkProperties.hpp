#ifndef IMEKA_FIBER_TRK_PROPERTIES_HPP_INCLUDED
#define IMEKA_FIBER_TRK_PROPERTIES_HPP_INCLUDED

#include <vtkFloatArray.h>

#include "FiberBundleExports.h"

class vtkCellData;
class vtkPolyData;

namespace Imeka
{

namespace Fiber
{

typedef vtkSmartPointer<vtkFloatArray> Properties[10];

class FIBERBUNDLE_EXPORT PropertiesHelperToReadTrk
{
public:
  PropertiesHelperToReadTrk(const unsigned int, char[10][20]);

  void Save(float*);
  void StoreInPolydata(vtkPolyData*);

private:
  unsigned int m_NbRawProperties;
  Properties m_Properties;
};

class FIBERBUNDLE_EXPORT PropertiesHelperToWriteTrk
{
public:
  PropertiesHelperToWriteTrk(vtkCellData*, vtkCellData*);

  void Save(const unsigned int);

private:
  unsigned int m_NbProperties;
  Properties m_OldProperties;
  Properties m_NewProperties;
};

void FIBERBUNDLE_EXPORT CopyPropertiesSettings(vtkPolyData*, vtkPolyData*);
unsigned int FIBERBUNDLE_EXPORT FillPropertiesFromPolydata(
  vtkPolyData*, char[10][20]);
unsigned int FIBERBUNDLE_EXPORT NbFloatsForProperties(vtkPolyData*);
void FIBERBUNDLE_EXPORT AppendPropertiesToVector(
  vtkPolyData*, const vtkIdType, std::vector<float>&);

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_TRK_PROPERTIES_HPP_INCLUDED
