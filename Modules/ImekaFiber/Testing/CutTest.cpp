#include <mitkTestingMacros.h>

#include "ImekaBoundingObject/BoundingObjectFactory.hpp"
#include "ImekaBoundingObject/NodeUtils.hpp"
#include "ImekaCommon/Colors.hpp"
#include "ImekaCommon/Predicate.hpp"
#include "ImekaFiber/Testing/FibersManagerTester.hpp"
#include "ImekaGeometry/MathUtils.hpp"

const QColor ORANGE(255, 100, 0);

class CutTestSuite : public FibersManagerTester
{
  CPPUNIT_TEST_SUITE(CutTestSuite);

  MITK_TEST(CutInside);
  MITK_TEST(CutInsideMiddleOfNowhere);
  MITK_TEST(TooBigToCut);
  MITK_TEST(CutOutside);
  MITK_TEST(CutInsideTG);
  MITK_TEST(CutOutsideTG);
  MITK_TEST(CutColor);
  CPPUNIT_TEST_SUITE_END();

private:
  mitk::FilteredFiberBundle::Pointer m_Fibers;
  mitk::DataNode::Pointer m_FibersNode;
  mitk::BoundingObject::Pointer m_SO;
  mitk::DataNode::Pointer m_SONode;

  mitk::FilteredFiberBundle::Pointer Cut(
    mitk::DataNode* fibersNode,
    const char* cutWhat,
    const mitk::Vector3D& spacing,
    const mitk::Point3D& origin)
  {
    auto geo = m_SO->GetGeometry();
    geo->SetSpacing(spacing);
    geo->SetOrigin(origin);

    fibersNode->SetStringProperty("Cut", cutWhat);

    auto newNode = m_DM->GetFirst(
      Imeka::Predicate::Property("name", "FibersNode cut"));
    return dynamic_cast<mitk::FilteredFiberBundle*>(newNode->GetData());
  }

public:
  void setUp() override
  {
    FibersManagerTester::setUp();

    auto geo = mitk::Geometry3D::New();
    m_Fibers = Get1_10Fibers(geo, false);
    m_FibersNode = AddFibersNode(m_Fibers);

    m_SO = Imeka::BoundingObject::
      BoundingObjectFactory::get()->createBoundingObject("Cuboid");
    m_SONode = Imeka::BoundingObject::GetNewBoundingObjectNode(m_SO, false);
    m_SONode->SetBoolProperty("CutWithMeSenpai!", true);
    m_DM->AddNode(m_SONode);
  }

  void tearDown() override
  {
    m_SO = nullptr;
    m_FibersNode = m_SONode = nullptr;
    FibersManagerTester::tearDown();
  }

  void CutInside()
  {
    auto newFibers = Cut(
      m_FibersNode, "Inside",
      Vector3D(2.0, 1.0, 10.0), Point3D(0.0, 0.0, 0.0));
    CPPUNIT_ASSERT_EQUAL(3, newFibers->GetNumFibers());
    CPPUNIT_ASSERT_EQUAL(1.0f, newFibers->GetMinFiberLength());
    CPPUNIT_ASSERT_EQUAL(2.0f, newFibers->GetMaxFiberLength());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
      5.0 / 3.0, newFibers->GetMeanFiberLength(), 0.000001);

