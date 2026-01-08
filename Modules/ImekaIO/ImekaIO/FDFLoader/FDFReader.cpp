
#include "FDFReader.hpp"

#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/tokenizer.hpp>

#include <mitkFileReaderRegistry.h>
#include <mitkImage.h>
#include <mitkIOConstants.h>

#include "../MimeType.hpp"

namespace Imeka
{

namespace IO
{

const unsigned int FDFReader::SPACING_TO_MM_RATIO = 10;
const unsigned int FDFReader::TOTAL_ELEM_IN_3x3_MATRIX = 9;
const unsigned int FDFReader::ELEM_3x3_MATRIX = 3;

FDFReader::FDFReader()
  : mitk::AbstractFileReader(
  GetFDFImageMimeType(), GetFDFImageDescription())
{
  m_ServiceReg = this->RegisterService();
}

FDFReader::FDFReader(const FDFReader &other)
  : mitk::AbstractFileReader(other)
{}

FDFReader* FDFReader::Clone() const
{
  return new FDFReader(*this);
}

std::vector<itk::SmartPointer<mitk::BaseData>> FDFReader::DoRead()
{
  std::vector<itk::SmartPointer<mitk::BaseData>> result;
  const std::string& locale = "C";
  const std::string& currLocale = setlocale( LC_ALL, nullptr );
  setlocale(LC_ALL, locale.c_str());

  std::ifstream inStream(GetInputLocation().c_str());
  FDFHeader header = ReadHeader(inStream);
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
  mitk::BaseGeometry* imageGeom = image->GetGeometry();
  if (!imageGeom)
  {
    mitkThrowException(InvalidGeometry);
  }

  if (header.orientation.size() == TOTAL_ELEM_IN_3x3_MATRIX)
  {
    vtkSmartPointer<vtkMatrix4x4> orientation = vtkMatrix4x4::New();
    for (unsigned y = 0; y < ELEM_3x3_MATRIX; ++y)
    {
      for (unsigned x = 0; x < ELEM_3x3_MATRIX; ++x)
      {
        orientation->SetElement(x, y,
          header.orientation[x + ELEM_3x3_MATRIX * y]);
      }
    }
    imageGeom->SetIndexToWorldTransformByVtkMatrix(orientation);
  }

  mitk::Vector3D spacing;
  float spacingZ = 1.0;
  if (header.matrix.size() > 2)
  {
    spacingZ = (header.roi[2] / header.matrix[2]) * SPACING_TO_MM_RATIO;
  }
  else if (header.roi.size() > 2)
  {
    spacingZ = header.roi[2] * SPACING_TO_MM_RATIO;
  }
  mitk::FillVector3D(
    spacing,
    (header.roi[0] / header.matrix[0]) * SPACING_TO_MM_RATIO,
    (header.roi[1] / header.matrix[1]) * SPACING_TO_MM_RATIO,
    spacingZ);
  imageGeom->SetSpacing(spacing);

  mitk::Point3D origin;
  mitk::FillVector3D(
    origin,
    header.origin[0],
    header.origin[1],
    (header.origin.size()) > 2 ? header.origin[2] : 0.0);
  imageGeom->SetOrigin(origin);

  result.push_back(image);

  setlocale(LC_ALL, currLocale.c_str());
  MITK_INFO << "FDF Image read";
  delete reg;

  return result;
}

FDFReader::FDFHeader FDFReader::ReadHeader(std::ifstream& inf)
{
  typedef std::istreambuf_iterator<char> char_iterator;
  typedef boost::tokenizer<boost::char_separator<char>,
    char_iterator> tokenizer;

  boost::char_separator<char> line_sep("\n");

  tokenizer line(char_iterator(inf), char_iterator(), line_sep);
  FDFReader::FDFHeader header;

  for (const auto& t : line)
  {
    std::vector<std::string> res;
    boost::algorithm::split(res, t, boost::algorithm::is_any_of(",{}; "), boost::token_compress_on);

    if (res.size() >= 4)
    {
      const char ob = 3, oe = -1;
      const auto attribName = res[1];
      try
      {
        if(attribName == "rank")
        {
          header.rank = boost::lexical_cast<float>(res[ob]);
        }
        else if(attribName == "*spatial_rank")
        {
          header.spatial_rank = res[ob];
        }
        else if(attribName == "*storage")
        {
          header.storage = res[ob];
        }
        else if(attribName == "bits")
        {
          header.bits = boost::lexical_cast<float>(res[ob]);
        }
        else if(attribName == "*type")
        {
          header.type = res[ob];
        }
        else if(attribName == "matrix[]")
        {
          LexicalPush(res.begin()+ob, res.end()+oe, header.matrix);
        }
        else if(attribName == "*abscissa")
        {
          LexicalPush(res.begin() + ob, res.end() + oe, header.abscissa);
        }
        else if(attribName == "*ordinate")
        {
          LexicalPush(res.begin() + ob, res.end() + oe, header.ordinate);
        }
        else if(attribName == "span[]")
        {
          LexicalPush(res.begin() + ob, res.end() + oe, header.span);
        }
        else if(attribName == "origin[]")
        {
          LexicalPush(res.begin() + ob, res.end() + oe, header.origin);
        }
        else if(attribName == "*nucleus[]")
        {
          LexicalPush(res.begin() + ob, res.end() + oe, header.nucleus);
        }
        else if(attribName == "nucfreq[]")
        {
          LexicalPush(res.begin() + ob, res.end() + oe, header.nucfreq);
        }
        else if(attribName == "location[]")
        {
          LexicalPush(res.begin() + ob, res.end() + oe, header.location);
        }
        else if(attribName == "roi[]")
        {
          LexicalPush(res.begin() + ob, res.end() + oe, header.roi);
        }
        else if(attribName == "gap")
        {
          header.gap = boost::lexical_cast<float>(res[ob]);
        }
        else if(attribName == "*file")
        {
          header.file = res[ob];
        }
        else if(attribName == "slice_no")
        {
          header.slice_no = boost::lexical_cast<int>(res[ob]);
        }
        else if(attribName == "slices")
        {
          header.slices = boost::lexical_cast<int>(res[ob]);
        }
        else if(attribName == "echo_no")
        {
          header.echo_no = boost::lexical_cast<int>(res[ob]);
        }
        else if(attribName == "echoes")
        {
          header.echoes = boost::lexical_cast<int>(res[ob]);
        }
        else if(attribName == "TE")
        {
          header.TE = boost::lexical_cast<float>(res[ob]);
        }
        else if(attribName == "te")
        {
          header.te = boost::lexical_cast<float>(res[ob]);
        }
        else if(attribName == "TR")
        {
          header.TR = boost::lexical_cast<float>(res[ob]);
        }
        else if(attribName == "tr")
        {
          header.tr = boost::lexical_cast<float>(res[ob]);
        }
        else if(attribName == "ro_size")
        {
          header.ro_size = boost::lexical_cast<int>(res[ob]);
        }
        else if(attribName == "pe_size")
        {
          header.pe_size = boost::lexical_cast<int>(res[ob]);
        }
        else if(attribName == "*sequence")
        {
          header.sequence = res[ob];
        }
        else if(attribName == "*studyid")
        {
          header.studyid = res[ob];
        }
        else if(attribName == "*position1")
        {
          header.position1 = res[ob];
        }
        else if(attribName == "*position2")
        {
          header.position2 = res[ob];
        }
        else if(attribName == "TI")
        {
          header.TI = boost::lexical_cast<float>(res[ob]);
        }
        else if(attribName == "ti")
        {
          header.ti = boost::lexical_cast<float>(res[ob]);
        }
        else if(attribName == "array_index")
        {
          header.array_index = boost::lexical_cast<int>(res[ob]);
        }
        else if(attribName == "array_dim")
        {
          header.array_dim = boost::lexical_cast<float>(res[ob]);
        }
        else if(attribName == "image")
        {
          header.image = boost::lexical_cast<float>(res[ob]);
        }
        else if(attribName == "display_order")
        {
          header.display_order = boost::lexical_cast<int>(res[ob]);
        }
        else if(attribName == "bigendian")
        {
          header.bigendian = boost::lexical_cast<int>(res[ob]);
        }
        else if(attribName == "imagescale")
        {
          header.imagescale = boost::lexical_cast<float>(res[ob]);
        }
        else if(attribName == "psi")
        {
          header.psi = boost::lexical_cast<float>(res[ob]);
        }
        else if(attribName == "phi")
        {
          header.phi = boost::lexical_cast<float>(res[ob]);
        }
        else if(attribName == "theta")
        {
          header.theta = boost::lexical_cast<float>(res[ob]);
        }
        else if(attribName == "dro")
        {
          header.dro = boost::lexical_cast<float>(res[ob]);
        }
        else if(attribName == "dpe")
        {
          header.dpe = boost::lexical_cast<float>(res[ob]);
        }
        else if(attribName == "dsl")
        {
          header.dsl = boost::lexical_cast<float>(res[ob]);
        }
        else if(attribName == "bvalue")
        {
          header.bvalue = boost::lexical_cast<float>(res[ob]);
        }
        else if(attribName == "orientation[]")
        {
          LexicalPush(res.begin() + ob, res.end() + oe, header.orientation);
        }
        else if(attribName == "checksum")
        {
          header.checksum = boost::lexical_cast<int>(res[ob]);
        }
      }
      catch (const boost::bad_lexical_cast &)
      {
        MITK_ERROR << "Error while reading header. "
          "A lexical cast failed to read a number.";
      }
    }
  }

  return header;
}

void FDFReader::SetHeaderOptions(
  const FDFHeader &header,
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
  options[mitk::IOConstants::SIZE_X()] = int(header.matrix[0]);
  options[mitk::IOConstants::SIZE_Y()] = int(header.matrix[1]);
  if (header.rank > 2)
  {
    options[mitk::IOConstants::SIZE_Z()] = int(header.matrix[2]);
  }
  else
  {
    options[mitk::IOConstants::SIZE_Z()] = 1;
  }

  if (header.rank > 3)
  {
    options[mitk::IOConstants::SIZE_T()] = header.matrix[3];
  }

  auto endianity = mitk::IOConstants::ENDIANNESS_BIG();
  if (header.bigendian == 0)
  {
    endianity = mitk::IOConstants::ENDIANNESS_LITTLE();
  }

  options[mitk::IOConstants::ENDIANNESS()] = endianity;
}

} // namespace IO

} // namespace Imeka
