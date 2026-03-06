#include "vtkFiberMapper.hpp"

#include "vtkOpenGLIndexBufferObject.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkOpenGLVertexBufferObjectGroup.h"
#include "vtkRenderer.h"

//-----------------------------------------------------------------------------
vtkStandardNewMacro(vtkFiberMapper)

vtkFiberMapper::vtkFiberMapper()
  : m_FiberMapperData(nullptr)
  , m_FirstTime(true)
{
  this->SetVBOShiftScaleMethod(vtkPolyDataMapper::DISABLE_SHIFT_SCALE);
}

void vtkFiberMapper::SetFiberMapperData(const Imeka::Fiber::FiberMapperData* data)
{
  m_FiberMapperData = data;
}

void vtkFiberMapper::UpdateIBO()
{
  if (!m_FiberMapperData)
  {
    return;
  }

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
  // We only send the complete points data the first time. On all other times, it's totally
  // useless and wasteful. Even when filtering, we only update the IBO.
  auto polyData = this->CurrentInput;

  if (m_FirstTime || (m_FiberMapperData && m_FiberMapperData->fiberBundle && m_FiberMapperData->fiberBundle->IsRTT()) || this->VBOBuildTime < polyData->GetMTime())
  {
    m_FirstTime = false;
    return true;
  }

  return false;
}

// We NEVER (except on RTT) want to build the IBO because the normal build is
// simply sending ALL points as indices. Our version is a) probably filtered
// and b) is called anyway after.
void vtkFiberMapper::BuildIBO(vtkRenderer *ren, vtkActor *act, vtkPolyData *poly)
{
  if (m_FiberMapperData && m_FiberMapperData->fiberBundle && m_FiberMapperData->fiberBundle->IsRTT())
  {
    auto polyData = this->CurrentInput;
    if (this->VBOBuildTime < polyData->GetMTime())
    {
      vtkOpenGLPolyDataMapper::BuildIBO(ren, act, poly);
    }
  }
  else
  {
    this->UpdateIBO();
  }
}
