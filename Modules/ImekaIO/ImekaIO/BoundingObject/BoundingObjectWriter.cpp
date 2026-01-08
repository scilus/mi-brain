
#include "BoundingObjectWriter.hpp"

#include <mitkBoundingObject.h>
#include <tinyxml2.h>

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

  tinyxml2::XMLDocument documentXML;
  tinyxml2::XMLDeclaration* declXML = documentXML.NewDeclaration("xml version=\"1.0\"");
  documentXML.InsertFirstChild(declXML);

  tinyxml2::XMLElement* mainXML = documentXML.NewElement("mitkBoundingObject");
  mainXML->SetAttribute("type", type.c_str());
  documentXML.InsertEndChild(mainXML);

  WriteOrigin(mainXML, geo, &documentXML);
  WriteWorldTransform(mainXML, geo, &documentXML);

  documentXML.SaveFile(GetOutputLocation().c_str());

  MITK_INFO << "Fiber bundle written";
}

void BoundingObjectWriter::WriteOrigin(
  tinyxml2::XMLElement* mainXML,
  const mitk::BaseGeometry* geo,
  tinyxml2::XMLDocument* doc)
{
  const mitk::Point3D origin = geo->GetOrigin();
  tinyxml2::XMLElement* originXML = doc->NewElement("origin");

  originXML->SetAttribute("x", std::to_string(origin[0]).c_str());
  originXML->SetAttribute("y", std::to_string(origin[1]).c_str());
  originXML->SetAttribute("z", std::to_string(origin[2]).c_str());

  mainXML->InsertEndChild(originXML);
}

void BoundingObjectWriter::WriteWorldTransform(
  tinyxml2::XMLElement* mainXML,
  const mitk::BaseGeometry* geo,
  tinyxml2::XMLDocument* doc)
{
  const itk::MatrixOffsetTransformBase<mitk::ScalarType>::MatrixType matrix =
    geo->GetIndexToWorldTransform()->GetMatrix();

  tinyxml2::XMLElement* transformXML = doc->NewElement("WorldTransformMatrix");
  for (unsigned int i = 0; i < 3; ++i)
  {
    tinyxml2::XMLElement* row = doc->NewElement("Row");
    row->SetAttribute("col1", std::to_string(matrix(i, 0)).c_str());
    row->SetAttribute("col2", std::to_string(matrix(i, 1)).c_str());
    row->SetAttribute("col3", std::to_string(matrix(i, 2)).c_str());
    transformXML->InsertEndChild(row);
  }
  mainXML->InsertEndChild(transformXML);
}

} // namespace IO

} // namespace Imeka
