
#ifndef IMEKA_FIBER_TESTING_TRACTS_TESTER_HPP_INCLUDED
#define IMEKA_FIBER_TESTING_TRACTS_TESTER_HPP_INCLUDED

#include "FibersManagerTester.hpp"

#include "ImekaFiberExports.h"

// Tests the loading and saving of trk, tck and maybe others
class ImekaFiber_EXPORT TractsTester : public FibersManagerTester
{
public:
  TractsTester(const bool needAnat, const std::string& type);

  void setUp() override;

protected:
  void SavingLoadingToyData();
  void SavingLoadingRealData();

  void FailIfDifferent(
    mitk::FilteredFiberBundle* fibers,
    mitk::FilteredFiberBundle* loaded) const;

private:
  const bool m_NeedAnat;
  const std::string m_Type;

protected:
  const std::string m_ToyTractsPath;
  const std::string m_RealFAPath;
  const std::string m_RealTractsPath;
};

#endif // IMEKA_FIBER_TESTING_TRACTS_TESTER_HPP_INCLUDED
