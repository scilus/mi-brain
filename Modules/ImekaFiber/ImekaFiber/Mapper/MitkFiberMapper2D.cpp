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
#include <vtkOpenGLPolyDataMapper.h>
#include <vtkOpenGLHelper.h>
#include <vtkShaderProgram.h>
#include <vtkPlane.h>
#include <vtkCutter.h>
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
#include <vtkCleanPolyData.h>
#include <mitkPlaneGeometry.h>
#include <mitkSliceNavigationController.h>
#include <mitkCoreServices.h>

class vtkShaderCallback : public vtkCommand
{
public:
  static vtkShaderCallback *New()
  {
    return new vtkShaderCallback;
  }
  mitk::BaseRenderer *renderer;
  mitk::DataNode *node;

  void Execute(vtkObject *, unsigned long, void*cbo) override
  {
    vtkShaderProgram *program = reinterpret_cast<vtkShaderProgram*>(cbo);

    float fiberOpacity;
    bool fiberFading = false;
    float fiberThickness = 0.0;

    node->GetOpacity(fiberOpacity, nullptr);
    node->GetFloatProperty("Fiber2DSliceThickness", fiberThickness);
    node->GetBoolProperty("Fiber2DfadeEFX", fiberFading);

    program->SetUniformf("fiberOpacity", fiberOpacity);
    program->SetUniformi("fiberFadingON", fiberFading);
    program->SetUniformf("fiberThickness", fiberThickness);

    if (this->renderer)
    {
      //get information about current position of views
      mitk::SliceNavigationController::Pointer sliceContr = renderer->GetSliceNavigationController();
      mitk::PlaneGeometry::ConstPointer planeGeo = sliceContr->GetCurrentPlaneGeometry();

      //generate according cutting planes based on the view position
      float planeNormal[3];
      planeNormal[0] = planeGeo->GetNormal()[0];
      planeNormal[1] = planeGeo->GetNormal()[1];
      planeNormal[2] = planeGeo->GetNormal()[2];

      float tmp1 = planeGeo->GetOrigin()[0] * planeNormal[0];
      float tmp2 = planeGeo->GetOrigin()[1] * planeNormal[1];
      float tmp3 = planeGeo->GetOrigin()[2] * planeNormal[2];
      float thickness = tmp1 + tmp2 + tmp3; //attention, correct normalvector

      float a[4];
      for (int i = 0; i < 3; ++i)
        a[i] = planeNormal[i];

      a[3] = thickness;
      program->SetUniform4f("slicingPlane", a);

    }
  }

  vtkShaderCallback() { this->renderer = nullptr; }
};

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
  m_ParamsPerView[mitk::AnatomicalPlane::Axial] = true;
  m_ParamsPerView[mitk::AnatomicalPlane::Sagittal] = true;
  m_ParamsPerView[mitk::AnatomicalPlane::Coronal] = true;
}

mitk::FiberBundle* mitk::MitkFiberMapper2D::GetInput()
{
  return dynamic_cast< mitk::FiberBundle * > (GetDataNode()->GetData());
}

