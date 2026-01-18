#include "vtkFiberMapper.hpp"

#include "vtkOpenGLIndexBufferObject.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkOpenGLVertexBufferObjectGroup.h"
#include "vtkRenderer.h"

//-----------------------------------------------------------------------------
vtkStandardNewMacro(vtkFiberMapper)

vtkFiberMapper::vtkFiberMapper()
  : m_FirstTime(true)
{}

void vtkFiberMapper::SetFiberMapperData(const Imeka::Fiber::FiberMapperData* data)
{
  m_FiberMapperData = data;
}

void vtkFiberMapper::UpdateIBO()
{
  auto& IBO = m_FiberMapperData->GetIBO();
  if (IBO.size() > 0)
  {
    this->Primitives[PrimitiveLines].IBO->Upload(
      IBO, vtkOpenGLIndexBufferObject::ElementArrayBuffer);
  }
  this->Primitives[PrimitiveLines].IBO->IndexCount = IBO.size();
}

bool vtkFiberMapper::GetNeedToRebuildBufferObjects(vtkRenderer *, vtkActor *)
{
  // [IMK MEGA FIX]
  // The original code had a fast-path that only updated VBOs, assuming the data
  // was in model coordinates (vertexMC). However, the data from the reader is in
  // world coordinates. This mismatch caused the rendering to break on the second frame.
  // By always returning true, we force the mapper to take the slow, full-rebuild path
  // every time. This path correctly handles the world-coordinate data and prevents the
  // fibers from moving to the origin. This is inefficient but correct.
  return true;
}

// We NEVER (except on RTT) want to build the IBO because the normal build is
// simply sending ALL points as indices. Our version is a) probably filtered
// and b) is called anyway after.
void vtkFiberMapper::BuildIBO(vtkRenderer *ren, vtkActor *act, vtkPolyData *poly)
{
  if (m_FiberMapperData->fiberBundle->IsRTT())
  {
    auto polyData = this->CurrentInput;
    if (this->VBOBuildTime < polyData->GetMTime())
    {
      vtkOpenGLPolyDataMapper::BuildIBO(ren, act, poly);
    }
  }
}
