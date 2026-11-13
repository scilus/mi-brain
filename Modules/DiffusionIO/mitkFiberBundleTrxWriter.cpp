#include "mitkFiberBundleTrxWriter.h"

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

namespace mitk
{

FiberBundleTrxWriter::FiberBundleTrxWriter()
  : AbstractFileWriter(FilteredFiberBundle::GetStaticNameOfClass(), DiffusionIOMimeTypes::FIBERBUNDLE_TRX_MIMETYPE_NAME(), "TRX Fiber Bundle Writer")
{
  RegisterService();
}

FiberBundleTrxWriter::FiberBundleTrxWriter(const FiberBundleTrxWriter & other)
  : AbstractFileWriter(other)
{}

FiberBundleTrxWriter* FiberBundleTrxWriter::Clone() const
{
  return new FiberBundleTrxWriter(*this);
}

void FiberBundleTrxWriter::Write()
{
  auto input = dynamic_cast<const mitk::FiberBundle*>(this->GetInput());
  if (input == nullptr)
  {
    mitkThrow() << "Input is not a fiber bundle.";
  }

  // trx-cpp picks the format from the extension, so it must be set before
  // LocalFile builds its temporary file name from it.
  const std::string ext = itksys::SystemTools::GetFilenameLastExtension(
    this->GetOutputLocation());
  if (ext == "")
  {
    this->SetOutputLocation(this->GetOutputLocation() + ".trx");
  }

  LocaleGuard localeGuard;

  // trx-cpp only writes to files: LocalFile gives the output location, or a
  // temporary file that is copied into the output stream on destruction.
  LocalFile localFile(this);

  MITK_INFO << "Writing fiber bundle as TRX";
  try
  {
    // Only a FilteredFiberBundle has streamline groups (TRX groups that were
    // loaded and/or tract groups added by the save visible fibers actions)
    const auto filtered = dynamic_cast<const mitk::FilteredFiberBundle*>(input);
    if (filtered)
    {
      WriteTrx(localFile.GetFileName(), input->GetFiberPolyData(),
               filtered->GetStreamlineGroups());
    }
    else
    {
      WriteTrx(localFile.GetFileName(), input->GetFiberPolyData());
    }
  }
  catch (const std::exception& e)
  {
    mitkThrow() << "Failed to write TRX file: " << e.what();
  }
  MITK_INFO << "Fiber bundle written";
}

} // namespace mitk