    float thickness = 0.0;
    auto cutedNode = m_DM->GetNodeContainingThis(newFibers);
    cutedNode->GetPropertyValue("Fiber2DSliceThickness", thickness);
    CPPUNIT_ASSERT_EQUAL(0.5f, thickness);
  }

  void CutInsideMiddleOfNowhere()
  {
    auto newFibers = Cut(
      m_FibersNode, "Inside",
      Vector3D(3.0, 2.0, 5.0), Point3D(2.0, 4.0, 0.0));
    CPPUNIT_ASSERT_EQUAL(6, newFibers->GetNumFibers());
    CPPUNIT_ASSERT_EQUAL(5.0f, newFibers->GetMinFiberLength());
    CPPUNIT_ASSERT_EQUAL(5.0f, newFibers->GetMaxFiberLength());
    CPPUNIT_ASSERT_EQUAL(5.0f, newFibers->GetMeanFiberLength());
  }

  void TooBigToCut()
  {
    auto newFibers = Cut(
      m_FibersNode, "Inside",
      Vector3D(20.0, 20.0, 0.5), Point3D(5.0, 5.0, 0.0));
    CPPUNIT_ASSERT_EQUAL(
      m_Fibers->GetNumFibers(),
      newFibers->GetNumFibers());
    CPPUNIT_ASSERT_EQUAL(
      m_Fibers->GetMinFiberLength(),
      newFibers->GetMinFiberLength());
    CPPUNIT_ASSERT_EQUAL(
      m_Fibers->GetMaxFiberLength(),
      newFibers->GetMaxFiberLength());
    CPPUNIT_ASSERT_EQUAL(
      m_Fibers->GetMeanFiberLength(),
      newFibers->GetMeanFiberLength());

    // We can't compare them using Equals here because Cut shuffles the
    // streamlines and Equals assumes that everything is ordered.
  }

  void CutOutside()
  {
    auto newFibers = Cut(
      m_FibersNode, "Outside",
      Vector3D(2.0, 1.0, 10.0), Point3D(0.0, 0.0, 0.0));
    CPPUNIT_ASSERT_EQUAL(8, newFibers->GetNumFibers());
    CPPUNIT_ASSERT_EQUAL(1.0f, newFibers->GetMinFiberLength());
    CPPUNIT_ASSERT_EQUAL(10.0f, newFibers->GetMaxFiberLength());
    CPPUNIT_ASSERT_EQUAL(6.25f, newFibers->GetMeanFiberLength());
  }

  void CutInsideTG()
  {
    const auto origin = Point3D(0.0, 2.0, 0.0);
    const auto TGNode = SetupTG(
      m_FibersNode, Vector3D(2.5, 1.001, 2.5), origin
    ).first;

    Imeka::Color::Apply(m_FibersNode, ORANGE);

    // Use another SO, same origin but bigger
    auto newFibers = Cut(
      TGNode, "Inside", Vector3D(3.5, 1.501, 5.0), origin);
    CPPUNIT_ASSERT_EQUAL(5, newFibers->GetNumFibers());
    CPPUNIT_ASSERT_EQUAL(3.0f, newFibers->GetMinFiberLength());
    CPPUNIT_ASSERT_EQUAL(3.5f, newFibers->GetMaxFiberLength());
    CPPUNIT_ASSERT_EQUAL(3.4f, newFibers->GetMeanFiberLength());

    auto newFibersNode = m_DM->GetNodeContainingThis(newFibers);
    AssertColorAre(newFibersNode, { ORANGE, ORANGE });
  }

  void CutOutsideTG()
  {
    const auto origin = Point3D(0.0, 2.0, 0.0);
    const auto TGNode = SetupTG(
      m_FibersNode, Vector3D(2.5, 1.001, 2.5), origin
    ).first;

    Imeka::Color::Apply(m_FibersNode, ORANGE);

    // Use another SO, same origin but bigger
    auto newFibers = Cut(
      TGNode, "Outside", Vector3D(4.0, 1.501, 5.0), origin);
    CPPUNIT_ASSERT_EQUAL(3, newFibers->GetNumFibers());
    CPPUNIT_ASSERT_EQUAL(1.0f, newFibers->GetMinFiberLength());
    CPPUNIT_ASSERT_EQUAL(3.0f, newFibers->GetMaxFiberLength());
    CPPUNIT_ASSERT_EQUAL(2.0f, newFibers->GetMeanFiberLength());

    auto newFibersNode = m_DM->GetNodeContainingThis(newFibers);
    AssertColorAre(newFibersNode, { ORANGE, ORANGE });
  }

  void CutColor()
  {
    Imeka::Color::Apply(m_FibersNode, ORANGE);

    auto newFibers = Cut(
      m_FibersNode, "Inside",
      Vector3D(2.0, 1.0, 10.0), Point3D(0.0, 0.0, 0.0));
    auto newFibersNode = m_DM->GetNodeContainingThis(newFibers);

    AssertColorAre(newFibersNode, { ORANGE, ORANGE });
  }
};

MITK_TEST_SUITE_REGISTRATION(Cut)
