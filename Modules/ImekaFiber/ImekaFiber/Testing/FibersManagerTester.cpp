
#include "FibersManagerTester.hpp"

#include <itkImage.h>

#include <vtkCellArray.h>
#include <vtkPolyData.h>

#include <mitkIOUtil.h>
#include <mitkTestingMacros.h>

#include "ImekaBoundingObject/BoundingObjectFactory.hpp"
#include "ImekaBoundingObject/NodeUtils.hpp"
#include "ImekaCommon/Predicate.hpp"
#include "ImekaFiber/utils.hpp"
#include "FiberBundle/DataStorageUtils.hpp"
#include "ImekaGeometry/MitkImageUtils.hpp"
#include "ImekaGeometry/VtkImageUtils.hpp"

FibersManagerTester::FibersManagerTester()
  : m_DS(nullptr)
  , m_DM(nullptr)
  , m_Groups(nullptr)
  , m_FM(nullptr)
  , m_BlockNodeAdded(false)
  , m_BlockNodeRemoved(false)
{}

void FibersManagerTester::setUp()
{
  m_DS = mitk::StandaloneDataStorage::New();
  m_DM = new Imeka::DataManager(m_DS);
  m_Groups = new Imeka::Fiber::GroupNodes(
    [](mitk::DataNode*){}, m_Callback, *m_DM);
  m_FM = new Imeka::Fiber::FibersManager(*m_DM, m_Callback, *m_Groups);

  /* We must add a NodeAdded listener because we are not connected to the
  normal MITK view system, so BrainAnalysisView::NodeAdded will never be
  called, which is not bad because it's only doing UI related stuff, but
  it's also calling FibersManager::NodeAdded... this is important!      */
  m_DS->AddNodeEvent.AddListener(
    mitk::MessageDelegate1<FibersManagerTester, const mitk::DataNode*>(
    this, &FibersManagerTester::NodeAddedProxy));
  m_DS->RemoveNodeEvent.AddListener(
    mitk::MessageDelegate1<FibersManagerTester, const mitk::DataNode*>(
      this, &FibersManagerTester::NodeRemovedProxy));

  m_FM->SetFilteringLengths(0.0, 400.0);
  m_FM->SetFilteringSkipParameters(0.0, 10000, true);

  DataStorageSingleton::dataStorage = m_DS;
}

void FibersManagerTester::tearDown()
{
  // Before destroying everything, we must make sure that octreeFuture has
  // finished calculating its result, otherwise it will crash.
  m_FM->WaitForAllFutures();

  m_DS->AddNodeEvent.RemoveListener(
    mitk::MessageDelegate1<FibersManagerTester, const mitk::DataNode*>(
    this, &FibersManagerTester::NodeAddedProxy));

  delete m_FM;
  delete m_Groups;
  delete m_DM;
  m_DS = nullptr;
}

FibersManagerTester::~FibersManagerTester()
{}

mitk::Image::Pointer FibersManagerTester::GetAnat(
  const mitk::Vector3D& spacing,
  const mitk::Point3D& origin,
  bool reverse)
{
  typedef itk::Image<float, 3> AnatType;
  const AnatType::SizeType size = { { 10, 10, 10 } };
  auto itkAnat = Imeka::Geometry::CreateItkImage<AnatType>(size);
  itkAnat->FillBuffer(0.0);

  for (int r = 0; r < 10; r++)
  {
    for (int c = 0; c < 10; c++)
    {
      const AnatType::IndexType index = { { r, c, 0 } };
      const float value = (r + c + 2) * 0.01; // 0.02 to 1.0
      if (reverse)
      {
        itkAnat->SetPixel(index, 1.0f - value);
      }
      else
      {
        itkAnat->SetPixel(index, value);
      }
    }
  }

  return Imeka::Geometry::MitkImageFromItkImage(itkAnat, spacing, origin, true);
}

mitk::DataNode* FibersManagerTester::AddAnatNode(mitk::Image* anat)
{
  if (!anat)
  {
    m_FM->SetAnat(nullptr, nullptr);
    return nullptr;
  }
  auto node = mitk::DataNode::New();
  node->SetData(anat);
  node->SetStringProperty("full_path", "some path");
  node->SetName("anat");
  m_DM->AddNode(node);
  m_FM->SetAnat(node, anat);
  return node;
}

