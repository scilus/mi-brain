#include <mitkTestingMacros.h>

#include "ImekaBoundingObject/BoundingObjectFactory.hpp"
#include "ImekaBoundingObject/NodeUtils.hpp"
#include "ImekaCommon/Predicate.hpp"
#include "ImekaFiber/Testing/FibersManagerTester.hpp"
#include "ImekaGeometry/MathUtils.hpp"

class MirrorTestSuite : public FibersManagerTester
{
  CPPUNIT_TEST_SUITE(MirrorTestSuite);
  MITK_TEST(MirrorAxial);
  MITK_TEST(MirrorCoronal);
  MITK_TEST(MirrorSagittal);
  MITK_TEST(TestFromAnatAutoUpdateMirror);
  MITK_TEST(MirrorFiltering);

  CPPUNIT_TEST_SUITE_END();

private:
  void Check(
    mitk::FilteredFiberBundle* fibers,
    const bool X, const bool Y, const bool Z)
  {
    float addX = 0.0, addY = 0.0, addZ = 0.0;
    float _X = 1.0, _Y = 1.0;
    if (X) { addX = 9.0; _X = -1.0; }
    if (Y) { addY = 9.0; _Y = -1.0; }
    if (Z) { addZ = 9.0; }

    vtkPoints* points = fibers->GetFiberPolyData()->GetPoints();
    AssertPointIs(points, 0, _X * 0.0 + addX, _Y * 0.0 + addY, 0.0 + addZ);
    AssertPointIs(points, 1, _X * 1.0 + addX, _Y * 0.0 + addY, 0.0 + addZ);
    AssertPointIs(points, 2, _X * 0.0 + addX, _Y * 0.5 + addY, 0.0 + addZ);
    AssertPointIs(points, 3, _X * 2.0 + addX, _Y * 0.5 + addY, 0.0 + addZ);
    AssertPointIs(points, 4, _X * 0.0 + addX, _Y * 1.0 + addY, 0.0 + addZ);
    AssertPointIs(points, 5, _X * 3.0 + addX, _Y * 1.0 + addY, 0.0 + addZ);
    AssertPointIs(points, 6, _X * 0.0 + addX, _Y * 1.5 + addY, 0.0 + addZ);
    AssertPointIs(points, 7, _X * 4.0 + addX, _Y * 1.5 + addY, 0.0 + addZ);
    AssertPointIs(points, 8, _X * 0.0 + addX, _Y * 2.0 + addY, 0.0 + addZ);
    AssertPointIs(points, 9, _X * 5.0 + addX, _Y * 2.0 + addY, 0.0 + addZ);
    AssertPointIs(points, 10, _X * 0.0 + addX, _Y * 2.5 + addY, 0.0 + addZ);
    AssertPointIs(points, 11, _X * 6.0 + addX, _Y * 2.5 + addY, 0.0 + addZ);
    AssertPointIs(points, 12, _X * 0.0 + addX, _Y * 3.0 + addY, 0.0 + addZ);
    AssertPointIs(points, 13, _X * 7.0 + addX, _Y * 3.0 + addY, 0.0 + addZ);
  }

  void AssertPointIs(
    vtkPoints* points,
    const vtkIdType idx,
    const float x, const float y, const float z)
  {
    mitk::Point3D p;
    points->GetPoint(idx, p.Begin());

    if (p[0] != x || p[1] != y || p[2] != z)
    {
      std::stringstream ss;
      ss << "equality assertion failed\n"
        << "- Expected: " << x << ", " << y << ", " << z << "\n"
        << "- Actual  : " << p[0] << ", " << p[1] << ", " << p[2] << "\n";
      CPPUNIT_FAIL(ss.str().c_str());
    }
  }

  mitk::Image::Pointer m_Anat;
  mitk::FilteredFiberBundle::Pointer m_Fibers;
  mitk::DataNode::Pointer m_FibersNode;

public:
  void setUp() override
  {
    FibersManagerTester::setUp();

    m_Anat = GetAnat();
    AddAnatNode(m_Anat);
    m_Fibers = Get1_10Fibers(m_Anat->GetGeometry(), false);
    m_FibersNode = AddFibersNode(m_Fibers);
  }

  void tearDown() override
  {
    m_Fibers = nullptr;
    m_FibersNode = nullptr;
    FibersManagerTester::tearDown();
  }

  void MirrorAxial()
  {
    m_FibersNode->SetIntProperty("MirrorFibers", 2);
    Check(m_Fibers, false, false, true);
  }

  void MirrorCoronal()
  {
    m_FibersNode->SetIntProperty("MirrorFibers", 1);
    Check(m_Fibers, false, true, false);
  }

  void MirrorSagittal()
  {
    m_FibersNode->SetIntProperty("MirrorFibers", 0);
    Check(m_Fibers, true, false, false);
  }

  void TestFromAnatAutoUpdateMirror()
  {
    auto fibers = Get1_10Fibers(m_Anat->GetGeometry(), true);
    auto node1 = AddFibersNode(fibers, "FibersNode1");
    node1->SetIntProperty(
      "ColorType", mitk::FilteredFiberBundle::Coloring::FromAnat);
    node1->SetIntProperty("MirrorFibers", 0);

    // The `+ 12` is working only because we have exactly 3 points per
    // streamline. Each point is 4 bytes.
    auto colors = fibers->GetFiberColors()->GetPointer(0);
    AssertColorIs(QColor(163, 255, 92), colors + 0);
    AssertColorIs(QColor(99, 255, 155), colors + 4);
    AssertColorIs(QColor(163, 255, 92), colors + 8);
    colors += 12;
    AssertColorIs(QColor(223, 255, 32), colors + 0);
    AssertColorIs(QColor(99, 255, 155), colors + 4);
    AssertColorIs(QColor(223, 255, 32), colors + 8);
    colors += 12;
    AssertColorIs(QColor(223, 255, 32), colors + 0);
    AssertColorIs(QColor(39, 255, 215), colors + 4);
    AssertColorIs(QColor(223, 255, 32), colors + 8);
  }

  void MirrorFiltering()
  {
    // Place the SO in an empty zone then mirror the image and we should have
    // some visibile streamlines because they should be right on the SO now.
    auto TGNode = SetupTG(
      m_FibersNode,
      Vector3D(1.5, 1.5, 1.5),
      Point3D(8.0, 0.0, 0.0)).first;

    const auto& mapperData =
      m_FM->GetFiberNodeData(m_FibersNode).fiberMapperData;
    const auto& pmd = mapperData.partsMapperData.at(TGNode);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), mapperData.GetIBO().size());
    CPPUNIT_ASSERT_EQUAL(0, pmd.GetIndices().size());

    m_FibersNode->SetIntProperty("MirrorFibers", 0);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), mapperData.GetIBO().size());
    CPPUNIT_ASSERT_EQUAL(4, pmd.GetIndices().size());
  }
};

MITK_TEST_SUITE_REGISTRATION(Mirror)
