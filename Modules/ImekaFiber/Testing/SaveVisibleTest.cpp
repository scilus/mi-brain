#include <mitkTestingMacros.h>

#include <vtkCellData.h>

#include "ImekaCommon/Predicate.hpp"
#include "ImekaGeometry/MathUtils.hpp"
#include "ImekaFiber/Testing/FibersManagerTester.hpp"
#include "ImekaFiber/Saver.hpp"

class SaveVisibleTestSuite : public FibersManagerTester
{
  CPPUNIT_TEST_SUITE(SaveVisibleTestSuite);

  MITK_TEST(SaveProperties1);
  MITK_TEST(SaveProperties13);
  MITK_TEST(SaveAllTracts);
  MITK_TEST(SaveTracts);
  MITK_TEST(SaveTractsDM);
  MITK_TEST(SaveTG1);
  MITK_TEST(SaveTG2);
  MITK_TEST(Vis_SaveAllTracts);
  MITK_TEST(VisTG_SaveTracts);
  MITK_TEST(Vis_SaveTracts);
  MITK_TEST(Vis_SaveTG2);
  CPPUNIT_TEST_SUITE_END();

private:
  void HideActiveROI(mitk::BoundingObject* SO)
  {
    // Should change the ActiveROI name too
    m_DM->GetNodeContainingThis(SO)->SetName("SO1");
    auto activeROI =
      m_DM->GetFirst(Imeka::Predicate::Property("name", "SO1"), m_TG1);
    activeROI->SetVisibility(false);
  }

  void CheckProperties(const unsigned int nb, const float minPossible)
  {
    auto loaded = LoadFibers(m_SavePath);
    vtkSmartPointer<vtkFloatArray> arr = vtkFloatArray::SafeDownCast(
      loaded->GetFiberPolyData()->GetCellData()->GetAbstractArray("floats"));
    CPPUNIT_ASSERT(arr->GetSize() == nb);
    for (unsigned int i = 0; i < nb; ++i)
    {
      CPPUNIT_ASSERT(arr->GetValue(i) >= minPossible);
    }
  }

  void CheckTupleProperties(const unsigned int nb, const float minPossible)
  {
    auto loaded = LoadFibers(m_SavePath);
    vtkSmartPointer<vtkFloatArray> arr = vtkFloatArray::SafeDownCast(
      loaded->GetFiberPolyData()->GetCellData()->GetAbstractArray("floats3"));
    float values[3];
    CPPUNIT_ASSERT(arr->GetSize() / arr->GetNumberOfComponents() == nb);
    for (unsigned int i = 0; i < nb; ++i)
    {
      arr->GetTypedTuple(i, values);
      CPPUNIT_ASSERT(values[0] >= minPossible && values[1] >= minPossible && values[2] >= minPossible);
    }
  }

  mitk::DataNode* m_FibersNode;
  mitk::DataNode *m_TG1, *m_TG2;
  mitk::BoundingObject *m_SO1, *m_SO2;

  const std::string m_SavePath = "saved.trk";
  const int m_NbFibersTG1 = 74;
  const int m_NbFibersTG2 = 56;
  const int m_nbFibersTotal = 123;

public:
  void setUp() override
  {
    FibersManagerTester::setUp();

    AddAnatNode(LoadImage(GetDataFilePath("mibrain/fa.nii.gz")));
    m_FibersNode = AddFibersNode(
      LoadFibers(GetDataFilePath("mibrain/base.trk")));

    const auto size = Vector3D(4.5, 4.5, 4.5);
    std::tie(m_TG1, m_SO1) = SetupTG(
      m_FibersNode, size, Point3D(21.3286, 3.9594, 2.5752), "TG1");
    std::tie(m_TG2, m_SO2) = SetupTG(
      m_FibersNode, size, Point3D(-20.4822, 3.4373, 1.4572), "TG2");

    Imeka::Fiber::Saver::Instance().SetSavePath(m_SavePath);
  }

  void tearDown() override
  {
    m_FibersNode = nullptr;
    m_SO1 = m_SO2 = nullptr;
    FibersManagerTester::tearDown();
  }

  void SaveProperties1()
  {
    // Add properties
    auto floats = vtkSmartPointer<vtkFloatArray>::New();
    floats->SetName("floats");
    for (unsigned int i = 0; i < 313; ++i)
    {
      floats->InsertNextValue(i * 0.01);
    }
    auto cellData = dynamic_cast<mitk::FilteredFiberBundle*>(
      m_FibersNode->GetData())->GetFiberPolyData()->GetCellData();
    cellData->AddArray(floats);
    const float minPossibleValue = 19 * 0.01 - 0.0001;
    const float minPossibleValueTG1 = 122 * 0.01 - 0.0001;

    // Save all visible, save tracts.
    m_Groups->Tracts->SetStringProperty("Save", "file");
    CheckProperties(m_nbFibersTotal, minPossibleValue);
    Imeka::Fiber::Saver::Instance().SetSavePath(m_SavePath);
    m_FibersNode->SetStringProperty("Save", "file");
    CheckProperties(m_nbFibersTotal, minPossibleValue);

    // Save TG1.
    Imeka::Fiber::Saver::Instance().SetSavePath(m_SavePath);
    m_TG1->SetStringProperty("Save", "file");
    CheckProperties(m_NbFibersTG1, minPossibleValueTG1);

    // Save TG2.
    Imeka::Fiber::Saver::Instance().SetSavePath(m_SavePath);
    m_TG2->SetStringProperty("Save", "file");
    CheckProperties(m_NbFibersTG2, minPossibleValue);
  }

