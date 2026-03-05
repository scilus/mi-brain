
#include "VFFReader.hpp"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>

#include <mitkFileReaderRegistry.h>
#include <mitkImage.h>
#include <mitkIOConstants.h>

#include "../MimeType.hpp"

namespace Imeka
{

namespace IO
{

VFFReader::VFFReader()
  : mitk::AbstractFileReader(
      GetVFFImageMimeType(), GetVFFImageDescription())
{
  m_ServiceReg = this->RegisterService();
}

VFFReader::VFFReader(const VFFReader &other)
  : mitk::AbstractFileReader(other)
{}

VFFReader* VFFReader::Clone() const
{
  return new VFFReader(*this);
}

std::vector<itk::SmartPointer<mitk::BaseData>>
VFFReader::DoRead()
{
  std::vector<itk::SmartPointer<mitk::BaseData>> result;

  const std::string& locale = "C";
  const std::string& currLocale = setlocale( LC_ALL, nullptr );
  setlocale(LC_ALL, locale.c_str());

  std::ifstream inStream(GetInputLocation().c_str());
  VFFHeader header;
  try
  {
    header = ReadHeader(inStream);
  }
  catch (const boost::bad_lexical_cast &)
  {
    MITK_ERROR << "Error while reading header. "
      "A lexical cast failed to read a number.";
    return result;
  }

  mitk::IFileReader::Options options;
  SetHeaderOptions(header, options);

  // Obtain a RawfileReader
  const auto reg = new mitk::FileReaderRegistry;
  const auto mimetype = mitk::FileReaderRegistry::GetMimeTypeForFile("*.raw");
  const auto refs = mitk::FileReaderRegistry::GetReferences(mimetype);
  mitk::IFileReader* reader = reg->GetReader(refs.at(0));
  reader->SetInput(GetInputLocation().c_str());
  reader->SetOptions(options);

  const auto objectsRead = reader->Read();
  mitk::Image* image = dynamic_cast<mitk::Image*>(
    objectsRead.front().GetPointer());

  const float flipX = (header.rank > 2) ? -1.0 : 1.0; //Only if dimension > 2
  const float flipY = -1.0; // Always

  mitk::AffineTransform3D::MatrixType matrix;
  matrix.SetIdentity();
  matrix(0, 0) = flipX * header.spacing[0] * header.elementSize;
  matrix(1, 1) = flipY * header.spacing[1] * header.elementSize;
  if (header.rank > 2)
  {
    matrix(2, 2) = header.spacing[2] * header.elementSize;
  }
  else
  {
    matrix(2, 2) = 1.0;
  }

  auto transform = mitk::AffineTransform3D::New();
  transform->SetMatrix(matrix);
  image->GetGeometry()->SetIndexToWorldTransform(transform);
  image->GetGeometry()->SetSpacing(
    header.spacing * header.elementSize);

  result.push_back(image);

  setlocale(LC_ALL, currLocale.c_str());
  MITK_INFO << "FDF Image read";

  return result;
}

VFFReader::VFFHeader VFFReader::ReadHeader(std::ifstream& inf) const
{
  VFFHeader header;
  std::string line;

  bool elementSizeRead = false;

  std::getline(inf, line);
  while (!inf.eof() && line != "\f")
  {
    std::string key, value;
    ParseLine(line, key, value);

    if (key == "rank")
    {
      header.rank = boost::lexical_cast<unsigned int>(value);
    }
    else if (key == "type")
    {
      header.type = value;
    }
    else if (key == "size")
    {
      std::vector<std::string> size;
      boost::split(size, value, boost::is_any_of(" "));
      for (unsigned int i = 0; i < size.size(); ++i)
      {
        header.size.push_back(boost::lexical_cast<float>(size[i]));
      }
    }
    else if (key == "origin")
    {
      std::vector<std::string> origin;
      boost::split(origin, value, boost::is_any_of(" "));
      for (unsigned int i = 0; i < origin.size(); ++i)
      {
        header.origin[i] = boost::lexical_cast<float>(origin[i]);
      }
    }
    else if (key == "bands")
    {
      if (boost::lexical_cast<unsigned int>(value) != 1)
      {
        MITK_WARN << "bands in header is not equal to 1. Loading might fails.";
      }
    }
    else if (key == "bits")
    {
      header.bits = boost::lexical_cast<unsigned int>(value);
    }
    else if (key == "format")
    {
      if (value != "slice" && value != "base")
      {
        MITK_WARN << "Key 'format' in header is not equal to 'slice' or 'base'"
          " Loading might fails.";
      }
    }
    else if (key == "reverse_order")
    {
      // This key my be linked to the indianity.
      if (value != "no")
      {
        MITK_WARN << "Key 'reverse_order' in header is not equal to 'no'."
          " Loading might fails.";
      }
    }
    else if (key == "spacing")
    {
      std::vector<std::string> spacing;
      boost::split(spacing, value, boost::is_any_of(" "));
      for (unsigned int i = 0; i < spacing.size(); ++i)
      {
        header.spacing[i] = boost::lexical_cast<float>(spacing[i]);
      }
    }
    else if (key == "elementsize")
    {
      header.elementSize = boost::lexical_cast<float>(value);
      elementSizeRead = true;
    }

    std::getline(inf, line);
  }

  if (header.rank < 3)
  {
    header.spacing[2] = 1.0;
  }

  if (!elementSizeRead)
  {
    header.elementSize = 1.0;
  }

  return header;
}

void VFFReader::ParseLine(
  const std::string &line,
  std::string &left,
  std::string &right) const
{
  const char SEP = '=';
  const unsigned int index = line.find(SEP);
  left = line.substr(0, index);
  right = line.substr(index + 1, line.length() - index - 2);
}

void VFFReader::SetHeaderOptions(
  const VFFHeader &header,
  mitk::IFileReader::Options& options)
{
  switch (static_cast<int>(header.bits))
  {
  case 8:
    options[mitk::IOConstants::PIXEL_TYPE()] =
      mitk::IOConstants::PIXEL_TYPE_CHAR();
    break;
  case 16:
    options[mitk::IOConstants::PIXEL_TYPE()] =
      mitk::IOConstants::PIXEL_TYPE_SHORT();
    break;
  case 32:
    // Unsure! May be SINT or UINT...
    options[mitk::IOConstants::PIXEL_TYPE()] =
      mitk::IOConstants::PIXEL_TYPE_FLOAT();
    break;
  case 64:
    options[mitk::IOConstants::PIXEL_TYPE()] =
      mitk::IOConstants::PIXEL_TYPE_DOUBLE();
    break;
  }

  options[mitk::IOConstants::DIMENSION()] = header.rank;
  options[mitk::IOConstants::SIZE_X()] = static_cast<int>(header.size[0]);
  options[mitk::IOConstants::SIZE_Y()] = static_cast<int>(header.size[1]);
  options[mitk::IOConstants::SIZE_Z()] = static_cast<int>(header.size[2]);
  if (header.rank > 3)
  {
    options[mitk::IOConstants::SIZE_T()] = header.size[3];
  }

  options[mitk::IOConstants::ENDIANNESS()] =
    mitk::IOConstants::ENDIANNESS_BIG();
}

} // namespace IO

} // namespace Imeka
