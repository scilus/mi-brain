#include <mitkTestingMacros.h>

#include "ImekaCommon/Predicate.hpp"
#include "ImekaFiber/Filtering/ActiveROIsDialogBox.hpp"
#include "ImekaFiber/Testing/FibersManagerTester.hpp"
#include "ImekaFiber/Surfaces.hpp"
#include "ImekaGeometry/MathUtils.hpp"
#include "ImekaGeometry/MitkImageUtils.hpp"
#include "ImekaGeometry/VtkImageUtils.hpp"

class SimpleFilteringTestSuite : public FibersManagerTester
{
  CPPUNIT_TEST_SUITE(SimpleFilteringTestSuite);

  MITK_TEST(TestSO);
  MITK_TEST(TestMaskAndAutoUpdate);
  MITK_TEST(TestSurfaceAndAutoUpdate);
  MITK_TEST(TestROIOrdering);
  CPPUNIT_TEST_SUITE_END();

private:
  typedef itk::Image<unsigned char, 3> MaskType;

  std::pair<MaskType::Pointer, mitk::Image::Pointer> GetItkMitkImages()
  {
    const MaskType::SizeType size = { { 10, 10, 10 } };
    auto itkImage = Imeka::Geometry::CreateItkImage<MaskType>(size);
    itkImage->FillBuffer(0);

    // Create a mitk::Image to use it as a mask
    auto geo = mitk::Geometry3D::New();
    auto mitkImage = Imeka::Geometry::MitkImageFromItkImage(
      itkImage, geo->GetSpacing(), geo->GetOrigin(), false);

    return std::make_pair(itkImage, mitkImage);
  }

  mitk::FilteredFiberBundle::Pointer m_Fibers;
  mitk::DataNode::Pointer m_FibersNode;

public:
  void setUp() override
  {
    FibersManagerTester::setUp();

    auto geo = mitk::Geometry3D::New();
    m_Fibers = Get1_10Fibers(geo, false);
    m_FibersNode = AddFibersNode(m_Fibers);
  }

  void tearDown() override
  {
    m_Fibers = nullptr;
    m_FibersNode = nullptr;
    FibersManagerTester::tearDown();
  }

  // Toy test with a SO. We do test with real data in other files.
  void TestSO()
  {
    CPPUNIT_ASSERT_EQUAL(10, static_cast<int>(m_Fibers->GetFibersVisibility().size()));
    const auto TGNode_SO_pair = SetupTG(
      m_FibersNode,
      Vector3D(2.5, 1.501, 2.5),
      Point3D(0.0, -2.0, 0.0));
    auto TGNode = TGNode_SO_pair.first;
    auto SO = TGNode_SO_pair.second;
    auto geo = SO->GetGeometry();
    CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(m_Fibers->GetFibersVisibility().size()));

