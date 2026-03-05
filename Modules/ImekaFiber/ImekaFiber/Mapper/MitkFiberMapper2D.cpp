/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#include "MitkFiberMapper2D.hpp"
#include "mitkBaseRenderer.h"
#include "mitkDataNode.h"
#include <vtkActor.h>
#include "vtkFiberMapper.hpp"
#include <vtkOpenGLHelper.h>
#include <vtkShaderProgram.h>
#include <vtkPlane.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkShaderProperty.h>
#include <vtkProperty.h>
#include <vtkLookupTable.h>
#include <vtkPoints.h>
#include <vtkCamera.h>
#include <vtkPolyLine.h>
#include <vtkRenderer.h>
#include <vtkCellArray.h>
#include <vtkMatrix4x4.h>
#include <vtkTubeFilter.h>
#include <mitkPlaneGeometry.h>
#include <mitkSliceNavigationController.h>
#include <mitkCoreServices.h>

#include "mitkFiberShaderController.h"

mitk::MitkFiberMapper2D::MitkFiberMapper2D()
  : m_UpdateIndices(false)
  , m_LineWidth(1)
{
  m_lut = vtkSmartPointer<vtkLookupTable>::New();
  m_lut->Build();
}

mitk::MitkFiberMapper2D::~MitkFiberMapper2D()
{}

void mitk::MitkFiberMapper2D::SetFiberMapperData(Imeka::Fiber::FiberMapperData* data)
{
  m_FiberMapperData = data;
}

void mitk::MitkFiberMapper2D::UpdateIndices()
{
  m_ParamsPerView[(int)mitk::AnatomicalPlane::Axial] = true;
  m_ParamsPerView[(int)mitk::AnatomicalPlane::Sagittal] = true;
  m_ParamsPerView[(int)mitk::AnatomicalPlane::Coronal] = true;
  m_UpdateIndices = true;
}
mitk::FiberBundle* mitk::MitkFiberMapper2D::GetInput()
{
  return dynamic_cast< mitk::FiberBundle * > (GetDataNode()->GetData());
}

void mitk::MitkFiberMapper2D::Update(mitk::BaseRenderer * renderer)
{
  GetDataNode()->SetVisibility(true, renderer, "visible");
  GetDataNode()->SetVisibility(true, nullptr);
  bool visible = true;
  GetDataNode()->GetVisibility(visible, renderer, "visible");
  if (!visible)
    return;

  GetDataNode()->GetVisibility(visible, nullptr);
  if (!visible)
    return;

  // Calculate time step of the input data for the specified renderer (integer value)
  // this method is implemented in mitkMapper
  this->CalculateTimeStep(renderer);

  //check if updates occured in the node or on the display
  FBXLocalStorage *localStorage = m_LocalStorageHandler.GetLocalStorage(renderer);

  //set renderer independent shader properties
  const DataNode::Pointer node = this->GetDataNode();
  float thickness = 2.0;
  if (!this->GetDataNode()->GetPropertyValue("Fiber2DSliceThickness", thickness))
    MITK_INFO << "FIBER2D SLICE THICKNESS PROPERTY ERROR";

  bool fiberfading = false;
  if (!this->GetDataNode()->GetPropertyValue("Fiber2DfadeEFX", fiberfading))
    MITK_INFO << "FIBER2D SLICE FADE EFX PROPERTY ERROR";

  mitk::FiberBundle* fiberBundle = this->GetInput();
  if (fiberBundle == nullptr)
    return;

  int lineWidth = 1.0;
  node->GetIntProperty("LineWidth", lineWidth);
  if (m_LineWidth != lineWidth)
  {
    m_LineWidth = lineWidth;
    fiberBundle->RequestUpdate2D();
  }

  vtkProperty *property = localStorage->m_Actor->GetProperty();
  property->SetLighting(false);

  localStorage->m_Mapper->SetFiberMapperData(m_FiberMapperData);
  const auto view = renderer->GetSliceNavigationController()->GetDefaultViewDirection();
  auto& updateIndices = m_ParamsPerView[(int)view];
  if (updateIndices)
  {
    localStorage->m_Mapper->UpdateIBO();
    updateIndices = false;
  }

  if (localStorage->m_LastUpdateTime<renderer->GetCurrentWorldPlaneGeometryUpdateTime() || localStorage->m_LastUpdateTime<fiberBundle->GetUpdateTime2D())
  {
    this->UpdateShaderParameter(renderer);
    this->GenerateDataForRenderer(renderer);
  }
}

