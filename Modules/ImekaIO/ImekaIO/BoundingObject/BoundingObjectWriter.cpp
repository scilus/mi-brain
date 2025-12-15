
#include "BoundingObjectWriter.hpp"

#include <mitkBoundingObject.h>
#include <tinyxml.h>

#include "../MimeType.hpp"
#include "ImekaBoundingObject/BoundingObjectFactory.hpp"

namespace Imeka
{

namespace IO
{

BoundingObjectWriter::BoundingObjectWriter()
  : mitk::AbstractFileWriter(mitk::BoundingObject::GetStaticNameOfClass(),
                             GetBoundingObjectMimeType(),
                             GetBoundingObjectDescription())
{
  RegisterService();
}

BoundingObjectWriter::BoundingObjectWriter(const BoundingObjectWriter& other)
  : mitk::AbstractFileWriter(other)
{}

BoundingObjectWriter* BoundingObjectWriter::Clone() const
{
  return new BoundingObjectWriter(*this);
}

void BoundingObjectWriter::Write()
{
  const std::string type = GetInput()->GetNameOfClass();
  if (!Imeka::BoundingObject::BoundingObjectFactory::get()->typeExists(type))
  {
    MITK_ERROR << "This BoundingObject type is not supported."
      "See BoundingObjectWriter.cpp.";
    return;
  }

  mitk::BaseGeometry* geo = GetInput()->GetGeometry();

  TiXmlDocument documentXML;
  {
    TiXmlDeclaration* declXML = new TiXmlDeclaration("1.0", "", "");
    documentXML.LinkEndChild(declXML);

    TiXmlElement* mainXML = new TiXmlElement("mitkBoundingObject");
    mainXML->SetAttribute("type", type);
    documentXML.LinkEndChild(mainXML);

    WriteOrigin(mainXML, geo);
    WriteWorldTransform(mainXML, geo);
  }
  documentXML.SaveFile(GetOutputLocation());

  MITK_INFO << "Fiber bundle written";
}

void BoundingObjectWriter::WriteOrigin(
  TiXmlElement* mainXML,
  const mitk::BaseGeometry* geo)
{
  const mitk::Point3D origin = geo->GetOrigin();
  TiXmlElement* originXML = new TiXmlElement("origin");

  originXML->SetAttribute("x", std::to_string(origin[0]));
  originXML->SetAttribute("y", std::to_string(origin[1]));
  originXML->SetAttribute("z", std::to_string(origin[2]));

  mainXML->LinkEndChild(originXML);
}

void BoundingObjectWriter::WriteWorldTransform(
  TiXmlElement* mainXML,
  const mitk::BaseGeometry* geo)
{
  const itk::MatrixOffsetTransformBase<mitk::ScalarType>::MatrixType matrix =
    geo->GetIndexToWorldTransform()->GetMatrix();

  TiXmlElement* transformXML = new TiXmlElement("WorldTransformMatrix");
  for (unsigned int i = 0; i < 3; ++i)
  {
    TiXmlElement* row = new TiXmlElement("Row");
    row->SetAttribute("col1", std::to_string(matrix(i, 0)));
    row->SetAttribute("col2", std::to_string(matrix(i, 1)));
    row->SetAttribute("col3", std::to_string(matrix(i, 2)));
    transformXML->LinkEndChild(row);
  }
  mainXML->LinkEndChild(transformXML);
}

} // namespace IO

} // namespace Imeka
