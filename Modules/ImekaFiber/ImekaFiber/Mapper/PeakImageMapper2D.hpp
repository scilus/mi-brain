#ifndef IMEKA_PEAKS_MAPPER_2D_HPP_INCLUDED
#define IMEKA_PEAKS_MAPPER_2D_HPP_INCLUDED

//MITK Rendering
#include <FiberBundle/FilteredFiberBundle.hpp>
#include <mitkCommon.h>
#include <mitkVtkMapper.h>
#include "FiberBundle/mitkPeakImage.h"
#include <vtkSmartPointer.h>

#include "ImekaFiberExports.h"

class vtkActor;
class vtkOpenGLPolyDataMapper;

namespace mitk {

struct IShaderRepository;

class ImekaFiber_EXPORT PeakImageMapper2D : public VtkMapper
{

public:
    mitkClassMacro(PeakImageMapper2D, VtkMapper)
    itkFactorylessNewMacro(Self)
    itkCloneMacro(Self)

    mitk::PeakImage* GetInput();
    void SetFiberBundle(mitk::FilteredFiberBundle::Pointer f) { m_Fibers = f; }
    void Update(mitk::BaseRenderer * renderer) override;
    static void SetDefaultProperties(DataNode* node, BaseRenderer* renderer = nullptr, bool overwrite = false );
    void UpdateVtkTransform(mitk::BaseRenderer *renderer) override;
    vtkProp* GetVtkProp(mitk::BaseRenderer* renderer) override;

    class  LocalStorage : public mitk::Mapper::BaseLocalStorage
    {
    public:
        vtkSmartPointer<vtkActor> m_PointActor;
        vtkSmartPointer<vtkOpenGLPolyDataMapper> m_Mapper;

        itk::TimeStamp m_LastUpdateTime;
        LocalStorage();

        ~LocalStorage() override
        {
        }
    };

    /** \brief This member holds all three LocalStorages for the three 2D render windows. */
    mitk::LocalStorageHandler<LocalStorage> m_LocalStorageHandler;

protected:
    PeakImageMapper2D();
    ~PeakImageMapper2D() override;
    void GenerateDataForRenderer(mitk::BaseRenderer*) override;

private:
    vtkSmartPointer<vtkLookupTable> m_lut;
    mitk::FilteredFiberBundle::Pointer m_Fibers;
};

}//end namespace

#endif // IMEKA_PEAKS_MAPPER_2D_HPP_INCLUDED