void mitk::MitkFiberMapper2D::UpdateShaderParameter(mitk::BaseRenderer *)
{
  // see new vtkShaderCallback
}

// vtkActors and Mappers are feeded here
void mitk::MitkFiberMapper2D::GenerateDataForRenderer(mitk::BaseRenderer *renderer)
{
  GetDataNode()->SetVisibility(true, renderer, "visible");
  mitk::FiberBundle* fiberBundle = this->GetInput();

  //the handler of local storage gets feeded in this method with requested data for related renderwindow
  FBXLocalStorage *localStorage = m_LocalStorageHandler.GetLocalStorage(renderer);

  mitk::DataNode* node = this->GetDataNode();
  if (node == nullptr)
    return;

  vtkSmartPointer<vtkPolyData> fiberPolyData = fiberBundle->GetFiberPolyData();
  if (fiberPolyData == nullptr)
    return;

  fiberPolyData->GetPointData()->AddArray(fiberBundle->GetFiberColors());
  localStorage->m_Mapper->ScalarVisibilityOn();
  localStorage->m_Mapper->SetScalarModeToUsePointFieldData();
  localStorage->m_Mapper->SetLookupTable(m_lut);  //apply the properties after the slice was set
  localStorage->m_Actor->GetProperty()->SetOpacity(0.999);
  localStorage->m_Mapper->SelectColorArray("FIBER_COLORS");
  localStorage->m_Mapper->SetInputData(fiberPolyData);

  localStorage->m_Actor->GetShaderProperty()->SetVertexShaderCode(mitkFiberShaderController::GetVertexShaderCode().c_str());
  localStorage->m_Actor->GetShaderProperty()->SetFragmentShaderCode(mitkFiberShaderController::GetFragmentShaderCode().c_str());

  vtkSmartPointer<mitkFiberShaderController> myCallback = vtkSmartPointer<mitkFiberShaderController>::New();
  myCallback->SetRenderer(renderer);
  myCallback->SetDataNode(this->GetDataNode());
  localStorage->m_Mapper->AddObserver(vtkCommand::UpdateShaderEvent, myCallback);

  localStorage->m_Actor->SetMapper(localStorage->m_Mapper);
  localStorage->m_Actor->GetProperty()->SetLineWidth(m_LineWidth);

  // We have been modified => save this for next Update()
  localStorage->m_LastUpdateTime.Modified();
}

vtkProp* mitk::MitkFiberMapper2D::GetVtkProp(mitk::BaseRenderer *renderer)
{
  this->Update(renderer);
  return m_LocalStorageHandler.GetLocalStorage(renderer)->m_Actor;
}

void mitk::MitkFiberMapper2D::SetDefaultProperties(mitk::DataNode* node, mitk::BaseRenderer* renderer, bool overwrite)
{
  Superclass::SetDefaultProperties(node, renderer, overwrite);
  //    node->SetProperty("shader",mitk::ShaderProperty::New("mitkShaderFiberClipping"));

  //add other parameters to propertylist
  node->AddProperty("Fiber2DSliceThickness", mitk::FloatProperty::New(1.0f), renderer, overwrite);
  node->AddProperty("Fiber2DfadeEFX", mitk::BoolProperty::New(true), renderer, overwrite);
  node->AddProperty("color", mitk::ColorProperty::New(1.0, 1.0, 1.0), renderer, overwrite);
}

mitk::MitkFiberMapper2D::FBXLocalStorage::FBXLocalStorage()
{
  m_Actor = vtkSmartPointer<vtkActor>::New();
  m_Mapper = vtkSmartPointer<vtkFiberMapper>::New();
}
