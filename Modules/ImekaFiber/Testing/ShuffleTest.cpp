#include <mitkTestingMacros.h>

#include "ImekaFiber/Testing/FibersManagerTester.hpp"

class ShuffleTestSuite : public FibersManagerTester
{
  CPPUNIT_TEST_SUITE(ShuffleTestSuite);
  MITK_TEST(Shuffle);
  CPPUNIT_TEST_SUITE_END();

private:
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

  void Shuffle()
  {
    {
      auto colors = mitk::FilteredFiberBundle::GetNewColorArray(20);
      for (unsigned char i = 0; i < 10; ++i)
      {
        unsigned char rgba[] = { i, i, i, 255 };
        colors->InsertNextTypedTuple(rgba);
        colors->InsertNextTypedTuple(rgba);
      }
      m_Fibers->SetFiberColors(colors);
    }

    m_FibersNode->SetBoolProperty("ShuffleFibers", true);

    unsigned char rgba[] = { 0, 0, 0, 255 };
    auto colors = m_Fibers->GetFiberColors();
    for (unsigned int i = 0; i < 20; ++i)
    {
      colors->GetTypedTuple(i, rgba);
      CPPUNIT_ASSERT(rgba[0] == rgba[1] && rgba[1] == rgba[2]);
      CPPUNIT_ASSERT(rgba[0] < 10);
    }
  }
};

MITK_TEST_SUITE_REGISTRATION(Shuffle)
