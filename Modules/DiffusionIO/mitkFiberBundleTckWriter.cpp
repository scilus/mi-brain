#include "mitkFiberBundleTckWriter.hpp"

#include <itksys/SystemTools.hxx>

#include <vtkCellArray.h>

#include <mitkDiffusionIOMimeTypes.h>
#include "FiberBundle/FilteredFiberBundle.hpp"

#include "FiberBundle/Tck.hpp"

namespace mitk
{

FiberBundleTckWriter::FiberBundleTckWriter()
  : AbstractFileWriter(FilteredFiberBundle::GetStaticNameOfClass(), DiffusionIOMimeTypes::FIBERBUNDLE_TCK_MIMETYPE_NAME(), "Tck Fiber Bundle Reader")
{
  RegisterService();
}

FiberBundleTckWriter::FiberBundleTckWriter(const FiberBundleTckWriter & other)
  : AbstractFileWriter(other)
{}

FiberBundleTckWriter* FiberBundleTckWriter::Clone() const
{
  return new FiberBundleTckWriter(*this);
}

void FiberBundleTckWriter::Write()
{
  std::ostream* out;
  std::ofstream outStream;

  if (this->GetOutputStream())
  {
    out = this->GetOutputStream();
  }
  else
  {
    outStream.open(this->GetOutputLocation().c_str());
    out = &outStream;
  }

  if (!out->good())
  {
    mitkThrow() << "Stream not good.";
  }

  const std::string& locale = "C";
  const std::string& currLocale = setlocale(LC_ALL, NULL);
  setlocale(LC_ALL, locale.c_str());

  std::locale previousLocale(out->getloc());
  std::locale I("C");
  out->imbue(I);

  mitk::FiberBundle::ConstPointer input =
    dynamic_cast<const mitk::FiberBundle*>(this->GetInput());
  const std::string ext = itksys::SystemTools::GetFilenameLastExtension(
    this->GetOutputLocation().c_str());
  if (ext == "")
  {
    this->SetOutputLocation(this->GetOutputLocation() + ".tck");
  }

  MITK_INFO << "Writing fiber bundle as Tck";
  WriteTck(this->GetOutputLocation(), input->GetFiberPolyData());

  setlocale(LC_ALL, currLocale.c_str());
  MITK_INFO << "Fiber bundle written";
}

} // namespace mitk
