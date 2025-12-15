#include <mitkLookupTableProperty.h>
#include <mitkTestingMacros.h>

#include "ImekaCommon/Colors.hpp"
#include "ImekaFiber/Testing/FibersManagerTester.hpp"
#include "ImekaFiber/Saver.hpp"
#include "ImekaGeometry/MathUtils.hpp"

const QColor OLIVE(180, 180, 0);
const QColor ORANGE(255, 100, 0);

class ColoringTestSuite : public FibersManagerTester
{
  CPPUNIT_TEST_SUITE(ColoringTestSuite);

  MITK_TEST(TestLocalDefault);
  MITK_TEST(TestCategoryLocal);
  MITK_TEST(TestDatasetLocal);
  MITK_TEST(TestCategoryUniform);
  MITK_TEST(TestDatasetUniform);
  MITK_TEST(TestCategoryEndPoint);
  MITK_TEST(TestDatasetEndPoint);
  MITK_TEST(TestCategoryFromAnat);
  MITK_TEST(TestDatasetFromAnat);
  MITK_TEST(TestFromAnatAutoUpdateAnat);
  MITK_TEST(TestFromAnatAutoUpdateLUT);
  MITK_TEST(TestCategoryShuffle);
  MITK_TEST(TestColorAfterSave);
  MITK_TEST(TestUniformColorGuess);
  CPPUNIT_TEST_SUITE_END();

private:
  typedef mitk::FilteredFiberBundle::Coloring Coloring;
  mitk::DataNode::Pointer m_Node1, m_Node2;

  std::vector<QColor> GetGrayScaleLUTColors()
  {
    return{
      { 0, 0, 127 },   { 0, 0, 187 },    { 0, 0, 247 },
      { 0, 0, 187 },   { 0, 55, 255 },   { 0, 175, 255 },
      { 0, 0, 187 },   { 0, 115, 255 },  { 39, 255, 215 },
      { 0, 0, 247 },   { 0, 235, 255 },  { 223, 255, 32 },
      { 0, 0, 247 },   { 39, 255, 215 }, { 255, 167, 0 },
      { 0, 55, 255 },  { 163, 255, 92 }, { 243, 0, 0 },
      { 0, 55, 255 },  { 223, 255, 32 }, { 0, 0, 0 },
      { 0, 115, 255 }, { 255, 167, 0 },  { 0, 0, 0 },
      { 0, 115, 255 }, { 255, 108, 0 },  { 0, 0, 0 },
      { 0, 175, 255 }, { 0, 0, 0 },      { 0, 0, 0 }
    };
  }

public:
  void setUp() override
  {
    FibersManagerTester::setUp();

    auto geo = mitk::Geometry3D::New();
    m_Node1 = AddFibersNode(Get1_10Fibers(geo, true), "FibersNode1");
    m_Node2 = AddFibersNode(Get1_10Fibers(geo, true), "FibersNode2");
  }

  void tearDown() override
  {
    m_Node1 = m_Node2 = nullptr;
    FibersManagerTester::tearDown();
  }

  void TestLocalDefault()
  {
    const std::vector<QColor> expectedColors = { Qt::red, OLIVE, Qt::green };
    AssertColorAre(m_Node1, expectedColors);
    AssertColorAre(m_Node2, expectedColors);
  }

  void TestCategoryLocal()
  {
    m_Groups->Tracts->SetIntProperty("ColorType", Coloring::Orientation);
    TestLocalDefault();

    m_Groups->Tracts->SetIntProperty("ColorType", Coloring::EndPoint);
    m_Groups->Tracts->SetIntProperty("ColorType", Coloring::Orientation);
    TestLocalDefault();
  }

  void TestDatasetLocal()
  {
    m_Node1->SetIntProperty("ColorType", Coloring::Orientation);
    TestLocalDefault();

    m_Node1->SetIntProperty("ColorType", Coloring::EndPoint);
    m_Node1->SetIntProperty("ColorType", Coloring::Orientation);
    TestLocalDefault();
  }