    const auto& mapperData =
      m_FM->GetFiberNodeData(m_FibersNode).fiberMapperData;
    CPPUNIT_ASSERT_EQUAL(false, mapperData.useWhole);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), mapperData.GetIBO().size());
    const auto& pmd = mapperData.partsMapperData.at(TGNode);

    // m_Fibers->GetFibersVisibility() is NOT updated during moving, but the
    // inner results (PartMapperData) are.
    unsigned int lastNbVisibility = 0;
    for (unsigned int nbStreamlines :
      { 0, 1, 2, 3, 4, 5, 6, 7, 7, 7, 7, 6, 5, 4, 3, 2, 1, 0, 0, 0 })
    {
      if (nbStreamlines % 2 == 0)
      {
        m_FM->SelectionObjectReleased(SO);
        CPPUNIT_ASSERT_EQUAL(
          nbStreamlines,
          static_cast<unsigned int>(m_Fibers->GetFibersVisibility().size()));
        lastNbVisibility = nbStreamlines;
      }
      else
      {
        CPPUNIT_ASSERT_EQUAL(
          lastNbVisibility,
          static_cast<unsigned int>(m_Fibers->GetFibersVisibility().size()));
      }
      CPPUNIT_ASSERT_EQUAL(
        static_cast<int>(nbStreamlines), static_cast<int>(pmd.GetIndices().size()));
      CPPUNIT_ASSERT_EQUAL(2u * nbStreamlines, pmd.NbPoints());
      CPPUNIT_ASSERT_EQUAL(
        static_cast<size_t>(2 * nbStreamlines), mapperData.GetIBO().size());

      auto origin = geo->GetOrigin();
      origin[1] += 0.5;
      MoveSO(SO, origin);
    }
  }

  void TestMaskAndAutoUpdate()
  {
    auto tuple = GetItkMitkImages();
    auto& itkImage = std::get<0>(tuple);
    auto& mitkImage = std::get<1>(tuple);

    // Should light up 2 streamlines
    itkImage->SetPixel({ { 0, 0, 0 } }, 1);

    CreateTG(m_FibersNode, { AddAnatNode(mitkImage) }, "TGNode");

    const auto& mapperData =
      m_FM->GetFiberNodeData(m_FibersNode).fiberMapperData;
    CPPUNIT_ASSERT_EQUAL(false, mapperData.useWhole);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), mapperData.GetIBO().size());
    CPPUNIT_ASSERT_EQUAL(
      2u, static_cast<unsigned int>(m_Fibers->GetFibersVisibility().size()));

    /* Modified() is directly called by the Segmentation plugin when we modify
       an image, but we need to call it ourself here. */
    itkImage->SetPixel({ { 9, 5, 0 } }, 1); // Should add 1 streamline
    mitkImage->Modified();

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), mapperData.GetIBO().size());
    CPPUNIT_ASSERT_EQUAL(
      3u, static_cast<unsigned int>(m_Fibers->GetFibersVisibility().size()));
  }

  void TestSurfaceAndAutoUpdate()
  {
    auto tuple = GetItkMitkImages();
    auto& itkImage = std::get<0>(tuple);
    auto& mitkImage = std::get<1>(tuple);

    // Should light up 3 streamlines
    itkImage->SetPixel({ { 1, 1, 0 } }, 1);

    mitk::DataNode::Pointer anatNode = AddAnatNode(mitkImage);

    auto surfaceNode = mitk::DataNode::New();
    surfaceNode->SetName("surface");
    surfaceNode->SetData(Imeka::Surface::ImageToSurface(mitkImage, mitkImage));
    m_DM->AddNode(surfaceNode, anatNode);

    CreateTG(m_FibersNode, { surfaceNode }, "TGNode");

    const auto& mapperData =
      m_FM->GetFiberNodeData(m_FibersNode).fiberMapperData;
    CPPUNIT_ASSERT_EQUAL(false, mapperData.useWhole);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), mapperData.GetIBO().size());
    CPPUNIT_ASSERT_EQUAL(
      3u, static_cast<unsigned int>(m_Fibers->GetFibersVisibility().size()));

    /* Modified() is directly called by the Segmentation plugin when we modify
    an image, but we need to call it ourself here. */
    itkImage->SetPixel({ { 9, 5, 0 } }, 1); // Should add 2 streamline
    mitkImage->Modified();

    auto newNode = mitk::DataNode::New();
    newNode->SetName("surface");
    newNode->SetData(Imeka::Surface::ImageToSurface(mitkImage, mitkImage));
    m_DM->AddNode(newNode, anatNode);

    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(10), mapperData.GetIBO().size());
    CPPUNIT_ASSERT_EQUAL(
      5u, static_cast<unsigned int>(m_Fibers->GetFibersVisibility().size()));
  }

  void TestROIOrdering()
  {
    auto tuple = GetItkMitkImages();
    auto& itkImage = std::get<0>(tuple);
    auto& mitkImage = std::get<1>(tuple);
    itkImage->SetPixel({ { 1, 1, 0 } }, 1);

    auto surfaceNode = mitk::DataNode::New();
    surfaceNode->SetName("surface");
    surfaceNode->SetData(Imeka::Surface::ImageToSurface(mitkImage, mitkImage));
    m_DM->AddNode(surfaceNode);

    const auto spacing = Vector3D( 2.5, 2.5, 2.5);
    const auto position = Point3D(0.0, 0.0, 0.0);
    for (const auto& nodeName : {"C", "A", "aa", "12", "Zz"})
    {
      CreateSO(spacing, position, nodeName);
    }

    // Test 1: surface should be at the end where there's no anat
    const auto allROIs = m_DM->GetAll(
      Imeka::Predicate::Property("SelectionROI", true));
    auto orderedNodes =
      Imeka::Fiber::GetOrderedIndexes(allROIs, false);
    CPPUNIT_ASSERT_EQUAL(std::string("12"), orderedNodes[0]->GetName());
    CPPUNIT_ASSERT_EQUAL(std::string("A"), orderedNodes[1]->GetName());
    CPPUNIT_ASSERT_EQUAL(std::string("aa"), orderedNodes[2]->GetName());
    CPPUNIT_ASSERT_EQUAL(std::string("C"), orderedNodes[3]->GetName());
    CPPUNIT_ASSERT_EQUAL(std::string("Zz"), orderedNodes[4]->GetName());
    CPPUNIT_ASSERT_EQUAL(std::string("surface"), orderedNodes[5]->GetName());

    // Test 2: surface should now be ordered normally
    mitk::DataNode::Pointer anatNode = AddAnatNode(mitkImage);
    orderedNodes = Imeka::Fiber::GetOrderedIndexes(allROIs, true);
    CPPUNIT_ASSERT_EQUAL(std::string("surface"), orderedNodes[4]->GetName());
    CPPUNIT_ASSERT_EQUAL(std::string("Zz"), orderedNodes[5]->GetName());
  }
};

MITK_TEST_SUITE_REGISTRATION(SimpleFiltering)
