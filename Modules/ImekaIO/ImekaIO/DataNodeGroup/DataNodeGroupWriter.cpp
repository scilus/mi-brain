
#include "DataNodeGroupWriter.hpp"

#include "../MimeType.hpp"
#include "ImekaWidgets/MITK/DataNodeGroup.hpp"

#include <fstream>

namespace Imeka
{

namespace IO
{

DataNodeGroupWriter::DataNodeGroupWriter()
  : mitk::AbstractFileWriter(
      Imeka::Common::DataNodeGroup::GetStaticNameOfClass(),
      GetDataNodeGroupMimeType(),
      GetDataNodeGroupDescription())
{
  RegisterService();
}

DataNodeGroupWriter::DataNodeGroupWriter(const DataNodeGroupWriter& other)
  : mitk::AbstractFileWriter(other)
{}

DataNodeGroupWriter::~DataNodeGroupWriter()
{}

DataNodeGroupWriter* DataNodeGroupWriter::Clone() const
{
  return new DataNodeGroupWriter(*this);
}

void DataNodeGroupWriter::Write()
{
  std::ostream* out;
  std::ofstream outStream;
  if (this->GetOutputStream())
  {
    out = this->GetOutputStream();
  }
  else
  {
    outStream.open(this->GetOutputLocation().c_str());
    out = &outStream;
  }
  if (!out->good()) { mitkThrow() << "Stream not good."; }

  const std::string& currLocale = setlocale(LC_ALL, nullptr);
  setlocale(LC_ALL, "C");

  std::locale previousLocale(out->getloc());
  std::locale I("C");
  out->imbue(I);

  const std::string line =
    "At least some character because some reader will fail otherwise";
  outStream << line;

  out->imbue(previousLocale);
  setlocale(LC_ALL, currLocale.c_str());
  MITK_INFO << "Fiber bundle written";
}

} // namespace IO

} // namespace Imeka