mitk::FilteredFiberBundle::Pointer
FibersManagerTester::Get1_10Fibers(
  const mitk::BaseGeometry* anatGeo,
  const bool oblique)
{
  auto cells = vtkSmartPointer<vtkCellArray>::New();
  auto points = vtkSmartPointer<vtkPoints>::New();

  mitk::Point3D index(0.0), world;
  for (unsigned int i = 0; i < 10; ++i)
  {
    cells->InsertNextCell(oblique ? 3 : 2);

    index[0] = 0.0;
    index[1] = i * 0.5;
    anatGeo->IndexToWorld(index, world);
    cells->InsertCellPoint(points->InsertNextPoint(world.Begin()));

    index[0] = (i + 1) * 1.0;
    anatGeo->IndexToWorld(index, world);
    cells->InsertCellPoint(points->InsertNextPoint(world.Begin()));

    if (oblique)
    {
      index[1] += (i + 1) * 1.0;
      anatGeo->IndexToWorld(index, world);
      cells->InsertCellPoint(points->InsertNextPoint(world.Begin()));
    }
  }

  auto polyData = vtkSmartPointer<vtkPolyData>::New();
  polyData->SetPoints(points);
  polyData->SetLines(cells);

  return mitk::FilteredFiberBundle::New(polyData);;
}

mitk::DataNode* FibersManagerTester::AddFibersNode(
  mitk::FilteredFiberBundle* fibers,
  const char* nodeName)
{
  auto node = mitk::DataNode::New();
  node->SetData(fibers);
  node->SetStringProperty("full_path", "some path");
  node->SetName(nodeName);
  m_DM->AddNode(node);
  return node;
}

mitk::Image::Pointer FibersManagerTester::LoadImage(const std::string& path)
{
  const auto loadedBaseDatas = mitk::IOUtil::Load(path);
  mitk::Image::Pointer image =
    dynamic_cast<mitk::Image*>(loadedBaseDatas[0].GetPointer());
  return image;
}

mitk::FilteredFiberBundle::Pointer FibersManagerTester::LoadFibers(
  const std::string& path)
{
  const auto loadedBaseDatas = mitk::IOUtil::Load(path);
  auto baseData = loadedBaseDatas[0].GetPointer();
  mitk::FilteredFiberBundle::Pointer fibers =
    dynamic_cast<mitk::FilteredFiberBundle*>(baseData);
  return fibers;
}

std::pair<mitk::DataNode*, mitk::BoundingObject*>
FibersManagerTester::CreateSO(
  const mitk::Vector3D& spacing,
  const mitk::Point3D& origin,
  const std::string& nodeName) const
{
  auto SO = Imeka::BoundingObject::
    BoundingObjectFactory::get()->createBoundingObject("Cuboid");
  auto SONode = Imeka::BoundingObject::GetNewBoundingObjectNode(SO, false);
  m_DM->AddNode(SONode);
  if (nodeName != "")
  {
    auto newTGNode = m_DM->GetFirst(Imeka::Predicate::Property(
      "name", Imeka::BoundingObject::INTERNAL_BOUNDING_OBJECT_NAME));
    newTGNode->SetName(nodeName);
  }

  auto geo = SO->GetGeometry();
  geo->SetSpacing(spacing);
  geo->SetOrigin(origin);

  return std::make_pair(SONode, SO);
}

std::pair<mitk::DataNode*, mitk::BoundingObject*>
FibersManagerTester::SetupTG(
  mitk::DataNode* fibersNode,
  const mitk::Vector3D& spacing,
  const mitk::Point3D& origin,
  const std::string& nodeName)
{
  const auto p = CreateSO(spacing, origin);
  return std::make_pair(
    CreateTG(fibersNode, { p.first }, nodeName), p.second);
}

mitk::DataNode* FibersManagerTester::CreateTG(
  mitk::DataNode* fibersNode,
  const Nodes& ROINodes,
  const std::string& nodeName) const
{
  AutoSetupLines autoSetup;
  for (auto ROINode : ROINodes)
  {
    autoSetup[ROINode] =
      { nullptr, true, Imeka::Fiber::SelectionMode::AnyPart, false };
  }

  return CreateTG(fibersNode, autoSetup, nodeName);
}

