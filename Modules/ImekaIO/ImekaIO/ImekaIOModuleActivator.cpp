
#include <usModuleActivator.h>
#include <usModuleContext.h>

#include "MimeType.hpp"
#include "BoundingObject/BoundingObjectReader.hpp"
#include "BoundingObject/BoundingObjectWriter.hpp"
#include "DataNodeGroup/DataNodeGroupReader.hpp"
#include "DataNodeGroup/DataNodeGroupWriter.hpp"
#include "Dicom/DicomWriter.hpp"
#include "FDFLoader/FDFReader.hpp"
#include "VFFLoader/VFFReader.hpp"
#include "RGBNiftiReader/RGBNiftiFileReader.hpp"

namespace Imeka
{

namespace IO
{

class IOModuleActivator : public us::ModuleActivator
{
public:

  void Load(us::ModuleContext* context)
  {
    us::ServiceProperties props;
    props[us::ServiceConstants::SERVICE_RANKING()] = 1;

    context->RegisterService(GetBoundingObjectMimeType().Clone(), props);
    context->RegisterService(GetDataNodeGroupMimeType().Clone(), props);
    context->RegisterService(GetFDFImageMimeType().Clone(), props);
    context->RegisterService(GetVFFImageMimeType().Clone(), props);
    context->RegisterService(GetRGBNiftiImageMimeType().Clone(), props);

    m_BoundingObjectReader = new BoundingObjectReader();
    m_BoundingObjectWriter = new BoundingObjectWriter();
    m_DataNodeGroupReader = new DataNodeGroupReader();
    m_DataNodeGroupWriter = new DataNodeGroupWriter();
    m_DicomWriter = new DicomWriter();
    m_FDFReader = new FDFReader();
    m_VFFReader = new VFFReader();
    m_RGBReader = new RGBNiftiFileReader();
  }

  void Unload(us::ModuleContext*)
  {
    delete m_BoundingObjectReader;
    delete m_BoundingObjectWriter;
    delete m_DataNodeGroupReader;
    delete m_DataNodeGroupWriter;
    delete m_DicomWriter;
    delete m_FDFReader;
    delete m_VFFReader;
    delete m_RGBReader;
  }

private:
  BoundingObjectReader* m_BoundingObjectReader;
  BoundingObjectWriter* m_BoundingObjectWriter;
  DataNodeGroupReader* m_DataNodeGroupReader;
  DataNodeGroupWriter* m_DataNodeGroupWriter;
  DicomWriter* m_DicomWriter;
  FDFReader* m_FDFReader;
  VFFReader* m_VFFReader;
  RGBNiftiFileReader* m_RGBReader;
};

} // namespace IO

} // namespace Imeka

US_EXPORT_MODULE_ACTIVATOR(Imeka::IO::IOModuleActivator)
