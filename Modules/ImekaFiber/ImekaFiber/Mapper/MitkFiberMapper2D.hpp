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

#ifndef FiberBundleMAPPER2D_H_HEADER_INCLUDED
#define FiberBundleMAPPER2D_H_HEADER_INCLUDED

#include <mitkCommon.h>
#include <mitkBaseRenderer.h>
#include <mitkVtkMapper.h>
#include "FiberBundle/mitkFiberBundle.h"
#include <vtkSmartPointer.h>

#include "MapperData.hpp"

class vtkActor;
class mitkBaseRenderer;
class vtkFiberMapper;
class vtkCutter;
class vtkPlane;
class vtkPolyData;

namespace mitk {

  struct IShaderRepository;

  class MitkFiberMapper2D : public VtkMapper
  {

  public:
    mitkClassMacro(MitkFiberMapper2D, VtkMapper);
    itkFactorylessNewMacro(Self)
    itkCloneMacro(Self)

    mitk::FiberBundle* GetInput();
    void Update(mitk::BaseRenderer * renderer) override;
    static void SetDefaultProperties(DataNode* node, BaseRenderer* renderer = nullptr, bool overwrite = false);
    vtkProp* GetVtkProp(mitk::BaseRenderer* renderer) override;

    void SetFiberMapperData(Imeka::Fiber::FiberMapperData* data);
    void UpdateIndices();

    class  FBXLocalStorage : public mitk::Mapper::BaseLocalStorage
    {
    public:
      vtkSmartPointer<vtkActor> m_Actor;
      vtkSmartPointer<vtkFiberMapper> m_Mapper;
      itk::TimeStamp m_LastUpdateTime;
      FBXLocalStorage();
      ~FBXLocalStorage() override { }
    };

    /** \brief This member holds all three LocalStorages for the three 2D render windows. */
    mitk::LocalStorageHandler<FBXLocalStorage> m_LocalStorageHandler;

  protected:
    MitkFiberMapper2D();
    ~MitkFiberMapper2D() override;

    /** Does the actual resampling, without rendering. */
    void GenerateDataForRenderer(mitk::BaseRenderer*) override;

    void UpdateShaderParameter(mitk::BaseRenderer*);

    Imeka::Fiber::FiberMapperData* m_FiberMapperData;
    std::map<int, bool> m_ParamsPerView;

  private:
    bool m_UpdateIndices;
    vtkSmartPointer<vtkLookupTable> m_lut;

    int     m_LineWidth;
  };


}//end namespace

#endif
