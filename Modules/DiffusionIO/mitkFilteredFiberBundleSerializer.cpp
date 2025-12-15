
#include "mitkFilteredFiberBundleSerializer.hpp"

#include "FiberBundle/FilteredFiberBundle.hpp"
#include "mitkFiberBundleVtkWriter.h"

#include <itksys/SystemTools.hxx>
#include <mitkIOUtil.h>


MITK_REGISTER_SERIALIZER(FilteredFiberBundleSerializer)

namespace mitk
{

FilteredFiberBundleSerializer::FilteredFiberBundleSerializer()
{}

FilteredFiberBundleSerializer::~FilteredFiberBundleSerializer()
{}

std::string FilteredFiberBundleSerializer::Serialize()
{
  auto fb = dynamic_cast<const FilteredFiberBundle*>(m_Data.GetPointer());
  if (!fb)
  {
    MITK_ERROR << " Object at " << (const void*) this->m_Data
      << " is not an mitk::FiberBundle. Cannot serialize as FiberBundle.";
    return "";
  }

  std::string filename(this->GetUniqueFilenameInWorkingDirectory());
  filename += "_";
  filename += m_FilenameHint;
  filename += ".trk";

  std::string fullname(m_WorkingDirectory);
  fullname += "/";
  fullname += itksys::SystemTools::ConvertToOutputPath(filename.c_str());

  try
  {
    mitk::IOUtil::Save(const_cast<FilteredFiberBundle*>(fb), fullname);
  }
  catch (std::exception& e)
  {
    MITK_ERROR << " Error serializing object at " << (const void*) this->m_Data
      << " to "
      << fullname
      << ": "
      << e.what();
    return "";
  }
  return filename;
}

} // namespace mitk
