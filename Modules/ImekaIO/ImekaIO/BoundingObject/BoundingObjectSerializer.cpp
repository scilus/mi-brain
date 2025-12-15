
#include "BoundingObjectSerializer.hpp"

#include <mitkBoundingObject.h>
#include <mitkIOUtil.h>

#include <itksys/SystemTools.hxx>

// Needs to be in GLOBAL namespace
MITK_REGISTER_SERIALIZER(CuboidSerializer);
MITK_REGISTER_SERIALIZER(EllipsoidSerializer);
MITK_REGISTER_SERIALIZER(ConeSerializer);
MITK_REGISTER_SERIALIZER(CylinderSerializer);

namespace mitk
{

std::string BoundingObjectSerializer::Serialize()
{
  const BoundingObject* boundingObject =
    dynamic_cast<const BoundingObject*>(m_Data.GetPointer());
  if (!boundingObject)
  {
    MITK_ERROR << " Object at " << static_cast<const void*>(this->m_Data)
      << " is not an mitk::BoundingObject. Cannot serialize as BoundingObject.";
  }

  const std::string filename = GetUniqueFilenameInWorkingDirectory()
    + "_" + m_FilenameHint + ".bdo";
  const std::string fullname = m_WorkingDirectory + "/"
    + itksys::SystemTools::ConvertToOutputPath(filename.c_str());

  try
  {
    IOUtil::Save(const_cast<BoundingObject*>(boundingObject), fullname);
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


