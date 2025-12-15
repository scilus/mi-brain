
#ifndef IMEKA_IO_DATANODE_GROUP_WRITER_HPP_INCLUDED
#define IMEKA_IO_DATANODE_GROUP_WRITER_HPP_INCLUDED

#include <mitkAbstractFileWriter.h>

#include "ImekaIOExports.h"

namespace Imeka
{

namespace IO
{

class ImekaIO_EXPORT DataNodeGroupWriter : public mitk::AbstractFileWriter
{
public:
  DataNodeGroupWriter();
  DataNodeGroupWriter(const DataNodeGroupWriter& other);
  virtual DataNodeGroupWriter* Clone() const;
  virtual ~DataNodeGroupWriter();

  using mitk::AbstractFileWriter::Write;
  virtual void Write();
};

} // namespace IO

} // namespace Imeka

#endif // IMEKA_IO_DATANODE_GROUP_WRITER_HPP_INCLUDED

