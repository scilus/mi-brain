#include "mitkFiberBundleTrxReader.h"

#include <clocale>

#include <itksys/SystemTools.hxx>

#include <mitkDiffusionIOMimeTypes.h>
#include "FiberBundle/FilteredFiberBundle.hpp"
#include "FiberBundle/Trx.h"

namespace
{
  // Restores the previous locale when leaving scope, even on exceptions.
  class LocaleGuard
  {
  public:
    LocaleGuard()
      : m_PreviousLocale(setlocale(LC_ALL, nullptr))
    {
      setlocale(LC_ALL, "C");
    }
    ~LocaleGuard() { setlocale(LC_ALL, m_PreviousLocale.c_str()); }

  private:
    std::string m_PreviousLocale;
  };
}

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
  LocaleGuard localeGuard;

  const std::string filename = this->GetInputLocation();
  MITK_INFO << "Loading tractogram (TRX format): " << itksys::SystemTools::GetFilenameName(filename);

  auto fiber = FilteredFiberBundle::New();
  if (ReadTrx(filename, fiber) == 0)
  {
    throw std::length_error(
      "There's 0 point in this file. It won't be added to the DataManager "
      "because there's nothing to be done with this file.");
  }

  result.push_back(fiber.GetPointer());

  MITK_INFO << "Fiber bundle read: " << fiber->GetNumFibers() << " fibers";
  return result;
}