  void TestCategoryUniform()
  {
    Imeka::Color::Apply(m_Groups->Tracts, ORANGE);
    AssertColorAre(m_Node1, { ORANGE, ORANGE, ORANGE });
    AssertColorAre(m_Node2, { ORANGE, ORANGE, ORANGE });
  }

  void TestDatasetUniform()
  {
    Imeka::Color::Apply(m_Node1, ORANGE);
    AssertColorAre(m_Node1, { ORANGE, ORANGE, ORANGE });
    AssertColorAre(m_Node2, { Qt::red, OLIVE, Qt::green });
  }

  void TestCategoryEndPoint()
  {
    m_Groups->Tracts->SetIntProperty("ColorType", Coloring::EndPoint);
    AssertColorAre(m_Node1, { OLIVE, OLIVE, OLIVE });
    AssertColorAre(m_Node2, { OLIVE, OLIVE, OLIVE });
  }

  void TestDatasetEndPoint()
  {
    m_Node1->SetIntProperty("ColorType", Coloring::EndPoint);
    AssertColorAre(m_Node1, { OLIVE, OLIVE, OLIVE });
    AssertColorAre(m_Node2, { Qt::red, OLIVE, Qt::green });
  }

  void TestCategoryFromAnat()
  {
    AddAnatNode(GetAnat());
    m_Groups->Tracts->SetIntProperty("ColorType", Coloring::FromAnat);

    AssertColorIs(m_Node1, GetGrayScaleLUTColors());
    AssertColorIs(m_Node2, GetGrayScaleLUTColors());
  }

  void TestDatasetFromAnat()
  {
    AddAnatNode(GetAnat());
    m_Node1->SetIntProperty("ColorType", Coloring::FromAnat);

    AssertColorIs(m_Node1, GetGrayScaleLUTColors());
    AssertColorAre(m_Node2, { Qt::red, OLIVE, Qt::green });
  }

  void TestFromAnatAutoUpdateAnat()
  {
    AddAnatNode(GetAnat());
    m_Groups->Tracts->SetIntProperty("ColorType", Coloring::FromAnat);

    AddAnatNode(GetAnat(1.0, 0.0, true));

    const std::vector<QColor> hotIronLUTColors{
      { 127, 0, 0 },   { 127, 0, 0 },    { 183, 0, 0 },
      { 127, 0, 0 },   { 243, 0, 0 },    { 255, 108, 0 },
      { 127, 0, 0 },   { 255, 48, 0 },   { 255, 227, 0 },
      { 183, 0, 0 },   { 255, 167, 0 },  { 99, 255, 155 },
      { 183, 0, 0 },   { 255, 227, 0 },  { 0, 235, 255 },
      { 243, 0, 0 },   { 163, 255, 92 }, { 0, 55, 255 },
      { 243, 0, 0 },   { 99, 255, 155 }, { 0, 0, 0 },
      { 255, 48, 0 },  { 0, 235, 255 },  { 0, 0, 0 },
      { 255, 48, 0 },  { 0, 175, 255 },  { 0, 0, 0 },
      { 255, 108, 0 }, { 0, 0, 0 },      { 0, 0, 0 }
    };
    AssertColorIs(m_Node1, hotIronLUTColors);
    AssertColorIs(m_Node2, hotIronLUTColors);
  }

