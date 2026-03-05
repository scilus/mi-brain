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

#include "mitkFiberShaderController.h"

#include <vtkShaderProgram.h>
#include <mitkSliceNavigationController.h>
#include <mitkPlaneGeometry.h>
#include <mitkProperties.h>

namespace mitk {

  mitkFiberShaderController::mitkFiberShaderController()
    : m_Renderer(nullptr)
    , m_DataNode(nullptr)
  {
  }

  mitkFiberShaderController::~mitkFiberShaderController()
  {
  }

  mitkFiberShaderController* mitkFiberShaderController::New()
  {
    return new mitkFiberShaderController();
  }

  void mitkFiberShaderController::SetRenderer(mitk::BaseRenderer* renderer)
  {
    m_Renderer = renderer;
  }

  void mitkFiberShaderController::SetDataNode(mitk::DataNode* node)
  {
    m_DataNode = node;
  }

  void mitkFiberShaderController::Execute(vtkObject*, unsigned long, void* callData)
  {
    vtkShaderProgram* program = reinterpret_cast<vtkShaderProgram*>(callData);

    if (!m_DataNode || !m_Renderer)
      return;

    float fiberOpacity = 1.0f;
    bool fiberFading = false;
    float fiberThickness = 0.0f;

    m_DataNode->GetOpacity(fiberOpacity, nullptr);
    m_DataNode->GetFloatProperty("Fiber2DSliceThickness", fiberThickness);
    m_DataNode->GetBoolProperty("Fiber2DfadeEFX", fiberFading);

    program->SetUniformf("fiberOpacity", fiberOpacity);
    program->SetUniformi("fiberFadingON", fiberFading);
    program->SetUniformf("fiberThickness", fiberThickness);

    mitk::SliceNavigationController::Pointer sliceContr = m_Renderer->GetSliceNavigationController();
    mitk::PlaneGeometry::ConstPointer planeGeo = sliceContr->GetCurrentPlaneGeometry();

    if (planeGeo.IsNotNull())
    {
      float planeNormal[3];
      planeNormal[0] = planeGeo->GetNormal()[0];
      planeNormal[1] = planeGeo->GetNormal()[1];
      planeNormal[2] = planeGeo->GetNormal()[2];

      float d = planeGeo->GetOrigin()[0] * planeNormal[0] +
                planeGeo->GetOrigin()[1] * planeNormal[1] +
                planeGeo->GetOrigin()[2] * planeNormal[2];

      float slicingPlane[4];
      slicingPlane[0] = planeNormal[0];
      slicingPlane[1] = planeNormal[1];
      slicingPlane[2] = planeNormal[2];
      slicingPlane[3] = d;

      program->SetUniform4f("slicingPlane", slicingPlane);
    }
  }

  std::string mitkFiberShaderController::GetVertexShaderCode()
  {
    return
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
      "}\n";
  }

  std::string mitkFiberShaderController::GetFragmentShaderCode()
  {
    return
      "//VTK::System::Dec\n"
      "//VTK::Output::Dec\n"
      "uniform vec4 slicingPlane;\n"
      "uniform float fiberThickness;\n"
      "uniform int fiberFadingON;\n"
      "uniform float fiberOpacity;\n"
      "varying vec4 positionWorld;\n"
      "varying vec4 colorVertex;\n"
      "out vec4 out_Color;\n"
      "void main(void)\n"
      "{\n"
      "  float dist = dot(positionWorld.xyz, slicingPlane.xyz) - slicingPlane.w;\n"
      "  if (abs(dist) >= fiberThickness)\n"
      "    discard;\n"
      "  if (fiberFadingON != 0)\n"
      "  {\n"
      "    float x = (dist + fiberThickness) / (fiberThickness * 2.0);\n"
      "    x = 1.0 - x;\n"
      "    out_Color = vec4(colorVertex.xyz * x, fiberOpacity);\n"
      "  }\n"
      "  else\n"
      "  {\n"
      "    out_Color = vec4(colorVertex.xyz, fiberOpacity);\n"
      "  }\n"
      "}\n";
  }

} // namespace mitk
