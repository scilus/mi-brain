#include <mitkTestingMacros.h>

#include "ImekaFiber/Testing/FibersManagerTester.hpp"

class MinMaxFilteringTestSuite : public FibersManagerTester
{
  CPPUNIT_TEST_SUITE(MinMaxFilteringTestSuite);

  MITK_TEST(LengthFilteringIsOk);
  CPPUNIT_TEST_SUITE_END();

private:
  mitk::FilteredFiberBundle::Pointer m_Fibers;

public:
  void setUp() override
  {
    FibersManagerTester::setUp();

    auto geo = mitk::Geometry3D::New();
    m_Fibers = Get1_10Fibers(geo, false);
  }

  void tearDown() override
  {
    m_Fibers = nullptr;
    FibersManagerTester::tearDown();
  }

  void LengthFilteringIsOk()
  {
    auto node = mitk::DataNode::New();
    node->SetData(m_Fibers);
    node->SetName("1_10Fibers");

    m_DM->AddNode(node);

    const Imeka::Fiber::FiberMapperData& mapperData =
      m_FM->GetFiberNodeData(node).fiberMapperData;
    CPPUNIT_ASSERT_EQUAL(true, mapperData.useWhole);
    CPPUNIT_ASSERT_EQUAL(
      static_cast<size_t>(0), mapperData.partsMapperData.size());
    CPPUNIT_ASSERT_EQUAL(20u, mapperData.TotalNbPoints());
    CPPUNIT_ASSERT_EQUAL(20u, mapperData.wholeMapperData.NbPoints());
    CPPUNIT_ASSERT_EQUAL(10, static_cast<int>(mapperData.wholeMapperData.GetIndices().size()));

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(20), mapperData.GetIBO().size());
    CPPUNIT_ASSERT_EQUAL(10, static_cast<int>(m_Fibers->GetFibersVisibility().size()));

    m_FM->SetFilteringLengths(0.0, 10.0);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(20), mapperData.GetIBO().size());
    CPPUNIT_ASSERT_EQUAL(10, static_cast<int>(m_Fibers->GetFibersVisibility().size()));

    m_FM->SetFilteringLengths(1.0, 9.0);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(18), mapperData.GetIBO().size());
    CPPUNIT_ASSERT_EQUAL(9, static_cast<int>(m_Fibers->GetFibersVisibility().size()));

    m_FM->SetFilteringLengths(1.0, 8.0);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(16), mapperData.GetIBO().size());
    CPPUNIT_ASSERT_EQUAL(8, static_cast<int>(m_Fibers->GetFibersVisibility().size()));

    m_FM->SetFilteringLengths(3.0, 6.0);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(8), mapperData.GetIBO().size());
    CPPUNIT_ASSERT_EQUAL(4, static_cast<int>(m_Fibers->GetFibersVisibility().size()));

    m_FM->SetFilteringLengths(3.0, 3.0);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), mapperData.GetIBO().size());
    CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(m_Fibers->GetFibersVisibility().size()));

    CPPUNIT_ASSERT_EQUAL(true, mapperData.useWhole);

    CPPUNIT_ASSERT_EQUAL(
      static_cast<size_t>(0), mapperData.partsMapperData.size());
    CPPUNIT_ASSERT_EQUAL(2u, mapperData.TotalNbPoints());
    CPPUNIT_ASSERT_EQUAL(2u, mapperData.wholeMapperData.NbPoints());
    CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(mapperData.wholeMapperData.GetIndices().size()));
  }
};

MITK_TEST_SUITE_REGISTRATION(MinMaxFiltering)
