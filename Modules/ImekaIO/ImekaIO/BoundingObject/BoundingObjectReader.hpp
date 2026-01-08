
#ifndef IMEKA_BOUNDING_OBJECT_READER_HPP_INCLUDED
#define IMEKA_BOUNDING_OBJECT_READER_HPP_INCLUDED

#include <mitkAbstractFileReader.h>

#include "ImekaIOExports.h"

namespace tinyxml2 { class XMLElement; }
namespace mitk { class BoundingObject; }

namespace Imeka
{

namespace IO
{

class ImekaIO_EXPORT BoundingObjectReader : public mitk::AbstractFileReader
{
public:
  BoundingObjectReader();
  BoundingObjectReader(const BoundingObjectReader& other);
  virtual ~BoundingObjectReader() {}

  virtual BoundingObjectReader* Clone() const;
  using mitk::AbstractFileReader::Read;

protected:
  virtual std::vector<itk::SmartPointer<mitk::BaseData>> DoRead();

private:
  us::ServiceRegistration<mitk::IFileReader> m_ServiceReg;

  void ReadOrigin(mitk::BoundingObject*, tinyxml2::XMLElement*);
  void ReadWorldTransform(mitk::BoundingObject*, tinyxml2::XMLElement*);
};

} // namespace IO

} // namespace Imeka

#endif // IMEKA_BOUNDING_OBJECT_READER_HPP_INCLUDED

