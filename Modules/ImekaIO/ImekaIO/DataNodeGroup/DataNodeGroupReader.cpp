
#include "DataNodeGroupReader.hpp"

#include "../MimeType.hpp"
#include "ImekaWidgets/MITK/DataNodeGroup.hpp"

namespace Imeka
{

namespace IO
{

DataNodeGroupReader::DataNodeGroupReader()
  : mitk::AbstractFileReader(
      GetDataNodeGroupMimeType(), GetDataNodeGroupDescription())
{
  m_ServiceReg = this->RegisterService();
}

DataNodeGroupReader::DataNodeGroupReader(const DataNodeGroupReader &other)
  : mitk::AbstractFileReader(other)
{}

DataNodeGroupReader* DataNodeGroupReader::Clone() const
{
  return new DataNodeGroupReader(*this);
}

std::vector<itk::SmartPointer<mitk::BaseData>> DataNodeGroupReader::Read()
{
  std::vector<itk::SmartPointer<mitk::BaseData>> result;
  result.push_back(Imeka::Common::DataNodeGroup::New().GetPointer());
  return result;
}

} // namespace IO

} // namespace Imeka
