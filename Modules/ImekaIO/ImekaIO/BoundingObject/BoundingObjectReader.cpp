
#include "BoundingObjectReader.hpp"

#include <mitkBoundingObject.h>
#include <itksys/SystemTools.hxx>
#include <tinyxml.h>

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
BoundingObjectReader::Read()
{
  std::vector<itk::SmartPointer<mitk::BaseData>> result;

  const std::string& locale = "C";
  const std::string& currLocale = setlocale( LC_ALL, nullptr );
  setlocale(LC_ALL, locale.c_str());

  TiXmlDocument doc(GetInputLocation().c_str());
  if (!doc.LoadFile())
  {
    mitkThrow() << "Could not open file " << GetInputLocation()
      << " for reading.";
  }

  TiXmlHandle hDoc(&doc);
  TiXmlElement* pElem;
  TiXmlHandle hRoot(0);

  pElem = hDoc.FirstChildElement().Element();
  hRoot = TiXmlHandle(pElem);

  const std::string type = pElem->Attribute("type");
  if (!Imeka::BoundingObject::BoundingObjectFactory::get()
    ->typeExists(type))
  {
    MITK_ERROR << "This BoundingObject type is not supported. "
      "See BoundingObjectWriter.cpp.\n" << __FILE__ << "  line " << __LINE__;
    return result;
  }

  auto bdo = Imeka::BoundingObject::BoundingObjectFactory::get()
    ->createBoundingObject(type);
  ReadWorldTransform(bdo, hRoot);
  ReadOrigin(bdo, hRoot);

  result.push_back(bdo.GetPointer());

  setlocale(LC_ALL, currLocale.c_str());
  MITK_INFO << "Bounding object read";

  return result;
}

void BoundingObjectReader::ReadOrigin(
  mitk::BoundingObject* boundingObject, TiXmlHandle xmlHandle)
{
  double temp = 0.0;
  mitk::Point3D origin;
  TiXmlElement* pElem = xmlHandle.FirstChildElement("origin").Element();
  pElem->Attribute("x", &temp); origin[0] = temp;
  pElem->Attribute("y", &temp); origin[1] = temp;
  pElem->Attribute("z", &temp); origin[2] = temp;
  boundingObject->GetGeometry()->SetOrigin(origin);
}

void BoundingObjectReader::ReadWorldTransform(
  mitk::BoundingObject* boundingObject, TiXmlHandle xmlHandle)
{
  TiXmlElement* row = xmlHandle.FirstChildElement("WorldTransformMatrix")
    .Element()->FirstChildElement("Row");
  mitk::AffineTransform3D::MatrixType matrix;
  for (unsigned int i = 0; i < 3; ++i)
  {
    double temp = 0.0;
    row->Attribute("col1", &temp); matrix(i, 0) = temp;
    row->Attribute("col2", &temp); matrix(i, 1) = temp;
    row->Attribute("col3", &temp); matrix(i, 2) = temp;
    row = row->NextSiblingElement("Row");
  }
  mitk::AffineTransform3D::Pointer transform = mitk::AffineTransform3D::New();
  transform->SetMatrix(matrix);
  boundingObject->GetGeometry()->SetIndexToWorldTransform(transform);
}

} // namespace IO

} // namespace Imeka
