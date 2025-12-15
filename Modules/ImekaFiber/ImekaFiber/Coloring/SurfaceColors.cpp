
#include "SurfaceColors.hpp"

#include <vtkPointData.h>
#include <vtkPolyData.h>

#include <mitkRenderingManager.h>

#include "ImekaCommon/Colors.hpp"

namespace Imeka
{

namespace Fiber
{

SurfaceColors::SurfaceColors(
  Imeka::DataManager& DM,
  Imeka::Callback& callback,
  NodeDataMap& fibersNodeData)
  : m_DM(DM)
  , m_Callback(callback)
  , m_FibersNodeData(fibersNodeData)
  , m_AnatNode(nullptr)
{}

void SurfaceColors::SetAnat(mitk::DataNode* node)
{
  m_AnatNode = node;
}

void SurfaceColors::SetSurfaceActions(mitk::DataNode* node)
{
  // Uniform and Shuffle are handled here. This is the normal MITK property.
  m_Callback.Add("color", node, [this](mitk::DataNode* node)
  {
    node->SetBoolProperty("scalar visibility", false);
    node->Modified();

    node->SetIntProperty("ColorType", -1);
  });
//   m_Callback.Add("ColorType", -1, node, [this](mitk::DataNode* node)
//   {
//     int colorTypeInt = 0;
//     node->GetIntProperty("ColorType", colorTypeInt);
//     if (colorTypeInt == -1) { return; }
//
//     const auto coloring = static_cast<SurfaceColorTypes>(colorTypeInt);
//     if (coloring == FILE)
//     {
//       ColorFromFile(node);
//     }
//     else if (coloring == MAP)
//     {
//       ColorFromMap(node);
//     }
//     else if (coloring == FIBER)
//     {
//       ColorFromFibers(node);
//     }
//
//     node->SetIntProperty("ColorType", -1);
//     node->Modified();
//   });
}

void SurfaceColors::ColorFromFile(mitk::DataNode*)
{
//   auto surface = dynamic_cast<mitk::Surface*>(node->GetData());
//   auto polydata = surface->GetVtkPolyData();
//   polydata->GetPointData()->SetActiveScalars(
//     polydata->GetPointData()->GetArrayName(0));
//   node->SetColor(1.0, 1.0, 1.0); // bug when color+scalar(mix both)
//   node->SetBoolProperty("scalar visibility", true);
}

void SurfaceColors::ColorFromMap(mitk::DataNode*)
{

}

void SurfaceColors::ColorFromFibers(mitk::DataNode*)
{

}

} // namespace Fiber

} // namespace Imeka
