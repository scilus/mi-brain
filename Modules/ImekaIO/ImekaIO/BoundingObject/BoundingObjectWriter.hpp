
#ifndef IMEKA_BOUNDING_OBJECT_WRITER_HPP_INCLUDED
#define IMEKA_BOUNDING_OBJECT_WRITER_HPP_INCLUDED

#include <mitkAbstractFileWriter.h>

#include "ImekaIOExports.h"

namespace tinyxml2 { class XMLElement; class XMLDocument; }
namespace mitk { class BaseGeometry; };

namespace Imeka
{

namespace IO
{

class BoundingObjectWriter : public mitk::AbstractFileWriter
{
public:
  BoundingObjectWriter();
  BoundingObjectWriter(const BoundingObjectWriter& other);

  virtual BoundingObjectWriter* Clone() const;
  virtual ~BoundingObjectWriter() {}

  using mitk::AbstractFileWriter::Write;
  virtual void Write();

private:
  void WriteOrigin(tinyxml2::XMLElement* mainXML, const mitk::BaseGeometry*, tinyxml2::XMLDocument* doc);
  void WriteWorldTransform(tinyxml2::XMLElement* mainXML, const mitk::BaseGeometry*, tinyxml2::XMLDocument* doc);
};

} // namespace IO

} // namespace Imeka

#endif // IMEKA_BOUNDING_OBJECT_WRITER_HPP_INCLUDED