  void TestFromAnatAutoUpdateLUT()
  {
    auto anatNode = AddAnatNode(GetAnat());
    m_Groups->Tracts->SetIntProperty("ColorType", Coloring::FromAnat);

    auto mitkLut = mitk::LookupTable::New();
    mitkLut->SetType(mitk::LookupTable::HOT_IRON);
    auto lutProperty = mitk::LookupTableProperty::New();
    lutProperty->SetLookupTable(mitkLut);
    anatNode->SetProperty("LookupTable", lutProperty);

    const std::vector<QColor> hotIronLUTColors{
      { 0, 0, 0 },   { 30, 0, 0 },     { 60, 0, 0 },
      { 30, 0, 0 },  { 90, 0, 0 },     { 150, 0, 0 },
      { 30, 0, 0 },  { 120, 0, 0 },    { 210, 0, 0 },
      { 60, 0, 0 },  { 180, 0, 0 },    { 255, 46, 0 },
      { 60, 0, 0 },  { 210, 0, 0 },    { 255, 106, 0 },
      { 90, 0, 0 },  { 255, 16, 0 },   { 255, 196, 136 },
      { 90, 0, 0 },  { 255, 46, 0 },   { 0, 0, 0 },
      { 120, 0, 0 }, { 255, 106, 0 },  { 0, 0, 0 },
      { 120, 0, 0 }, { 255, 136, 20 }, { 0, 0, 0 },
      { 150, 0, 0 }, { 0, 0, 0 },      { 0, 0, 0 }
    };
    AssertColorIs(m_Node1, hotIronLUTColors);
    AssertColorIs(m_Node2, hotIronLUTColors);
  }

  void TestCategoryShuffle()
  {
    // Shuffled colors are not exactly random; we always use the same seed
    // so we know which colors we will get.
    const QColor firstColor(2, 75, 252);
    const QColor secondColor(148, 252, 2);
    m_Groups->Tracts->SetIntProperty("ShuffleColor", 1);
    AssertColorAre(m_Node1, { firstColor, firstColor, firstColor });
    AssertColorAre(m_Node2, { secondColor, secondColor, secondColor });
  }

   // Even if it's possible to do a Shuffle on tracts, we can't test it because
   // the code is in ColorShuffleAction and it simply calls Imeka::Color::Apply
   // so it would be the same test as TestDatasetUniform.

  void TestColorAfterSave()
  {
    AddAnatNode(GetAnat());

    Imeka::Color::Apply(m_Node1, ORANGE);

    const std::string savePath = "saved.trk";
    Imeka::Fiber::Saver::Instance().SetSavePath(savePath);
    m_Node1->SetStringProperty("Save", "file");
    auto loaded = LoadFibers(savePath);

    auto node = mitk::DataNode::New();
    node->SetData(loaded);
    node->SetName("x");
    AssertColorAre(node, { ORANGE, ORANGE, ORANGE });
    // Last bool (uniform) is false because it's loaded data and we don't
    // actually know it's an uniform color (we don't check on loading).
  }

  void TestUniformColorGuess()
  {
    // Change color on fibers nodes
    using namespace Imeka::Fiber;
    const QColor white(255, 255, 255);
    AssertColorIs(white, GetInitialColor(m_Groups->Tracts));
    AssertColorIs(white, GetInitialColor(m_Node1));

    Imeka::Color::Apply(m_Node1, Qt::red);
    AssertColorIs(white, InitialColorForTC(m_Groups->Tracts));
    AssertColorIs(Qt::red, GetInitialColor(m_Node1));

    Imeka::Color::Apply(m_Node2, Qt::red);
    AssertColorIs(Qt::red, InitialColorForTC(m_Groups->Tracts));
    AssertColorIs(Qt::red, GetInitialColor(m_Node2));

    // Change color onn Tracts Category
    Imeka::Color::Apply(m_Groups->Tracts, Qt::blue);
    AssertColorIs(Qt::blue, InitialColorForTC(m_Groups->Tracts));
    AssertColorIs(Qt::blue, GetInitialColor(m_Node1));
    AssertColorIs(Qt::blue, GetInitialColor(m_Node2));

    // Change color on Tracts Group
    const mitk::Vector3D spacing(1.0);
    const mitk::Point3D origin(0.0);
    auto TGNode1 = SetupTG(m_Node1, spacing, origin).first;
    AssertColorIs(Qt::blue, GetInitialColor(m_Node1, TGNode1));
  }

  QColor InitialColorForTC(const mitk::DataNode* node)
  {
    return Imeka::Fiber::GetSingleInitialColor(
      Imeka::DataManager(m_DS).DirectChildrenOf(node));
  }
};

MITK_TEST_SUITE_REGISTRATION(Coloring)
