
#ifndef IMEKA_IO_VFF_READER_HPP_INCLUDED
#define IMEKA_IO_VFF_READER_HPP_INCLUDED

#include <mitkAbstractFileReader.h>

#include "ImekaIOExports.h"

namespace mitk { class RawImageFileReader; }

namespace Imeka
{

namespace IO
{

class ImekaIO_EXPORT VFFReader : public mitk::AbstractFileReader
{
  struct VFFHeader
  {
    unsigned int rank;
    std::string type;
    std::vector<float> size;
    mitk::Point3D origin;
    unsigned int bits;
    mitk::Vector3D spacing;
    float elementSize;
  };

public:
  VFFReader();
  virtual ~VFFReader(){}
  VFFReader(const VFFReader& other);
  virtual VFFReader* Clone() const;

  using mitk::AbstractFileReader::Read;
  virtual std::vector<itk::SmartPointer<mitk::BaseData>> Read();

private:
  us::ServiceRegistration<mitk::IFileReader> m_ServiceReg;

  VFFHeader ReadHeader(std::ifstream& inf) const;
  void ParseLine(const std::string &line, std::string &left, std::string &right) const;
  void SetHeaderOptions(
    const VFFHeader &header,
    mitk::IFileReader::Options& options);
};

} // namespace IO

} // namespace Imeka

#endif // IMEKA_IO_VFF_READER_HPP_INCLUDED

