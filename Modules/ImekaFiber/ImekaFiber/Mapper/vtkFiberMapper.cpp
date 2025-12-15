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

bool vtkFiberMapper::GetNeedToRebuildBufferObjects(vtkRenderer *ren, vtkActor *)
{
  // We only send the complete points data the first time. On all other times, it's totally
  // useless and wasteful. Even when filtering, we only update the IBO.
  auto polyData = this->CurrentInput;
  if (m_FirstTime || m_FiberMapperData->fiberBundle->IsRTT())
  {
    m_FirstTime = false;
    return true;
  }

  // The following blocks shouldn't be here! They were originally in
  // `BuildBufferObjects` but we only call that method once so some buffers
  // (like the colors) are not updated anymore. Because it must be in a
  // function that receives a `vtkRenderer`, I can't put it where I want.
  vtkOpenGLRenderWindow *renWin = vtkOpenGLRenderWindow::SafeDownCast(ren->GetRenderWindow());
  vtkOpenGLVertexBufferObjectCache *cache = renWin->GetVBOCache();
  auto vbos = this->VBOs;

  // Some actions update the points position (mirror, ?), but this is rare and heavy!
  if (this->VBOBuildTime < polyData->GetMTime())
  {
    vbos->CacheDataArray("vertexMC", polyData->GetPoints()->GetData(), cache, VTK_FLOAT);
  }

  // If the colors haven't been updated, the next line do almost nothing.
  vbos->CacheDataArray("scalarColor", this->Colors, cache, VTK_UNSIGNED_CHAR);

  vbos->BuildAllVBOs(cache);
  vbos->ClearAllDataArrays();
  this->VBOBuildTime.Modified();

  return false;
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
