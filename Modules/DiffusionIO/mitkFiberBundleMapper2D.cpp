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

#include "mitkFiberBundleMapper2D.h"
#include "mitkBaseRenderer.h"
#include "mitkDataNode.h"
#include <vtkActor.h>
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkOpenGLHelper.h>
#include <vtkShaderProgram.h>
#include <vtkCutter.h>
#include <vtkPlane.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
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

// VTK 9 Migration: vtkShaderCallback removed as we switched to geometric slicing via vtkCutter

mitk::FiberBundleMapper2D::FiberBundleMapper2D()
  : m_LineWidth(1)
{
  m_lut = vtkSmartPointer<vtkLookupTable>::New();
  m_lut->Build();

}

mitk::FiberBundleMapper2D::~FiberBundleMapper2D()
{
}


mitk::FiberBundle* mitk::FiberBundleMapper2D::GetInput()
{
  return dynamic_cast< mitk::FiberBundle * > ( GetDataNode()->GetData() );
}



void mitk::FiberBundleMapper2D::Update(mitk::BaseRenderer * renderer)
{
  bool visible = true;
  GetDataNode()->GetVisibility(visible, renderer, "visible");
  if ( !visible )
    return;

  // Calculate time step of the input data for the specified renderer (integer value)
  // this method is implemented in mitkMapper
  this->CalculateTimeStep( renderer );

  //check if updates occured in the node or on the display
  FBXLocalStorage *localStorage = m_LocalStorageHandler.GetLocalStorage(renderer);

  //set renderer independent shader properties
  const DataNode::Pointer node = this->GetDataNode();
  float thickness = 2.0;
  if(!this->GetDataNode()->GetPropertyValue("Fiber2DSliceThickness",thickness))
    MITK_INFO << "FIBER2D SLICE THICKNESS PROPERTY ERROR";

  bool fiberfading = false;
  if(!this->GetDataNode()->GetPropertyValue("Fiber2DfadeEFX",fiberfading))
    MITK_INFO << "FIBER2D SLICE FADE EFX PROPERTY ERROR";

  mitk::FiberBundle* fiberBundle = this->GetInput();
  if (fiberBundle==nullptr)
    return;

  int lineWidth = 1.0;
  node->GetIntProperty("LineWidth", lineWidth);
  if (m_LineWidth!=lineWidth)
  {
    m_LineWidth = lineWidth;
    fiberBundle->RequestUpdate2D();
  }

  vtkProperty *property = localStorage->m_Actor->GetProperty();
  property->SetLighting(false);

  if ( localStorage->m_LastUpdateTime<renderer->GetCurrentWorldPlaneGeometryUpdateTime() || localStorage->m_LastUpdateTime<fiberBundle->GetUpdateTime2D() )
  {
    this->GenerateDataForRenderer( renderer );
  }
}

void mitk::FiberBundleMapper2D::UpdateShaderParameter(mitk::BaseRenderer *)
{
  // VTK 9 Migration: No longer using shaders, this method is obsolete
}

