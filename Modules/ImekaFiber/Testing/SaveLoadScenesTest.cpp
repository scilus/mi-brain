#include <mitkSceneIO.h>
#include <mitkTestingMacros.h>

#include "ImekaCommon/Predicate.hpp"
#include "ImekaFiber/Testing/FibersManagerTester.hpp"
#include "ImekaGeometry/MathUtils.hpp"

class SaveLoadScenesTestSuite : public FibersManagerTester
{
  CPPUNIT_TEST_SUITE(SaveLoadScenesTestSuite);

  MITK_TEST(_1TG_1SO);
  MITK_TEST(_1TG_1SO_2USO);
  MITK_TEST(_1TG_2SO);
  MITK_TEST(_2TG_1SO_each);
  MITK_TEST(_2TG_2SO_each);
  CPPUNIT_TEST_SUITE_END();

private:
  void SaveClearLoadTest(
    const std::string& scenePath,
    std::vector<mitk::DataNode*> fibersNodes)
  {
    // Keep node of some details in the scene
    const unsigned int nbNodes = m_DS->GetAll()->Size();
    std::map<std::string, int> nbStreamlinesPerBundle;
    for (auto fibersNode : fibersNodes)
    {
      std::string UUID = "";
      fibersNode->GetStringProperty(m_DM->UUIDPropertyName, UUID);
      mitk::FilteredFiberBundle* bundle =
        dynamic_cast<mitk::FilteredFiberBundle*>(fibersNode->GetData());
      nbStreamlinesPerBundle[UUID] = bundle->GetFibersVisibility().size();
    }

    auto sceneIO = mitk::SceneIO::New();
    CPPUNIT_ASSERT(!m_FM->GetFilteringUI().HasLoadedScene());
    CPPUNIT_ASSERT(sceneIO->SaveScene(m_DS->GetAll(), m_DS, scenePath));
    CPPUNIT_ASSERT(!m_FM->GetFilteringUI().HasLoadedScene());

    /* Clear scene. Start by manually removing some nodes because it crashes
       otherwise. Probably because the behavior is not exactly the same in the
       tests environment. */
    for (auto fibersNode : fibersNodes)
    {
      m_DM->RemoveNode(fibersNode);
    }
    m_Groups->Anatomies = nullptr;
    m_Groups->ROIs = nullptr;
    m_Groups->Tracts = nullptr;

    CPPUNIT_ASSERT(!m_FM->GetFilteringUI().HasLoadedScene());
    sceneIO->LoadScene(scenePath, m_DS, true);
    CPPUNIT_ASSERT(m_FM->GetFilteringUI().HasLoadedScene());

    // Check if the notes we took are still true
    CPPUNIT_ASSERT_EQUAL(nbNodes, m_DS->GetAll()->Size());
    for (auto p : nbStreamlinesPerBundle)
    {
      const auto UUID = p.first;
      const auto nbStreamlines = p.second;

      const auto loadedNode = m_DM->GetFirst(
        Imeka::Predicate::Property(m_DM->UUIDPropertyName, UUID));
      mitk::FilteredFiberBundle* loadedData =
        dynamic_cast<mitk::FilteredFiberBundle*>(loadedNode->GetData());

      CPPUNIT_ASSERT_EQUAL(
        nbStreamlines, loadedData->GetFibersVisibility().size());
    }
  }

public:
  void _1TG_1SO()
  {
    auto anat = GetAnat();
    AddAnatNode(anat);
    auto fibersNode = AddFibersNode(
      Get1_10Fibers(anat->GetGeometry(), false));
    SetupTG(
      fibersNode,
      Vector3D(2.5, 2.5, 2.5),
      Point3D(0.0, 0.0, 0.0));

    SaveClearLoadTest("1TG_1SO.mitk", { fibersNode });
  }

  void _1TG_1SO_2USO()
  {
    auto anat = GetAnat();
    AddAnatNode(anat);
    auto fibersNode = AddFibersNode(
      Get1_10Fibers(anat->GetGeometry(), false));

    const auto p1 = CreateSO(
      Vector3D(2.5, 2.5, 2.5),
      Point3D(0.0, 0.0, 0.0));
    CreateSO(
      Vector3D(2.5, 2.5, 2.5),
      Point3D(0.0, 0.0, 0.0));
    CreateTG(fibersNode, { p1.first }, "TG1");
    CreateSO(
      Vector3D(2.5, 2.5, 2.5),
      Point3D(0.0, 0.0, 0.0));

    SaveClearLoadTest("1TG_1SO_2USO.mitk", { fibersNode });
  }

  void _1TG_2SO()
  {
    auto anat = GetAnat();
    AddAnatNode(anat);
    auto fibersNode = AddFibersNode(
      Get1_10Fibers(anat->GetGeometry(), false));

    const auto p1 = CreateSO(
      Vector3D(2.5, 2.5, 2.5),
      Point3D(1.0, 1.0, 0.0));
    const auto p2 = CreateSO(
      Vector3D(2.5, 2.5, 2.5),
      Point3D(0.0, 0.0, 0.0));
    CreateTG(fibersNode, Nodes{ p1.first, p2.first }, "TG1");

    SaveClearLoadTest("1TG_2SO.mitk", { fibersNode });
  }

  void _2TG_1SO_each()
  {
    auto anat = GetAnat();
    AddAnatNode(anat);
    auto fibersNode = AddFibersNode(
      Get1_10Fibers(anat->GetGeometry(), false));

    SetupTG(
      fibersNode,
      Vector3D(2.5, 2.5, 2.5),
      Point3D(0.0, 0.0, 0.0),
      "TG1");
    SetupTG(
      fibersNode,
      Vector3D(2.5, 2.5, 2.5),
      Point3D(5.0, 2.0, 0.0),
      "TG2");

    SaveClearLoadTest("2TG_1SO_each.mitk", { fibersNode });
  }

  void _2TG_2SO_each()
  {
    auto anat = GetAnat();
    AddAnatNode(anat);
    auto fibersNode = AddFibersNode(
      Get1_10Fibers(anat->GetGeometry(), false));

    {
      const auto p1 = CreateSO(
        Vector3D(2.5, 2.5, 2.5),
        Point3D(1.0, 1.0, 0.0));
      const auto p2 = CreateSO(
        Vector3D(2.5, 2.5, 2.5),
        Point3D(0.0, 0.0, 0.0));
      CreateTG(fibersNode, Nodes{ p1.first, p2.first }, "TG1");
    }

    {
      const auto p1 = CreateSO(
        Vector3D(2.5, 2.5, 2.5),
        Point3D(1.0, 1.0, 5.0));
      const auto p2 = CreateSO(
        Vector3D(2.5, 2.5, 2.5),
        Point3D(0.0, 0.0, 5.0));
      CreateTG(fibersNode, Nodes{ p1.first, p2.first }, "TG2");
    }

    SaveClearLoadTest("2TG_2SO_each.mitk", { fibersNode });
  }
};

MITK_TEST_SUITE_REGISTRATION(SaveLoadScenes)
