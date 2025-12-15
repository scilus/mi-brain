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

#ifndef FiberBundleMapper3D_H_HEADER_INCLUDED
#define FiberBundleMapper3D_H_HEADER_INCLUDED

#include <mitkVtkMapper.h>
#include "FiberBundle/mitkFiberBundle.h"
#include "vtkFiberMapper.hpp"
#include <vtkSmartPointer.h>
class vtkPropAssembly;
class vtkPolyDataMapper;
class vtkLookupTable;
class vtkOpenGLActor;

#include "MapperData.hpp"

namespace mitk {

  //##Documentation
  //## @brief Mapper for FiberBundle
  //## @ingroup Mapper

  class MitkFiberMapper3D : public VtkMapper
  {
  public:

    mitkClassMacro(MitkFiberMapper3D, VtkMapper)
    itkFactorylessNewMacro(Self)
    itkCloneMacro(Self)

    const FiberBundle* GetInput();
    vtkProp *GetVtkProp(mitk::BaseRenderer *renderer) override; //looks like depricated.. should be replaced bz GetViewProp()
    static void SetDefaultProperties(DataNode* node, BaseRenderer* renderer = nullptr, bool overwrite = false);
    void GenerateDataForRenderer(mitk::BaseRenderer* renderer) override;

    void SetFiberMapperData(Imeka::Fiber::FiberMapperData* data);
    void UpdateIndices() { m_UpdateIndices = true; }

    class  LocalStorage3D : public mitk::Mapper::BaseLocalStorage
    {
    public:
      vtkSmartPointer<vtkActor> m_FiberActor;
      vtkSmartPointer<vtkFiberMapper> m_FiberMapper;
      vtkSmartPointer<vtkPropAssembly> m_FiberAssembly;

      itk::TimeStamp m_LastUpdateTime;
      LocalStorage3D();

      ~LocalStorage3D() override
      {
      }
    };

    /** \brief This member holds all three LocalStorages for the 3D render window(s). */
    mitk::LocalStorageHandler<LocalStorage3D> m_LocalStorageHandler;


  protected:

    MitkFiberMapper3D();
    ~MitkFiberMapper3D() override;
    void InternalGenerateData(mitk::BaseRenderer *renderer);

    void UpdateShaderParameter(mitk::BaseRenderer*);

    Imeka::Fiber::FiberMapperData* m_FiberMapperData;

  private:
    bool m_UpdateIndices;
    vtkSmartPointer<vtkLookupTable> m_lut;
    float   m_TubeRadius;
    int     m_TubeSides;
    int     m_LineWidth;
    float   m_RibbonWidth;
    bool    m_Lighting;
    vtkSmartPointer<vtkPolyData> m_FiberPolyData;
    mitk::FiberBundle* m_FiberBundle;
  };

} // end namespace mitk

#endif /* FiberBundleMapper3D_H_HEADER_INCLUDED */
