#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <set>

#include <mitkIOUtil.h>
#include <mitkTestingMacros.h>

#include "FiberBundle/Trx.h"
#include "ImekaFiber/Saver.hpp"
#include "ImekaFiber/Testing/FibersManagerTester.hpp"
#include "ImekaFiber/utils.hpp"
#include "ImekaGeometry/MathUtils.hpp"

// Streamline groups stored in a fiber bundle (e.g. TRX groups) are shown as
// tract group nodes under the dataset node and can be hidden.
class TrxGroupsTestSuite : public FibersManagerTester
{
  CPPUNIT_TEST_SUITE(TrxGroupsTestSuite);
  MITK_TEST(TestGroupsAreChildrenOfDataset);
  MITK_TEST(TestHidingGroups);
  MITK_TEST(TestHidingDataset);
  MITK_TEST(TestReadTrxGroups);
  MITK_TEST(TestSaveWholeKeepsGroups);
  MITK_TEST(TestSaveVisibleGroups);
  MITK_TEST(TestSaveNewTractGroup);
  MITK_TEST(TestSaveTractGroup);
  MITK_TEST(TestWriteReadTrxGroups);
  MITK_TEST(TestShuffleKeepsGroups);
  MITK_TEST(TestSaveOnlyVisibleDataset);
  CPPUNIT_TEST_SUITE_END();

private:
  mitk::FilteredFiberBundle::Pointer m_Fibers;
  mitk::DataNode::Pointer m_FibersNode;

  mitk::DataNode* GetChild(const std::string& name) const
  {
    for (auto node : m_DM->DirectChildrenOf(m_FibersNode))
    {
      if (node->GetName() == name) { return node; }
    }
    return nullptr;
  }

  int NbVisible() const
  {
    return m_Fibers->GetFibersVisibility().size();
  }

  typedef std::map<std::string, std::set<vtkIdType>> Groups;

  // Streamline i of Get1_10Fibers goes from x = 0 to x = i + 1, so the groups
  // of a saved bundle can be compared to the original streamline indices,
  // whatever the order of the saved streamlines.
  static Groups OriginalGroups(const mitk::FilteredFiberBundle* fibers)
  {
    auto polyData = fibers->GetFiberPolyData();
    Groups groups;
    for (const auto& name_indexes : fibers->GetStreamlineGroups())
    {
      auto& group = groups[name_indexes.first];
      for (const auto idx : name_indexes.second)
      {
        CPPUNIT_ASSERT(idx >= 0 && idx < fibers->GetNumFibers());
        vtkIdType nbPoints;
        const vtkIdType* pts;
        polyData->GetCellPoints(idx, nbPoints, pts);
        double p[3];
        polyData->GetPoint(pts[1], p);
        group.insert(static_cast<vtkIdType>(std::lround(std::abs(p[0]))) - 1);
      }
    }
    return groups;
  }

  mitk::FilteredFiberBundle::Pointer SaveDataset() const
  {
    return Imeka::Fiber::Saver::Instance().FiberBundleFromVisibleStreamlines(
      m_FibersNode, false);
  }

public:
  void setUp() override
  {
    FibersManagerTester::setUp();

    auto geo = mitk::Geometry3D::New();
    m_Fibers = Get1_10Fibers(geo, true);
    m_Fibers->SetStreamlineGroups({
      { "A", { 0, 1, 2 } },
      { "B", { 2, 5 } }
    });
    m_FibersNode = AddFibersNode(m_Fibers, "FibersNode");
  }

  void tearDown() override
  {
    m_Fibers = nullptr;
    m_FibersNode = nullptr;
    FibersManagerTester::tearDown();
  }

