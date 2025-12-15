#ifndef IMEKA_FIBER_MAPPER_HPP_INCLUDED
#define IMEKA_FIBER_MAPPER_HPP_INCLUDED

#include "vtkOpenGLPolyDataMapper.h"

#include "MapperData.hpp"
#include "ImekaFiberExports.h"

class ImekaFiber_EXPORT vtkFiberMapper : public vtkOpenGLPolyDataMapper
{
public:
  static vtkFiberMapper* New();
  vtkTypeMacro(vtkFiberMapper, vtkPolyDataMapper)

  void SetFiberMapperData(const Imeka::Fiber::FiberMapperData* data);
  void UpdateIBO();

protected:
  vtkFiberMapper();
  ~vtkFiberMapper() override {};

  /**
  * Does the VBO/IBO need to be rebuilt
  */
  virtual bool GetNeedToRebuildBufferObjects(vtkRenderer *ren, vtkActor *act);

  /**
  * Build the IBO, called by BuildBufferObjects
  */
  virtual void BuildIBO(vtkRenderer *ren, vtkActor *act, vtkPolyData *poly);

  const Imeka::Fiber::FiberMapperData* m_FiberMapperData;

private:
  bool m_FirstTime;
};

#endif // IMEKA_FIBER_MAPPER_HPP_INCLUDED
