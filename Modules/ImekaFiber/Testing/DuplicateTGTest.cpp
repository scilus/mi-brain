#include <mitkTestingMacros.h>

#include "ImekaCommon/Predicate.hpp"
#include "ImekaFiber/Testing/FibersManagerTester.hpp"

class DuplicateTGTestSuite : public FibersManagerTester
{
  CPPUNIT_TEST_SUITE(DuplicateTGTestSuite);

  MITK_TEST(TestDuplicateTG);
  CPPUNIT_TEST_SUITE_END();

private:
  mitk::FilteredFiberBundle::Pointer m_Fibers;
  mitk::DataNode::Pointer m_FibersNode, m_TGNode, m_ROINode;
 

public:
  void setUp() override
  {
    FibersManagerTester::setUp();

    auto geo = mitk::Geometry3D::New();
    m_Fibers = Get1_10Fibers(geo, true);
    m_FibersNode = AddFibersNode(m_Fibers, "FibersNode");

    const mitk::Vector3D spacing(1.0);
    const mitk::Point3D origin(0.0);
    auto t = SetupTG(m_FibersNode, spacing, origin);
    m_TGNode = t.first;
    m_ROINode = m_DM->GetNodeContainingThis(t.second);
  }

  void tearDown() override
  {
    m_Fibers = nullptr;
    m_FibersNode = m_TGNode = nullptr;
    FibersManagerTester::tearDown();
  }

  void TestDuplicateTG()
  {
    CPPUNIT_ASSERT_EQUAL(
      static_cast<size_t>(1),
      m_DM->DirectChildrenOf(m_FibersNode).size());

    auto nbFibers = m_Fibers->GetFibersVisibility().size();
    m_TGNode->SetBoolProperty("Duplicate", true);
    CPPUNIT_ASSERT_EQUAL(nbFibers, m_Fibers->GetFibersVisibility().size());

    auto allTG = m_DM->DirectChildrenOf(m_FibersNode);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), allTG.size());
    auto newTGNode = (allTG[0]->GetName() == "Dup New Tract Group")
      ? allTG[0] : allTG[1];
    auto newActiveROI = m_DM->GetAllNodes(newTGNode)[0];
    CPPUNIT_ASSERT_EQUAL(false, newTGNode->IsVisible(nullptr));
    CPPUNIT_ASSERT_EQUAL(false, newActiveROI->IsVisible(nullptr));

    // Enable it. Shouldn't change anything.
    newTGNode->SetVisibility(true);
    CPPUNIT_ASSERT_EQUAL(true, newTGNode->IsVisible(nullptr));
    CPPUNIT_ASSERT_EQUAL(true, newActiveROI->IsVisible(nullptr));
    CPPUNIT_ASSERT_EQUAL(nbFibers, m_Fibers->GetFibersVisibility().size());
    newActiveROI->SetVisibility(false);
    CPPUNIT_ASSERT_EQUAL(false, newTGNode->IsVisible(nullptr));
    CPPUNIT_ASSERT_EQUAL(false, newActiveROI->IsVisible(nullptr));
    CPPUNIT_ASSERT_EQUAL(nbFibers, m_Fibers->GetFibersVisibility().size());
    newActiveROI->SetVisibility(true);

    // Edit the new TG into a NOT, so we should see everything
    AutoSetupLines autoSetup = {
      { m_ROINode,
        { nullptr, true, Imeka::Fiber::SelectionMode::AnyPart, true }}
    };
    EditTG(newTGNode, autoSetup);

    CPPUNIT_ASSERT_EQUAL(10, m_Fibers->GetFibersVisibility().size());
    m_TGNode->SetVisibility(false);
    CPPUNIT_ASSERT_EQUAL(7, m_Fibers->GetFibersVisibility().size());
  }
};

MITK_TEST_SUITE_REGISTRATION(DuplicateTG)
