
#ifndef IMEKA_BOUNDING_OBJECT_WRITER_HPP_INCLUDED
#define IMEKA_BOUNDING_OBJECT_WRITER_HPP_INCLUDED

#include <mitkAbstractFileWriter.h>

#include "ImekaIOExports.h"

class TiXmlElement;
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
  void WriteOrigin(TiXmlElement* mainXML, const mitk::BaseGeometry*);
  void WriteWorldTransform(TiXmlElement* mainXML, const mitk::BaseGeometry*);
};

} // namespace IO

} // namespace Imeka

#endif // IMEKA_BOUNDING_OBJECT_WRITER_HPP_INCLUDED

