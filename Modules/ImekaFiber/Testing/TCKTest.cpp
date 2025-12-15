#include <mitkTestingMacros.h>

#include "ImekaFiber/Testing/TractsTester.hpp"

class TCKTestSuite : public TractsTester
{
  CPPUNIT_TEST_SUITE(TCKTestSuite);

  MITK_TEST(SavingLoadingToyData);
  MITK_TEST(SavingLoadingRealData);
  CPPUNIT_TEST_SUITE_END();

public:
  TCKTestSuite()
    : TractsTester(false, "tck")
  {}
};

MITK_TEST_SUITE_REGISTRATION(TCK)
