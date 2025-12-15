
#include "TractsTester.hpp"

#include <mitkIOUtil.h>
#include <mitkTestingMacros.h>

TractsTester::TractsTester(const bool needAnat, const std::string& type)
  : m_NeedAnat(needAnat)
  , m_Type(type)
  , m_ToyTractsPath(type + "." + type)
  , m_RealFAPath(GetDataFilePath("mibrain/fa.nii.gz"))
  , m_RealTractsPath(GetDataFilePath("mibrain/base." + m_Type))
{}

void TractsTester::setUp()
{
  FibersManagerTester::setUp();
  AddAnatNode(nullptr);
}

void TractsTester::SavingLoadingToyData()
{
  mitk::FilteredFiberBundle::Pointer fibers = nullptr;
  if (m_NeedAnat)
  {
    auto anat = GetAnat(2.0);
    AddAnatNode(anat);
    fibers = Get1_10Fibers(anat->GetGeometry(), false);
  }
  else
  {
    auto geo = mitk::Geometry3D::New();
    geo->SetSpacing(2.0);
    fibers = Get1_10Fibers(geo, false);
  }
  AddFibersNode(fibers);

  mitk::IOUtil::Save(fibers, m_ToyTractsPath);

  FailIfDifferent(fibers, LoadFibers(m_ToyTractsPath));
}

void TractsTester::SavingLoadingRealData()
{
  auto fibers = LoadFibers(m_RealTractsPath);
  AddFibersNode(fibers);
  mitk::IOUtil::Save(fibers, m_ToyTractsPath);

  auto loaded = LoadFibers(m_ToyTractsPath);
  AddFibersNode(loaded);
  FailIfDifferent(fibers, loaded);
}

void TractsTester::FailIfDifferent(
  mitk::FilteredFiberBundle* fibers,
  mitk::FilteredFiberBundle* loaded) const
{
  if (!fibers->Equals(loaded, 0.00001))
  {
    const std::string msg =
      "Loaded " + m_Type + " is different from saved " + m_Type + ".";
    CPPUNIT_FAIL(msg);
  }
}
