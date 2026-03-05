
#ifndef IMEKA_BOUNDING_OBJECT_WRITER_HPP_INCLUDED
#define IMEKA_BOUNDING_OBJECT_WRITER_HPP_INCLUDED

#include <mitkAbstractFileWriter.h>
#include <tinyxml2.h>

#include "ImekaIOExports.h"

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

  virtual BoundingObjectWriter* Clone() const override;
  virtual ~BoundingObjectWriter() {}

  virtual void Write() override;

private:
  void WriteOrigin(tinyxml2::XMLElement* mainXML, const mitk::BaseGeometry*);
  void WriteWorldTransform(tinyxml2::XMLElement* mainXML, const mitk::BaseGeometry*);
};

} // namespace IO

} // namespace Imeka

#endif // IMEKA_BOUNDING_OBJECT_WRITER_HPP_INCLUDED