// vtkActors and Mappers are feeded here
void mitk::FiberBundleMapper2D::GenerateDataForRenderer(mitk::BaseRenderer *renderer)
{
  mitk::FiberBundle* fiberBundle = this->GetInput();
  if (fiberBundle == nullptr)
    return;

  //the handler of local storage gets feeded in this method with requested data for related renderwindow
  FBXLocalStorage *localStorage = m_LocalStorageHandler.GetLocalStorage(renderer);

  mitk::DataNode* node = this->GetDataNode();
  if (node == nullptr)
    return;

  // MITK 2025: Ensure visibility is true for this renderer
  node->SetBoolProperty("visible", true, renderer);
  
  // Get current world plane geometry
  const mitk::PlaneGeometry* planeGeo = renderer->GetCurrentWorldPlaneGeometry();
  if (planeGeo == nullptr)
    return;

  // Update slicing plane
  mitk::Point3D planeOrigin = planeGeo->GetOrigin();
  mitk::Vector3D planeNormal = planeGeo->GetNormal();
  
  localStorage->m_SlicingPlane->SetOrigin(planeOrigin.GetDataPointer());
  localStorage->m_SlicingPlane->SetNormal(planeNormal.GetDataPointer());

  vtkSmartPointer<vtkPolyData> fiberPolyData = fiberBundle->GetFiberPolyData();
  if (fiberPolyData == nullptr)
    return;

  fiberPolyData->GetPointData()->AddArray(fiberBundle->GetFiberColors());
  
  // Setup Cutter
  localStorage->m_Cutter->SetInputData(fiberPolyData);
  localStorage->m_Cutter->SetCutFunction(localStorage->m_SlicingPlane);
  
  // Setup Mapper with Cutter output
  localStorage->m_Mapper->ScalarVisibilityOn();
  localStorage->m_Mapper->SetScalarModeToUsePointFieldData();
  localStorage->m_Mapper->SetLookupTable(m_lut);  //apply the properties after the slice was set
  localStorage->m_Mapper->SelectColorArray("FIBER_COLORS");
  localStorage->m_Mapper->SetInputConnection(localStorage->m_Cutter->GetOutputPort());

  localStorage->m_Actor->SetMapper(localStorage->m_Mapper);
  
  float opacity = 1.0f;
  node->GetOpacity(opacity, renderer);
  localStorage->m_Actor->GetProperty()->SetOpacity(opacity);

  // Set visual properties
  float thickness = 1.0f;
  node->GetFloatProperty("Fiber2DSliceThickness", thickness);
  if (thickness < 1.0f) thickness = 1.0f;
  
  localStorage->m_Actor->GetProperty()->SetPointSize(thickness);
  localStorage->m_Actor->GetProperty()->SetLineWidth(m_LineWidth);
  localStorage->m_Actor->GetProperty()->SetLighting(false);

  // We have been modified => save this for next Update()
  localStorage->m_LastUpdateTime.Modified();
}


vtkProp* mitk::FiberBundleMapper2D::GetVtkProp(mitk::BaseRenderer *renderer)
{
  this->Update(renderer);
  return m_LocalStorageHandler.GetLocalStorage(renderer)->m_Actor;
}


void mitk::FiberBundleMapper2D::SetDefaultProperties(mitk::DataNode* node, mitk::BaseRenderer* renderer, bool overwrite)
{
  Superclass::SetDefaultProperties(node, renderer, overwrite);

  // MITK 2025: Superclass may have set visible=false, explicitly set it to true
  node->SetBoolProperty("visible", true, renderer);
  std::cout << "FiberBundleMapper2D::SetDefaultProperties - Set visibility=true for renderer: " 
            << (renderer ? renderer->GetName() : "nullptr") << "\n";
  //    node->SetProperty("shader",mitk::ShaderProperty::New("mitkShaderFiberClipping"));

  //add other parameters to propertylist
  node->AddProperty( "Fiber2DSliceThickness", mitk::FloatProperty::New(1.0f), renderer, overwrite );
  node->AddProperty( "Fiber2DfadeEFX", mitk::BoolProperty::New(true), renderer, overwrite );
  node->AddProperty( "color", mitk::ColorProperty::New(1.0,1.0,1.0), renderer, overwrite);
}

// MITK 2025: Override to bypass visibility check that may incorrectly return false
void mitk::FiberBundleMapper2D::MitkRenderOpaqueGeometry(BaseRenderer *renderer)
{
  std::cout << "FiberBundleMapper2D::MitkRenderOpaqueGeometry called for renderer: " << renderer->GetName() << "\n";
  if (this->GetVtkProp(renderer)->GetVisibility())
  {
    std::cout << "  Rendering opaque 2D fibers...\n";
    GetVtkProp(renderer)->RenderOpaqueGeometry(renderer->GetVtkRenderer());
  }
}

void mitk::FiberBundleMapper2D::MitkRenderTranslucentGeometry(BaseRenderer *renderer)
{
  std::cout << "FiberBundleMapper2D::MitkRenderTranslucentGeometry called for renderer: " << renderer->GetName() << "\n";
  if (this->GetVtkProp(renderer)->GetVisibility())
  {
    std::cout << "  Rendering translucent 2D fibers...\n";
    GetVtkProp(renderer)->RenderTranslucentPolygonalGeometry(renderer->GetVtkRenderer());
  }
}

mitk::FiberBundleMapper2D::FBXLocalStorage::FBXLocalStorage()
{
  m_Actor = vtkSmartPointer<vtkActor>::New();
  m_Mapper = vtkSmartPointer<MITKFIBERBUNDLEMAPPER2D_POLYDATAMAPPER>::New();
  m_Cutter = vtkSmartPointer<vtkCutter>::New();
  m_SlicingPlane = vtkSmartPointer<vtkPlane>::New();
}
