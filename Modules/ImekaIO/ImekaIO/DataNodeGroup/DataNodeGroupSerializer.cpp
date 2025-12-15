
#include "DataNodeGroupSerializer.hpp"

#include <mitkIOUtil.h>
#include <itksys/SystemTools.hxx>

#include "ImekaWidgets/MITK/DataNodeGroup.hpp"

MITK_REGISTER_SERIALIZER(DataNodeGroupSerializer);

namespace mitk
{

std::string DataNodeGroupSerializer::Serialize()
{
  typedef Imeka::Common::DataNodeGroup DNG;

  auto dataNodeGroup = dynamic_cast<const DNG*>(m_Data.GetPointer());
  if (!dataNodeGroup)
  {
    MITK_ERROR << " Object at " << static_cast<const void*>(this->m_Data)
      << " is not an DataNodeGroup. Cannot serialize as DataNodeGroup.";
  }

  const std::string filename = GetUniqueFilenameInWorkingDirectory()
    + "_" + m_FilenameHint + ".dng";
  const std::string fullname = m_WorkingDirectory + "/"
    + itksys::SystemTools::ConvertToOutputPath(filename.c_str());

  try
  {
    mitk::IOUtil::Save(const_cast<DNG*>(dataNodeGroup), fullname);
  }
  catch (std::exception& e)
  {
    MITK_ERROR << " Error serializing object at "
      << static_cast<const void*>(this->m_Data)
      << " to " << fullname << ": " << e.what();
    return "";
  }
  return filename;
}

} // namespace mitk

