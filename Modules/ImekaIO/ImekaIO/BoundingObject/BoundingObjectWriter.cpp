
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
  {
    tinyxml2::XMLElement* mainXML = documentXML.NewElement("mitkBoundingObject");
    mainXML->SetAttribute("type", type.c_str());
    documentXML.InsertEndChild(mainXML);

    WriteOrigin(mainXML, geo);
    WriteWorldTransform(mainXML, geo);
  }
  documentXML.SaveFile(GetOutputLocation().c_str());

  MITK_INFO << "Fiber bundle written";
}

void BoundingObjectWriter::WriteOrigin(
  tinyxml2::XMLElement* mainXML,
  const mitk::BaseGeometry* geo)
{
  const mitk::Point3D origin = geo->GetOrigin();
  tinyxml2::XMLElement* originXML = mainXML->GetDocument()->NewElement("origin");

  originXML->SetAttribute("x", origin[0]);
  originXML->SetAttribute("y", origin[1]);
  originXML->SetAttribute("z", origin[2]);

  mainXML->InsertEndChild(originXML);
}

void BoundingObjectWriter::WriteWorldTransform(
  tinyxml2::XMLElement* mainXML,
  const mitk::BaseGeometry* geo)
{
  const itk::MatrixOffsetTransformBase<mitk::ScalarType>::MatrixType matrix =
    geo->GetIndexToWorldTransform()->GetMatrix();

  tinyxml2::XMLElement* transformXML = mainXML->GetDocument()->NewElement("WorldTransformMatrix");
  for (unsigned int i = 0; i < 3; ++i)
  {
    tinyxml2::XMLElement* row = mainXML->GetDocument()->NewElement("Row");
    row->SetAttribute("col1", matrix(i, 0));
    row->SetAttribute("col2", matrix(i, 1));
    row->SetAttribute("col3", matrix(i, 2));
    transformXML->InsertEndChild(row);
  }
  mainXML->InsertEndChild(transformXML);
}

} // namespace IO

} // namespace Imeka
