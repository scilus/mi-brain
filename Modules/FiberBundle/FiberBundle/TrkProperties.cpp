#include "TrkProperties.hpp"

#include <vtkCellData.h>
#include <vtkPolyData.h>

namespace Imeka
{

namespace Fiber
{

PropertiesHelperToReadTrk::PropertiesHelperToReadTrk(
  const unsigned int nbRawProperties,
  char propertyName[10][20])
    : m_NbRawProperties(nbRawProperties)
{
  unsigned int lastValid = 0;
  unsigned int nextValid = 0;
  for (unsigned int i = 0; i < m_NbRawProperties; ++i)
  {
    if (strcmp(propertyName[i], "") != 0)
    {
      m_Properties[nextValid] = vtkSmartPointer<vtkFloatArray>::New();
      m_Properties[nextValid]->SetName(propertyName[i]);
      lastValid = nextValid++;
    }
    else
    {
      m_Properties[lastValid]->SetNumberOfComponents(i - lastValid + 1);
    }
  }
}

void PropertiesHelperToReadTrk::Save(float* values)
{
  for (unsigned int i = 0; i < m_NbRawProperties; /**/)
  {
    m_Properties[i]->InsertNextTypedTuple(&values[i]);
    i += m_Properties[i]->GetNumberOfComponents();
  }
}

void PropertiesHelperToReadTrk::StoreInPolydata(vtkPolyData* polydata)
{
  auto cellData = polydata->GetCellData();
  for (unsigned int i = 0; i < m_NbRawProperties; /**/)
  {
    cellData->AddArray(m_Properties[i]);
    i += m_Properties[i]->GetNumberOfComponents();
  }
}

PropertiesHelperToWriteTrk::PropertiesHelperToWriteTrk(
  vtkCellData* oldProps, vtkCellData* newProps)
{
  m_NbProperties = oldProps->GetNumberOfArrays();
  for (unsigned int i = 0; i < m_NbProperties; ++i)
  {
    m_OldProperties[i] = vtkFloatArray::SafeDownCast(oldProps->GetArray(i));
    m_NewProperties[i] = vtkFloatArray::SafeDownCast(newProps->GetArray(i));
  }
}

void PropertiesHelperToWriteTrk::Save(const unsigned int cellID)
{
  float values[10];
  for (unsigned int i = 0; i < m_NbProperties; ++i)
  {
    m_OldProperties[i]->GetTypedTuple(cellID, values);
    m_NewProperties[i]->InsertNextTypedTuple(values);
  }
}

void CopyPropertiesSettings(
  vtkPolyData* oldPolydata, vtkPolyData* newPolydata)
{
  auto oldProperties = oldPolydata->GetCellData();
  auto newProperties = newPolydata->GetCellData();
  for (int i = 0; i < oldProperties->GetNumberOfArrays(); ++i)
  {
    auto oldArray = oldProperties->GetAbstractArray(i);
    auto newArray = vtkSmartPointer<vtkFloatArray>::New();
    newArray->SetName(oldArray->GetName());
    newArray->SetNumberOfComponents(oldArray->GetNumberOfComponents());
    newProperties->AddArray(newArray);
  }
}

unsigned int FillPropertiesFromPolydata(
  vtkPolyData* polydata,
  char propertyName[10][20])
{
  unsigned int atProperty = 0;
  auto cellData = polydata->GetCellData();
  for (int i = 0; i < cellData->GetNumberOfArrays(); ++i)
  {
    auto arr = cellData->GetArray(i);
    const unsigned int nb = arr->GetNumberOfComponents();
    strcpy(propertyName[atProperty], arr->GetName());
    if (nb > 1)
    {
      propertyName[atProperty][strlen(arr->GetName()) + 1] = '0' + nb;
    }
    atProperty += nb;
  }
  return atProperty;
}

unsigned int NbFloatsForProperties(vtkPolyData* polydata)
{
  unsigned int nbFloats = 0;
  auto cellData = polydata->GetCellData();
  for (int i = 0; i < cellData->GetNumberOfArrays(); ++i)
  {
    nbFloats += cellData->GetArray(i)->GetNumberOfComponents();
  }
  return nbFloats;
}

void AppendPropertiesToVector(
  vtkPolyData* polydata,
  const vtkIdType idxFiber,
  std::vector<float>& v)
{
  float typedTuple[10];
  auto cellData = polydata->GetCellData();
  for (int i = 0; i < cellData->GetNumberOfArrays(); ++i)
  {
    vtkSmartPointer<vtkFloatArray> arr =
      vtkFloatArray::SafeDownCast(cellData->GetArray(i));
    arr->GetTypedTuple(idxFiber, typedTuple);
    for (int j = 0; j < arr->GetNumberOfComponents(); ++j)
    {
      v.push_back(typedTuple[j]);
    }
  }
}

} // namespace Fiber

} // namespace Imeka