  void TestGroupsAreChildrenOfDataset()
  {
    const auto children = m_DM->DirectChildrenOf(m_FibersNode);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), children.size());
    CPPUNIT_ASSERT(GetChild("A") != nullptr);
    CPPUNIT_ASSERT(GetChild("B") != nullptr);
    CPPUNIT_ASSERT(Imeka::Fiber::IsTractGroup(GetChild("A")));

    // Groups are hidden on load: the whole tractogram is displayed
    CPPUNIT_ASSERT(!GetChild("A")->IsVisible(nullptr));
    CPPUNIT_ASSERT(!GetChild("B")->IsVisible(nullptr));
    CPPUNIT_ASSERT_EQUAL(10, NbVisible());
  }

  void TestHidingGroups()
  {
    // Showing a group restricts the display to it
    GetChild("A")->SetVisibility(true);
    CPPUNIT_ASSERT_EQUAL(3, NbVisible());
    CPPUNIT_ASSERT(!m_Fibers->GetFibersVisibility().contains(5));

    // Union of the visible groups
    GetChild("B")->SetVisibility(true);
    CPPUNIT_ASSERT_EQUAL(4, NbVisible());

    GetChild("A")->SetVisibility(false);
    CPPUNIT_ASSERT_EQUAL(2, NbVisible());
    CPPUNIT_ASSERT(m_Fibers->GetFibersVisibility().contains(2));
    CPPUNIT_ASSERT(m_Fibers->GetFibersVisibility().contains(5));

    // No visible group: back to the whole tractogram
    GetChild("B")->SetVisibility(false);
    CPPUNIT_ASSERT_EQUAL(10, NbVisible());
  }

  // Hiding then showing the dataset puts the groups back in their initial
  // state: all hidden, whole tractogram displayed
  void TestHidingDataset()
  {
    GetChild("A")->SetVisibility(true);
    CPPUNIT_ASSERT_EQUAL(3, NbVisible());

    m_FibersNode->SetVisibility(false);
    CPPUNIT_ASSERT(!GetChild("A")->IsVisible(nullptr));
    CPPUNIT_ASSERT(!GetChild("B")->IsVisible(nullptr));

    m_FibersNode->SetVisibility(true);
    CPPUNIT_ASSERT(m_FibersNode->IsVisible(nullptr));
    CPPUNIT_ASSERT(!GetChild("A")->IsVisible(nullptr));
    CPPUNIT_ASSERT(!GetChild("B")->IsVisible(nullptr));
    CPPUNIT_ASSERT_EQUAL(10, NbVisible());

    // Each group must still respond normally afterwards
    GetChild("B")->SetVisibility(true);
    CPPUNIT_ASSERT_EQUAL(2, NbVisible());
    GetChild("A")->SetVisibility(true);
    CPPUNIT_ASSERT_EQUAL(4, NbVisible());
    GetChild("B")->SetVisibility(false);
    CPPUNIT_ASSERT_EQUAL(3, NbVisible());
  }

  // Needs a real TRX file with groups: set MIBRAIN_TRX_GROUPS_TEST_FILE
  void TestReadTrxGroups()
  {
    const char* path = std::getenv("MIBRAIN_TRX_GROUPS_TEST_FILE");
    if (!path) { return; }

    auto fibers = mitk::FilteredFiberBundle::New();
    CPPUNIT_ASSERT(ReadTrx(path, fibers) > 0);

    const auto& groups = fibers->GetStreamlineGroups();
    CPPUNIT_ASSERT(!groups.empty());
    for (const auto& name_indexes : groups)
    {
      std::cout << "TRX group " << name_indexes.first << ": "
                << name_indexes.second.size() << " streamlines\n";
      for (const auto idx : name_indexes.second)
      {
        CPPUNIT_ASSERT(idx >= 0 && idx < fibers->GetNumFibers());
      }
    }
  }

  // Nothing is selected: the whole tractogram is saved with all its groups
  void TestSaveWholeKeepsGroups()
  {
    const auto saved = SaveDataset();
    CPPUNIT_ASSERT_EQUAL(10, saved->GetNumFibers());
    const Groups expected = { { "A", { 0, 1, 2 } }, { "B", { 2, 5 } } };
    CPPUNIT_ASSERT(expected == OriginalGroups(saved));
  }

  // Only the selected (visible) groups are saved, as streamlines and groups
  void TestSaveVisibleGroups()
  {
    GetChild("B")->SetVisibility(true);
    auto saved = SaveDataset();
    CPPUNIT_ASSERT_EQUAL(2, saved->GetNumFibers());
    CPPUNIT_ASSERT(Groups({ { "B", { 2, 5 } } }) == OriginalGroups(saved));

    // Streamline 2 is in both groups but saved once
    GetChild("A")->SetVisibility(true);
    saved = SaveDataset();
    CPPUNIT_ASSERT_EQUAL(4, saved->GetNumFibers());
    const Groups expected = { { "A", { 0, 1, 2 } }, { "B", { 2, 5 } } };
    CPPUNIT_ASSERT(expected == OriginalGroups(saved));
  }

  // A tract group created by the user is saved as a group named after it
  void TestSaveNewTractGroup()
  {
    const auto TGNode = SetupTG(m_FibersNode,
      Vector3D(2.5, 1.501, 2.5), Point3D(0.0, -0.5, 0.0), "My TG").first;
    const auto& pmd = m_FM->GetFiberNodeData(m_FibersNode)
      .fiberMapperData.partsMapperData.at(TGNode);
    CPPUNIT_ASSERT(!pmd.GetIndices().isEmpty());
    std::set<vtkIdType> inTG;
    for (const auto idx : pmd.GetIndices()) { inTG.insert(idx); }

    const auto saved = SaveDataset();
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(inTG.size()),
                         saved->GetNumFibers());
    CPPUNIT_ASSERT(Groups({ { "My TG", inTG } }) == OriginalGroups(saved));

    // With a TRX group too
    GetChild("B")->SetVisibility(true);
    std::set<vtkIdType> all = inTG;
    all.insert({ 2, 5 });
    const auto saved2 = SaveDataset();
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(all.size()),
                         saved2->GetNumFibers());
    const Groups expected = { { "My TG", inTG }, { "B", { 2, 5 } } };
    CPPUNIT_ASSERT(expected == OriginalGroups(saved2));
  }

  // Saving a single tract group gives a bundle with one group: itself
  void TestSaveTractGroup()
  {
    GetChild("A")->SetVisibility(true);
    const auto saved = Imeka::Fiber::Saver::Instance()
      .FiberBundleFromVisibleStreamlines(m_FibersNode, GetChild("A"), false);
    CPPUNIT_ASSERT_EQUAL(3, saved->GetNumFibers());
    CPPUNIT_ASSERT(Groups({ { "A", { 0, 1, 2 } } }) == OriginalGroups(saved));
  }

  void TestWriteReadTrxGroups()
  {
    GetChild("A")->SetVisibility(true);
    GetChild("B")->SetVisibility(true);
    auto saved = SaveDataset();
    // TRX can't store dots in group names
    auto groups = saved->GetStreamlineGroups();
    groups["left.AF"] = groups["A"];
    groups.erase("A");
    saved->SetStreamlineGroups(groups);

    const auto path = mitk::IOUtil::GetTempPath() + "/TrxGroupsTest.trx";
    std::remove(path.c_str());
    mitk::IOUtil::Save(saved, path); // Goes through the TRX writer

    auto loaded = mitk::FilteredFiberBundle::New();
    CPPUNIT_ASSERT(ReadTrx(path, loaded) > 0);
    std::remove(path.c_str());

    CPPUNIT_ASSERT_EQUAL(4, loaded->GetNumFibers());
    const Groups expected = { { "left_AF", { 0, 1, 2 } }, { "B", { 2, 5 } } };
    CPPUNIT_ASSERT(expected == OriginalGroups(loaded));
  }

  // Original index of each visible streamline (see OriginalGroups)
  std::set<vtkIdType> VisibleOriginalIndexes() const
  {
    auto polyData = m_Fibers->GetFiberPolyData();
    std::set<vtkIdType> visible;
    for (const auto idx : m_Fibers->GetFibersVisibility())
    {
      vtkIdType nbPoints;
      const vtkIdType* pts;
      polyData->GetCellPoints(idx, nbPoints, pts);
      double p[3];
      polyData->GetPoint(pts[1], p);
      visible.insert(static_cast<vtkIdType>(std::lround(std::abs(p[0]))) - 1);
    }
    return visible;
  }

  // Shuffling reorders the streamlines: the groups must follow them
  void TestShuffleKeepsGroups()
  {
    const Groups expected = { { "A", { 0, 1, 2 } }, { "B", { 2, 5 } } };
    GetChild("A")->SetVisibility(true);

    // Several times, a shuffle can keep a group in place by chance
    for (int i = 0; i < 5; ++i)
    {
      m_FibersNode->SetBoolProperty("ShuffleFibers", true);
      CPPUNIT_ASSERT(expected == OriginalGroups(m_Fibers));

      // The displayed group is still the same streamlines
      CPPUNIT_ASSERT(std::set<vtkIdType>({ 0, 1, 2 }) == VisibleOriginalIndexes());

      // And so are the saved ones
      const auto saved = SaveDataset();
      CPPUNIT_ASSERT_EQUAL(3, saved->GetNumFibers());
      CPPUNIT_ASSERT(Groups({ { "A", { 0, 1, 2 } } }) == OriginalGroups(saved));
    }

    // Back to the whole tractogram
    GetChild("A")->SetVisibility(false);
    CPPUNIT_ASSERT_EQUAL(10, NbVisible());
    CPPUNIT_ASSERT(expected == OriginalGroups(SaveDataset()));
  }

  // With many datasets and only one visible, the visible one is saved
  void TestSaveOnlyVisibleDataset()
  {
    auto geo = mitk::Geometry3D::New();
    auto other = Get1_10Fibers(geo, true);
    auto otherNode = AddFibersNode(other, "OtherNode");
    auto& saver = Imeka::Fiber::Saver::Instance();

    // Whatever the order of the datasets in the saver
    otherNode->SetVisibility(false);
    auto saved = saver.FiberBundleFromVisibleStreamlines();
    CPPUNIT_ASSERT_EQUAL(10, saved->GetNumFibers());
    const Groups expected = { { "A", { 0, 1, 2 } }, { "B", { 2, 5 } } };
    CPPUNIT_ASSERT(expected == OriginalGroups(saved));

    otherNode->SetVisibility(true);
    m_FibersNode->SetVisibility(false);
    saved = saver.FiberBundleFromVisibleStreamlines();
    CPPUNIT_ASSERT_EQUAL(10, saved->GetNumFibers());
    CPPUNIT_ASSERT(saved->GetStreamlineGroups().empty());
  }
};

MITK_TEST_SUITE_REGISTRATION(TrxGroups)