  void SaveProperties13()
  {
    // Add properties
    auto floats1 = vtkSmartPointer<vtkFloatArray>::New();
    floats1->SetName("floats");
    for (unsigned int i = 0; i < 313; ++i)
    {
      floats1->InsertNextValue(i * 0.01);
    }
    auto floats3 = vtkSmartPointer<vtkFloatArray>::New();
    floats3->SetName("floats3");
    floats3->SetNumberOfComponents(3);
    for (unsigned int i = 0; i < 313; ++i)
    {
      const float f = static_cast<float>(i) * 0.01;
      float tuple[3] = { f, f, f };
      floats3->InsertNextTypedTuple(tuple);
    }

    auto cellData = dynamic_cast<mitk::FilteredFiberBundle*>(
      m_FibersNode->GetData())->GetFiberPolyData()->GetCellData();
    cellData->AddArray(floats1);
    cellData->AddArray(floats3);
    const float minPossibleValue = 19 * 0.01 - 0.0001;
    const float minPossibleValueTG1 = 122 * 0.01 - 0.0001;

    // Save all visible.
    m_Groups->Tracts->SetStringProperty("Save", "file");
    CheckProperties(m_nbFibersTotal, minPossibleValue);
    CheckTupleProperties(m_nbFibersTotal, minPossibleValue);

    // Save TG1.
    Imeka::Fiber::Saver::Instance().SetSavePath(m_SavePath);
    m_TG1->SetStringProperty("Save", "file");
    CheckProperties(m_NbFibersTG1, minPossibleValueTG1);
    CheckTupleProperties(m_NbFibersTG1, minPossibleValueTG1);

    // Save TG2.
    Imeka::Fiber::Saver::Instance().SetSavePath(m_SavePath);
    m_TG2->SetStringProperty("Save", "file");
    CheckProperties(m_NbFibersTG2, minPossibleValue);
    CheckTupleProperties(m_NbFibersTG2, minPossibleValue);
  }

  void SaveAllTracts()
  {
    m_Groups->Tracts->SetStringProperty("Save", "file");
    auto loaded = LoadFibers(m_SavePath);
    const int nbFibers = loaded->GetNumFibers();
    CPPUNIT_ASSERT(abs(nbFibers - m_nbFibersTotal) <= 1);
  }

  void SaveTracts()
  {
    m_FibersNode->SetStringProperty("Save", "file");
    auto loaded = LoadFibers(m_SavePath);
    const int nbFibers = loaded->GetNumFibers();
    CPPUNIT_ASSERT(abs(nbFibers - m_nbFibersTotal) <= 1);
  }

  void SaveTractsDM()
  {
    m_FibersNode->SetStringProperty("Save", "dm");
    auto addedNode = m_DM->GetFirst(
      Imeka::Predicate::Property("name", "Fiber bundle"));
    auto addedFibers =
      dynamic_cast<mitk::FilteredFiberBundle*>(addedNode->GetData());
    const int nbFibers = addedFibers->GetNumFibers();
    CPPUNIT_ASSERT(abs(nbFibers - m_nbFibersTotal) <= 1);
  }

  void SaveTG1()
  {
    m_TG1->SetStringProperty("Save", "file");
    auto loaded = LoadFibers(m_SavePath);
    const int nbFibers = loaded->GetNumFibers();
    CPPUNIT_ASSERT(abs(nbFibers - m_NbFibersTG1) <= 1);
  }

  void SaveTG2()
  {
    m_TG2->SetStringProperty("Save", "file");
    auto loaded = LoadFibers(m_SavePath);
    const int nbFibers = loaded->GetNumFibers();
    CPPUNIT_ASSERT(abs(nbFibers - m_NbFibersTG2) <= 1);
  }

  void Vis_SaveAllTracts()
  {
    HideActiveROI(m_SO1);
    CPPUNIT_ASSERT(m_TG1->IsVisible(nullptr) == false);

    m_Groups->Tracts->SetStringProperty("Save", "file");
    auto loaded = LoadFibers(m_SavePath);
    const int nbFibers = loaded->GetNumFibers();
    CPPUNIT_ASSERT(abs(nbFibers - m_NbFibersTG2) <= 1);
  }

  void VisTG_SaveTracts()
  {
    m_TG1->SetVisibility(false);
    auto activeROI1 = m_DM->GetFirst(nullptr, m_TG1);
    CPPUNIT_ASSERT(activeROI1->IsVisible(nullptr) == false);

    m_FibersNode->SetStringProperty("Save", "file");
    auto loaded = LoadFibers(m_SavePath);
    const int nbFibers = loaded->GetNumFibers();
    CPPUNIT_ASSERT(abs(nbFibers - m_NbFibersTG2) <= 1);
  }

  void Vis_SaveTracts()
  {
    HideActiveROI(m_SO1);
    m_FibersNode->SetStringProperty("Save", "file");
    auto loaded = LoadFibers(m_SavePath);
    const int nbFibers = loaded->GetNumFibers();
    CPPUNIT_ASSERT(abs(nbFibers - m_NbFibersTG2) <= 1);
  }

  void Vis_SaveTG2()
  {
    HideActiveROI(m_SO1);
    m_TG2->SetStringProperty("Save", "file");
    auto loaded = LoadFibers(m_SavePath);
    const int nbFibers = loaded->GetNumFibers();
    CPPUNIT_ASSERT(abs(nbFibers - m_NbFibersTG2) <= 1);
  }
};

MITK_TEST_SUITE_REGISTRATION(SaveVisible)