void mitk::MitkFiberMapper2D::Update(mitk::BaseRenderer * renderer)
{
  // MITK 2025: Force visibility to true BEFORE checking it
  GetDataNode()->SetBoolProperty("visible", true, renderer);
  GetDataNode()->SetBoolProperty("visible", true, nullptr);
  
  bool visible = true;
  GetDataNode()->GetVisibility(visible, renderer, "visible");
  if (!visible)
  {
    std::cout << "MitkFiberMapper2D::Update - per-renderer visibility false after forcing, this shouldn't happen!\n";
    return;
  }

  GetDataNode()->GetVisibility(visible, nullptr);
  if (!visible)
  {
    std::cout << "MitkFiberMapper2D::Update - global visibility false after forcing, this shouldn't happen!\n";
    return;
  }

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
  // Check current visibility before forcing
  bool currentVis = false;
  GetDataNode()->GetBoolProperty("visible", currentVis, renderer);
  std::cout << "MitkFiberMapper2D::GenerateDataForRenderer called for renderer: " << renderer->GetName() 
            << ", current per-renderer visibility=" << currentVis << "\n";
  
  // MITK 2025: Force per-renderer visibility to true on every render
  GetDataNode()->SetBoolProperty("visible", true, renderer);
  std::cout << "  Forced visibility to TRUE for 2D renderer\n";
  
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
  
  // MITK 2025 FIX: Since shaders are disabled in VTK 9, we need to actually SLICE the fibers
  // Get the slice plane from the renderer
  const mitk::PlaneGeometry* planeGeometry = renderer->GetCurrentWorldPlaneGeometry();
  if (planeGeometry)
  {
    // Create a vtkPlane from the MITK plane geometry
    mitk::Point3D origin = planeGeometry->GetOrigin();
    mitk::Vector3D normal = planeGeometry->GetNormal();
    normal.Normalize();
    
    vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
    plane->SetOrigin(origin[0], origin[1], origin[2]);
    plane->SetNormal(normal[0], normal[1], normal[2]);
    
    // Use vtkCutter to slice the fibers at this plane
    vtkSmartPointer<vtkCutter> cutter = vtkSmartPointer<vtkCutter>::New();
    cutter->SetInputData(fiberPolyData);
    cutter->SetCutFunction(plane);
    cutter->Update();
    
    // MITK 2025: Clean polydata to merge coincident points (stitching fix)
    vtkSmartPointer<vtkCleanPolyData> cleaner = vtkSmartPointer<vtkCleanPolyData>::New();
    cleaner->SetInputConnection(cutter->GetOutputPort());
    cleaner->PointMergingOn();
    cleaner->SetTolerance(1e-5);
    cleaner->SetToleranceIsAbsolute(false);
    cleaner->Update();
    
    vtkSmartPointer<vtkPolyData> cleanedData = cleaner->GetOutput();
    
    // MITK 2025: Manually decompose polylines into independent segments (2 points)
    vtkSmartPointer<vtkCellArray> newLines = vtkSmartPointer<vtkCellArray>::New();
    vtkCellArray* lines = cleanedData->GetLines();
    
    if (lines)
    {
      vtkSmartPointer<vtkIdList> idList = vtkSmartPointer<vtkIdList>::New();
      lines->InitTraversal();
      while(lines->GetNextCell(idList))
      {
        for(vtkIdType i=0; i < idList->GetNumberOfIds()-1; ++i)
        {
          newLines->InsertNextCell(2);
          newLines->InsertCellPoint(idList->GetId(i));
          newLines->InsertCellPoint(idList->GetId(i+1));
        }
      }
    }
    
    vtkSmartPointer<vtkPolyData> decomposedData = vtkSmartPointer<vtkPolyData>::New();
    decomposedData->SetPoints(cleanedData->GetPoints());
    decomposedData->GetPointData()->PassData(cleanedData->GetPointData());
    decomposedData->SetLines(newLines);
    
    // MITK 2025: Critical fix - include Verts to render points that didn't form lines
    if (cleanedData->GetVerts() && cleanedData->GetVerts()->GetNumberOfCells() > 0)
    {
      decomposedData->SetVerts(cleanedData->GetVerts());
    }
    
    // Set the sliced output to the mapper
    localStorage->m_Mapper->SetInputData(decomposedData);
    std::cout << "  Sliced and decomposed fibers at plane: " << decomposedData->GetNumberOfPoints() << " points\n";
  }
  else
  {
    // Fallback: no slicing
    localStorage->m_Mapper->SetInputData(fiberPolyData);
    std::cout << "  No plane geometry - showing full 3D fibers\n";
  }
  
  localStorage->m_Mapper->ScalarVisibilityOn();
  localStorage->m_Mapper->SetScalarModeToUsePointFieldData();
  localStorage->m_Mapper->SetLookupTable(m_lut);  //apply the properties after the slice was set
  localStorage->m_Actor->GetProperty()->SetOpacity(0.999);
  localStorage->m_Mapper->SelectColorArray("FIBER_COLORS");
  
  // MITK 2025: Force VTK actor visibility
  localStorage->m_Actor->SetVisibility(1);
  std::cout << "  VTK Actor visibility forced to: " << localStorage->m_Actor->GetVisibility() << "\n";

  // TODO: VTK 9.4 shader API changed - need to use new AddShaderReplacement API
  /*
  localStorage->m_Mapper->SetVertexShaderCode(
    "//VTK::System::Dec\n"
    "attribute vec4 vertexMC;\n"

    "//VTK::Normal::Dec\n"
    "uniform mat4 MCDCMatrix;\n"

    "//VTK::Color::Dec\n"

    "varying vec4 positionWorld;\n"
    "varying vec4 colorVertex;\n"

    "void main(void)\n"
    "{\n"
    "  colorVertex = scalarColor;\n"
    "  positionWorld = vertexMC;\n"
    "  gl_Position = MCDCMatrix * vertexMC;\n"
    "}\n"
  );

  localStorage->m_Mapper->SetFragmentShaderCode(
    "//VTK::System::Dec\n"  // always start with this line
    "//VTK::Output::Dec\n"  // always have this line in your FS
    "uniform vec4 slicingPlane;\n"
    "uniform float fiberThickness;\n"
    "uniform int fiberFadingON;\n"
    "uniform float fiberOpacity;\n"

    "varying vec4 positionWorld;\n"
    "varying vec4 colorVertex;\n"
    "out vec4 out_Color;\n"

    "void main(void)\n"
    "{\n"
    "  float r1 = dot(positionWorld.xyz, slicingPlane.xyz) - slicingPlane.w;\n"

    "  if (abs(r1) >= fiberThickness)\n"
    "    discard;\n"

    "  if (fiberFadingON != 0)\n"
    "  {\n"
    "    float x = (r1 + fiberThickness) / (fiberThickness*2.0);\n"
    "    x = 1.0 - x;\n"
    "    out_Color = vec4(colorVertex.xyz*x, fiberOpacity);\n"
    "  }\n"
    "  else{\n"
    "    out_Color = vec4(colorVertex.xyz, fiberOpacity);\n"
    "  }\n"
    "}\n"
  );
  */

  // VTK 9.4: Shader callback system changed
  /*
  vtkSmartPointer<vtkShaderCallback> myCallback = vtkSmartPointer<vtkShaderCallback>::New();
  myCallback->renderer = renderer;
  myCallback->node = this->GetDataNode();
  localStorage->m_Mapper->AddObserver(vtkCommand::UpdateShaderEvent, myCallback);
  */

  localStorage->m_Actor->SetMapper(localStorage->m_Mapper);
  localStorage->m_Actor->GetProperty()->SetLineWidth(m_LineWidth);

  // We have been modified => save this for next Update()
  localStorage->m_LastUpdateTime.Modified();
}

vtkProp* mitk::MitkFiberMapper2D::GetVtkProp(mitk::BaseRenderer *renderer)
{
  std::cout << "MitkFiberMapper2D::GetVtkProp called for renderer: " << renderer->GetName() << "\n";
  this->Update(renderer);
  vtkActor* actor = m_LocalStorageHandler.GetLocalStorage(renderer)->m_Actor;
  std::cout << "  Returning actor with visibility: " << actor->GetVisibility() << "\n";
  return actor;
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
  m_Mapper = vtkSmartPointer<vtkOpenGLPolyDataMapper>::New();
}
