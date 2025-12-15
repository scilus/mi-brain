#include <mitkBoundingObject.h>
#include <mitkTestFixture.h>
#include <mitkTestingMacros.h>
#include <QmitkIOUtil.h>

#include "ImekaBoundingObject/BoundingObjectFactory.hpp"

class BoundingObjectTestSuite : public mitk::TestFixture
{
  CPPUNIT_TEST_SUITE(BoundingObjectTestSuite);
  MITK_TEST(TestBoundingObject);
  CPPUNIT_TEST_SUITE_END();

public:
  void TestBoundingObject()
  {
    const auto names =
      Imeka::BoundingObject::BoundingObjectFactory::get()->names();
    if (names.empty())
    {
      MITK_TEST_FAILED_MSG(<<"Wrong number of BoundingObject.")
    }

    const std::string type = names[0];
    auto boundingObject = Imeka::BoundingObject::BoundingObjectFactory::get()
      ->createBoundingObject(type);
    if (boundingObject.IsNull())
    {
      MITK_TEST_FAILED_MSG(
        <<"Can't create a BoundingObject with BoundingObjectFactory.")
    }

    mitk::AffineTransform3D::MatrixType matrix;
    matrix(0, 0) = 0.0; matrix(0, 1) = 0.1; matrix(0, 2) = 0.2;
    matrix(1, 0) = 1.0; matrix(1, 1) = 1.1; matrix(1, 2) = 1.2;
    matrix(2, 0) = 2.0; matrix(2, 1) = 2.1; matrix(2, 2) = 2.2;
    auto transform = mitk::AffineTransform3D::New();
    transform->SetMatrix(matrix);
    boundingObject->GetGeometry()->SetIndexToWorldTransform(transform);

    mitk::Point3D origin;
    origin[0] = 4.765487194;
    origin[1] = 3.92581749;
    origin[2] = 5.359539032;
    boundingObject->GetGeometry()->SetOrigin(origin);

    const std::string fileName = "savedBoundingObject.bdo";
    mitk::IOUtil::Save(boundingObject, fileName);

    auto baseData = mitk::IOUtil::Load(fileName)[0];
    mitk::BoundingObject::Pointer loadedBoundingObject =
      dynamic_cast<mitk::BoundingObject*>(baseData.GetPointer());
    if (type != loadedBoundingObject->GetNameOfClass())
    {
      MITK_TEST_FAILED_MSG(<<"Loaded BoundingObject's type is different"
        " from saved type.")
    }

    const auto loadedOrigin = boundingObject->GetGeometry()->GetOrigin();
    if (origin != loadedOrigin)
    {
      MITK_TEST_FAILED_MSG(<<"Origin is different on loaded BoundingObject.")
    }

    const auto loadedMatrix =
      boundingObject->GetGeometry()->GetIndexToWorldTransform()->GetMatrix();
    if (matrix != loadedMatrix)
    {
      MITK_TEST_FAILED_MSG(<<"World transform is different on loaded BoundingObject.")
    }
  }

};

MITK_TEST_SUITE_REGISTRATION(BoundingObject)
