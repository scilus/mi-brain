#include <mitkIOUtil.h>
#include <mitkTestingMacros.h>

#include "ImekaFiber/Testing/TractsTester.hpp"

class TRKTestSuite : public TractsTester
{
  CPPUNIT_TEST_SUITE(TRKTestSuite);

  MITK_TEST(SavingLoadingToyData);
  MITK_TEST(SavingLoadingRealData);
  CPPUNIT_TEST_SUITE_END();

public:
  TRKTestSuite()
    : TractsTester(true, "trk")
  {}
};

MITK_TEST_SUITE_REGISTRATION(TRK)
