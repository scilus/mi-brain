#include "mitkFiberBundleTrxReader.h"

#include <itksys/SystemTools.hxx>

#include <mitkDiffusionIOMimeTypes.h>
#include "FiberBundle/mitkTrackvis.h"

#include <trx/trx.h>

mitk::FiberBundleTrxReader::FiberBundleTrxReader()
  : mitk::AbstractFileReader( mitk::DiffusionIOMimeTypes::FIBERBUNDLE_TRX_MIMETYPE_NAME(), "TRX Fiber Bundle Reader" )
{
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
	  //trx::AnyTrxFile::Load
	  auto trxFile = trx::TrxFile::Load(this->GetInputLocation());


      if (errCode == 0)
      {
        const auto emptyFileWarning =
          "There's 0 point in this file. It won't be added to the DataManager "
          "because there's nothing to be done with this file.";
        throw std::length_error(emptyFileWarning);
      }
      else if (errCode == -1)
      {
        const auto noDSWarning =
          "MI-Brain error unrelated to the current trx. "
          "The TrackVisFiberReader doesn't know the DataStorage.";
        throw std::length_error(noDSWarning);
      }
      else if (errCode == -2)
      {
        const auto corruptedHeaderWarning =
          "The trx file you're trying to load is impossible to read because "
          "the header is corrupted.";
        throw std::length_error(corruptedHeaderWarning);
      }
      else if (errCode == -3)
      {
        const auto noReadPointsWarning =
          "Error reading points. Trx file is corrupted.";
        throw std::length_error(noReadPointsWarning);
      }
      else if (errCode == -4)
      {
        const auto noReadPropertiesWarning =
          "Error reading properties. Trx file is corrupted.";
        throw std::length_error(noReadPropertiesWarning);
      }

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
