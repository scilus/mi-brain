#include "mitkFiberBundleTrxReader.h"

#include <itksys/SystemTools.hxx>

#include <mitkDiffusionIOMimeTypes.h>

#include <trx/trx.h>

mitk::FiberBundleTrxReader::FiberBundleTrxReader()
  : mitk::AbstractFileReader( mitk::DiffusionIOMimeTypes::FIBERBUNDLE_TRX_MIMETYPE_NAME(), "TRX Fiber Bundle Reader" )
{
  MITK_INFO << "TRX READER REGISTERED";
  m_ServiceReg = this->RegisterService();
}

mitk::FiberBundleTrxReader::FiberBundleTrxReader(const FiberBundleTrxReader &other)
  :mitk::AbstractFileReader(other)
{
}

mitk::FiberBundleTrxReader * mitk::FiberBundleTrxReader::Clone() const
{
  return new FiberBundleTrxReader(*this);
}

std::vector<itk::SmartPointer<mitk::BaseData> > mitk::FiberBundleTrxReader::DoRead()
{

  std::vector<itk::SmartPointer<mitk::BaseData> > result;
  try
  {
    const std::string& locale = "C";
    const std::string& currLocale = setlocale( LC_ALL, nullptr );
    setlocale(LC_ALL, locale.c_str());

    std::string filename = this->GetInputLocation();
    MITK_INFO << "Loading tractogram (TrackVis format): " << itksys::SystemTools::GetFilenameName(filename);

    std::string ext = itksys::SystemTools::GetFilenameLastExtension(filename);
    ext = itksys::SystemTools::LowerCase(ext);

    if (ext==".trx")
    {

	  // TO DO: Read the TRX file
	  /*
      auto fiber = FilteredFiberBundle::New();
      TrackVisFiberReader reader;
      reader.Open(this->GetInputLocation().c_str());
      const auto errCode = reader.Read(fiber.GetPointer());
	  */

    auto fiber = FilteredFiberBundle::New();
	  auto trxFile = trx::TrxFile::Load(this->GetInputLocation());

      /*if (auto refGeometry = fiber->GetReferenceGeometry())
      {
        fiber->SetGeometry(refGeometry->Clone());
      }*/

      result.push_back(fiber.GetPointer());
    }

    setlocale(LC_ALL, currLocale.c_str());
    MITK_INFO << "Fiber bundle read";
    return result;
  }
  catch(...)
  {
    throw;
  }
  return result;
}
