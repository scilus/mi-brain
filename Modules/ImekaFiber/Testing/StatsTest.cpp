#include <mitkTestingMacros.h>

#include "ImekaFiber/Testing/FibersManagerTester.hpp"

class StatsTestSuite : public FibersManagerTester
{
  CPPUNIT_TEST_SUITE(StatsTestSuite);

  MITK_TEST(StatsAreOk);
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

  void StatsAreOk()
  {
    Imeka::Fiber::FiberIndexes visibility;
    m_Fibers->SetFibersVisibility(visibility);
    m_Fibers->CalculateStatsUsingVisibility();

    CPPUNIT_ASSERT_EQUAL(0, m_Fibers->GetFibersVisibility().size());
    CPPUNIT_ASSERT_EQUAL(10, m_Fibers->GetNumFibers());
    CPPUNIT_ASSERT_EQUAL(0.0f, m_Fibers->GetMinFiberLength());
    CPPUNIT_ASSERT_EQUAL(0.0f, m_Fibers->GetMaxFiberLength());
    CPPUNIT_ASSERT_EQUAL(0.0f, m_Fibers->GetMeanFiberLength());

    const std::vector<float> means{
      1.0, 1.5, 2.0, 2.5, 3, 3.5, 4.0, 4.5, 5, 5.5 };
    const std::vector<float> stddevs{
      0.0f, 0.70711f, 1.0f, 1.29099f, 1.58114f,
      1.87083f, 2.16025f, 2.44949f, 2.73861f, 3.02765f };
    for (int i = 0; i < 10; ++i)
    {
      visibility.insert(i);
      m_Fibers->SetFibersVisibility(visibility);
      m_Fibers->CalculateStatsUsingVisibility();

      CPPUNIT_ASSERT_EQUAL(i + 1, m_Fibers->GetFibersVisibility().size());
      CPPUNIT_ASSERT_EQUAL(1.0f, m_Fibers->GetMinFiberLength());
      CPPUNIT_ASSERT_EQUAL(1.0f * (i + 1), m_Fibers->GetMaxFiberLength());
      CPPUNIT_ASSERT_EQUAL(means[i], m_Fibers->GetMeanFiberLength());
      if (i > 0)
      {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(
          stddevs[i], m_Fibers->GetLengthStDev(), 0.00001);
      }
    }

    visibility.remove(0); visibility.remove(1);
    m_Fibers->SetFibersVisibility(visibility);
    m_Fibers->CalculateStatsUsingVisibility();

    CPPUNIT_ASSERT_EQUAL(8, m_Fibers->GetFibersVisibility().size());
    CPPUNIT_ASSERT_EQUAL(10, m_Fibers->GetNumFibers());
    CPPUNIT_ASSERT_EQUAL(3.0f, m_Fibers->GetMinFiberLength());
    CPPUNIT_ASSERT_EQUAL(10.0f, m_Fibers->GetMaxFiberLength());
    CPPUNIT_ASSERT_EQUAL(6.5f, m_Fibers->GetMeanFiberLength());
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
      2.44949f, m_Fibers->GetLengthStDev(), 0.00001);
  }
};

MITK_TEST_SUITE_REGISTRATION(Stats)