mitk::DataNode* FibersManagerTester::CreateTG(
  mitk::DataNode* fibersNode,
  const AutoSetupLines& autoSetupLines,
  const std::string& nodeName) const
{
  Imeka::Fiber::ActiveROIsDialogBox::LinesToAutoSetup = &autoSetupLines;
  fibersNode->SetBoolProperty("CreateNewTractGroup", true);
  auto newTGNode = m_DM->GetFirst(
    Imeka::Predicate::Property("name", "NewTractGroup"));
  newTGNode->SetName(nodeName);
  return newTGNode;
}

void FibersManagerTester::EditTG(
  mitk::DataNode* TGNode,
  const AutoSetupLines& autoSetupLines) const
{
  Imeka::Fiber::ActiveROIsDialogBox::LinesToAutoSetup = &autoSetupLines;
  TGNode->SetBoolProperty("EditActiveROIs", true);
}

void FibersManagerTester::MoveSO(
  mitk::BoundingObject* SO, const mitk::Point3D& p)
{
  SO->GetGeometry()->SetOrigin(p);
  SO->Modified();
  m_FM->SelectionObjectMoved(SO);
  SO->Modified();
}

void FibersManagerTester::AssertColorAre(
  const mitk::DataNode* node,
  const std::vector<QColor>& expectedColors) const
{
  auto fiber = dynamic_cast<mitk::FilteredFiberBundle*>(node->GetData());
  const auto nbStreamlines = fiber->GetNumFibers();
  auto colors = fiber->GetFiberColors();
  vtkIdType ptIdx = 0;
  for (vtkIdType i = 0; i < nbStreamlines; ++i)
  {
    for (auto& color : expectedColors)
    {
      unsigned char rgba[] = { 0, 0, 0, 0 };
      colors->GetTypedTuple(ptIdx++, rgba);
      AssertColorIs(color, rgba);
    }
  }
}

void FibersManagerTester::AssertColorIs(
  const mitk::DataNode* node,
  const std::vector<QColor>& expectedColors) const
{
  auto fiber = dynamic_cast<mitk::FilteredFiberBundle*>(node->GetData());
  auto colors = fiber->GetFiberColors();
  unsigned char rgba[] = { 0, 0, 0, 0 };
  for (unsigned int i = 0; i < expectedColors.size(); ++i)
  {
    colors->GetTypedTuple(i, rgba);
    AssertColorIs(expectedColors[i], rgba);
  }
}

void FibersManagerTester::AssertColorIs(
  const QColor& expected,
  const unsigned char* colors) const
{
  QColor actual(colors[0], colors[1], colors[2]);
  AssertColorIs(expected, actual);
}

void FibersManagerTester::AssertColorIs(
  const QColor& expected,
  const QColor& actual) const
{
  if (expected.red() != actual.red()
    || expected.green() != actual.green()
    || expected.blue() != actual.blue())
  {
    std::stringstream ss;
    ss << "equality assertion failed\n"
      << "- Expected: "
        << expected.red() << ", "
        << expected.green() << ", "
        << expected.blue() << "\n"
      << "- Actual  : "
        << static_cast<unsigned int>(actual.red()) << ", "
        << static_cast<unsigned int>(actual.green()) << ", "
        << static_cast<unsigned int>(actual.blue()) << "\n";
    CPPUNIT_FAIL(ss.str().c_str());
  }
}

void FibersManagerTester::NodeAddedProxy(const mitk::DataNode* node)
{
  // Otherwise ChangeParent kill the program with a stack overflow
  if (!m_BlockNodeAdded)
  {
    m_BlockNodeAdded = true;

    auto nonConstNode = const_cast<mitk::DataNode*>(node);
    if (m_Groups->UpdateGroupIfRequired(nonConstNode))
    {
      m_BlockNodeAdded = false;
      return;
    }
    m_FM->NodeAdded(nonConstNode);

    m_BlockNodeAdded = false;
  }
}

void FibersManagerTester::NodeRemovedProxy(const mitk::DataNode* node)
{
  if (!m_BlockNodeRemoved)
  {
    m_BlockNodeRemoved = true;

    auto nonConstNode = const_cast<mitk::DataNode*>(node);
    m_FM->NodeRemoved(nonConstNode);

    m_BlockNodeRemoved = false;
  }
}
