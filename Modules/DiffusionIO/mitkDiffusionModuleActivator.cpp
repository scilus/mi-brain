#include <usModuleActivator.h>
#include <usModuleContext.h>

#include <mitkFiberBundleVtkReader.h>
#include <mitkFiberBundleTckReader.hpp>
#include <mitkFiberBundleTrackVisReader.h>
#include <mitkFiberBundleDicomReader.h>

#include <mitkFiberBundleVtkWriter.h>
#include <mitkFiberBundleTckWriter.hpp>
#include <mitkFiberBundleTrackVisWriter.h>
#include <mitkFiberBundleDicomWriter.h>

#include <mitkCoreServices.h>
#include <mitkIPropertyDescriptions.h>
#include <mitkIPropertyPersistence.h>

#include "mitkDiffusionIOMimeTypes.h"

namespace mitk
{
  /**
  \brief Registers services for segmentation module.
  */
  class DiffusionModuleActivator : public us::ModuleActivator
  {
  public:

    void Load(us::ModuleContext* context) override
    {
      m_MimeTypes = mitk::DiffusionIOMimeTypes::Get();
      for (std::vector<mitk::CustomMimeType*>::const_iterator mimeTypeIter = m_MimeTypes.begin(),
        iterEnd = m_MimeTypes.end(); mimeTypeIter != iterEnd; ++mimeTypeIter)
      {
        us::ServiceProperties props;
        mitk::CustomMimeType* mt = *mimeTypeIter;
        if (mt->GetName()==mitk::DiffusionIOMimeTypes::FIBERBUNDLE_VTK_MIMETYPE_NAME())
          props[ us::ServiceConstants::SERVICE_RANKING() ] = -3;
        else if (mt->GetName()==mitk::DiffusionIOMimeTypes::FIBERBUNDLE_TRK_MIMETYPE_NAME())
          props[ us::ServiceConstants::SERVICE_RANKING() ] = -1;
        else if (mt->GetName()==mitk::DiffusionIOMimeTypes::FIBERBUNDLE_TCK_MIMETYPE_NAME())
          props[ us::ServiceConstants::SERVICE_RANKING() ] = -2;
        else if (mt->GetName()==mitk::DiffusionIOMimeTypes::FIBERBUNDLE_DICOM_MIMETYPE_NAME())
          props[ us::ServiceConstants::SERVICE_RANKING() ] = -4;
        else
          props[ us::ServiceConstants::SERVICE_RANKING() ] = 10;

        context->RegisterService(*mimeTypeIter, props);
      }

      m_FiberBundleVtkReader = new FiberBundleVtkReader();
      m_FiberBundleTrackVisReader = new FiberBundleTrackVisReader();
      m_FiberBundleTckReader = new FiberBundleTckReader();
      m_FiberBundleDicomReader = new FiberBundleDicomReader();

      m_FiberBundleVtkWriter = new FiberBundleVtkWriter();
      m_FiberBundleTckWriter = new FiberBundleTckWriter();
      m_FiberBundleTrackVisWriter = new FiberBundleTrackVisWriter();
      m_FiberBundleDicomWriter = new FiberBundleDicomWriter();
    }

    void Unload(us::ModuleContext*) override
    {
      for (unsigned int loop(0); loop < m_MimeTypes.size(); ++loop)
      {
        delete m_MimeTypes.at(loop);
      }

      delete m_FiberBundleVtkReader;
      delete m_FiberBundleTckReader;
      delete m_FiberBundleTrackVisReader;
      delete m_FiberBundleDicomReader;

      delete m_FiberBundleDicomWriter;
      delete m_FiberBundleVtkWriter;
      delete m_FiberBundleTckWriter;
      delete m_FiberBundleTrackVisWriter;
    }

  private:

    FiberBundleVtkReader * m_FiberBundleVtkReader;
    FiberBundleTckReader * m_FiberBundleTckReader;
    FiberBundleTrackVisReader * m_FiberBundleTrackVisReader;
    FiberBundleDicomReader * m_FiberBundleDicomReader;

    FiberBundleDicomWriter * m_FiberBundleDicomWriter;
    FiberBundleVtkWriter * m_FiberBundleVtkWriter;
    FiberBundleTckWriter *m_FiberBundleTckWriter;
    FiberBundleTrackVisWriter * m_FiberBundleTrackVisWriter;

    std::vector<mitk::CustomMimeType*> m_MimeTypes;

  };
}

US_EXPORT_MODULE_ACTIVATOR(mitk::DiffusionModuleActivator)
