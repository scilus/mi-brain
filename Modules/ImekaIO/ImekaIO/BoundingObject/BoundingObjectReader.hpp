
#ifndef IMEKA_BOUNDING_OBJECT_READER_HPP_INCLUDED
#define IMEKA_BOUNDING_OBJECT_READER_HPP_INCLUDED

#include <mitkAbstractFileReader.h>

#include "ImekaIOExports.h"

class TiXmlHandle;

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
  virtual std::vector<itk::SmartPointer<mitk::BaseData>> Read();

private:
  us::ServiceRegistration<mitk::IFileReader> m_ServiceReg;

  void ReadOrigin(mitk::BoundingObject*, TiXmlHandle);
  void ReadWorldTransform(mitk::BoundingObject*, TiXmlHandle);
};

} // namespace IO

} // namespace Imeka

#endif // IMEKA_BOUNDING_OBJECT_READER_HPP_INCLUDED

