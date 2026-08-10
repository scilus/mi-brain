
#include "BoundingObjectReader.hpp"

#include <mitkBoundingObject.h>
#include <itksys/SystemTools.hxx>
#include <tinyxml2.h>

#include "../MimeType.hpp"
#include "ImekaBoundingObject/BoundingObjectFactory.hpp"

namespace Imeka
{

namespace IO
{

BoundingObjectReader::BoundingObjectReader()
  : mitk::AbstractFileReader(
      GetBoundingObjectMimeType(), GetBoundingObjectDescription())
{
  m_ServiceReg = this->RegisterService();
}

BoundingObjectReader::BoundingObjectReader(const BoundingObjectReader &other)
  : mitk::AbstractFileReader(other)
{}

BoundingObjectReader* BoundingObjectReader::Clone() const
{
  return new BoundingObjectReader(*this);
}

std::vector<itk::SmartPointer<mitk::BaseData>>
BoundingObjectReader::DoRead()
{
  std::vector<itk::SmartPointer<mitk::BaseData>> result;

  const std::string& locale = "C";
  const std::string& currLocale = setlocale( LC_ALL, nullptr );
  setlocale(LC_ALL, locale.c_str());

  tinyxml2::XMLDocument doc;
  if (doc.LoadFile(GetInputLocation().c_str()) != tinyxml2::XML_SUCCESS)
  {
    mitkThrow() << "Could not open file " << GetInputLocation()
      << " for reading.";
  }

  tinyxml2::XMLElement* pElem = doc.FirstChildElement();
  if (!pElem) { return result; }

  const char* typeAttr = pElem->Attribute("type");
  if (!typeAttr) { return result; }
  const std::string type = typeAttr;

  if (!Imeka::BoundingObject::BoundingObjectFactory::get()
    ->typeExists(type))
  {
    MITK_ERROR << "This BoundingObject type is not supported. "
      "See BoundingObjectWriter.cpp.\n" << __FILE__ << "  line " << __LINE__;
    return result;
  }

  auto bdo = Imeka::BoundingObject::BoundingObjectFactory::get()
    ->createBoundingObject(type);
  ReadWorldTransform(bdo, pElem);
  ReadOrigin(bdo, pElem);

  result.push_back(bdo.GetPointer());

  setlocale(LC_ALL, currLocale.c_str());
  MITK_INFO << "Bounding object read";

  return result;
}

void BoundingObjectReader::ReadOrigin(
  mitk::BoundingObject* boundingObject, tinyxml2::XMLElement* xmlHandle)
{
  double temp = 0.0;
  mitk::Point3D origin;
  tinyxml2::XMLElement* pElem = xmlHandle->FirstChildElement("origin");
  if (pElem)
  {
    pElem->QueryDoubleAttribute("x", &temp); origin[0] = temp;
    pElem->QueryDoubleAttribute("y", &temp); origin[1] = temp;
    pElem->QueryDoubleAttribute("z", &temp); origin[2] = temp;
    boundingObject->GetGeometry()->SetOrigin(origin);
  }
}

void BoundingObjectReader::ReadWorldTransform(
  mitk::BoundingObject* boundingObject, tinyxml2::XMLElement* xmlHandle)
{
  tinyxml2::XMLElement* matrixElem = xmlHandle->FirstChildElement("WorldTransformMatrix");
  if (!matrixElem) { return; }
  tinyxml2::XMLElement* row = matrixElem->FirstChildElement("Row");
  mitk::AffineTransform3D::MatrixType matrix;
  for (unsigned int i = 0; i < 3; ++i)
  {
    if (row)
    {
      double temp = 0.0;
      row->QueryDoubleAttribute("col1", &temp); matrix(i, 0) = temp;
      row->QueryDoubleAttribute("col2", &temp); matrix(i, 1) = temp;
      row->QueryDoubleAttribute("col3", &temp); matrix(i, 2) = temp;
      row = row->NextSiblingElement("Row");
    }
  }
  mitk::AffineTransform3D::Pointer transform = mitk::AffineTransform3D::New();
  transform->SetMatrix(matrix);
  boundingObject->GetGeometry()->SetIndexToWorldTransform(transform);
}

} // namespace IO

} // namespace Imeka
