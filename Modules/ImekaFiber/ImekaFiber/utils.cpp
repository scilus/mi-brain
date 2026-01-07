#include "utils.hpp"

#include <QApplication>
#include <QMessageBox>

#include <mitkITKImageImport.h>
#include <mitkNodePredicateDataType.h>

#include "ImekaCommon/Predicate.hpp"
#include "ImekaFiber/Mapper/Mappers2DSettingsWidget.hpp"
#include "ImekaGeometry/VtkImageUtils.hpp"

namespace Imeka
{

namespace Fiber
{

const char* GroupPropertyName = "group_name";
const char* TractGroupName = "TractGroup";
const char* ActiveROIName = "ActiveROI";

mitk::NodePredicateAnd::Pointer GetAnatPredicate()
{
  return mitk::NodePredicateAnd::New(
    mitk::NodePredicateDataType::New("Image"),
    Imeka::Predicate::NotHelperObject());
}

mitk::NodePredicateImageInfo::Pointer GetRGBPredicate()
{
  auto pred = mitk::NodePredicateImageInfo::New();
  pred->SetPixelComponents(1);
  pred->SetDimension(4);
  pred->AddDimensionSize(3, 3);
  pred->SetPixelType("unsigned_char");
  return pred;
}

mitk::NodePredicateAnd::Pointer GetTensorsPredicate()
{
  auto pred = mitk::NodePredicateImageInfo::New();
  pred->SetPixelType("float");
  pred->SetPixelComponents(1);
  pred->SetDimension(4);
  pred->AddDimensionSize(3, 6);

  return mitk::NodePredicateAnd::New(
    pred, Imeka::Predicate::NotHelperObject());;
}

mitk::NodePredicateAnd::Pointer GetMaximaPredicate(const bool ignoreNotAPeaks)
{
  auto pred = mitk::NodePredicateImageInfo::New();
  pred->SetPixelType("float");
  pred->SetPixelComponents(1);
  pred->SetDimension(4);
  for (const auto size : { 3, 6, 9, 12, 15 })
  {
    pred->AddDimensionSize(3, size);
  }

  auto isMaxima = mitk::NodePredicateAnd::New();
  isMaxima->AddPredicate(pred);
  if (!ignoreNotAPeaks)
  {
    isMaxima->AddPredicate(Imeka::Predicate::NotProperty("NotAPeaks", true));
  }
  isMaxima->AddPredicate(Imeka::Predicate::NotHelperObject());
  return isMaxima;
}

mitk::NodePredicateAnd::Pointer IsMaskPredicate()
{
  auto is3DImage = mitk::NodePredicateImageInfo::New();
  is3DImage->SetDimension(3);

  return mitk::NodePredicateAnd::New(
    Imeka::Predicate::Property("binary", true),
    is3DImage
  );
}

mitk::NodePredicateOr::Pointer GetROIPredicate()
{
  auto pred = mitk::NodePredicateOr::New();
  pred->AddPredicate(Imeka::Predicate::IsBoundingObjectPredicate());
  pred->AddPredicate(mitk::NodePredicateDataType::New("LabelSetImage"));
  pred->AddPredicate(mitk::NodePredicateDataType::New("Surface"));
  pred->AddPredicate(IsMaskPredicate());
  return pred;
}

mitk::NodePredicateOr::Pointer GetTractPredicate()
{
  return mitk::NodePredicateOr::New(
    mitk::NodePredicateDataType::New("FiberBundle"),
    mitk::NodePredicateDataType::New("FilteredFiberBundle"));
}

mitk::NodePredicateProperty::Pointer GetTractGroupPredicate()
{
  return Imeka::Predicate::Property(GroupPropertyName, TractGroupName);
}

mitk::NodePredicateProperty::Pointer GetActiveROIPredicate()
{
  return Imeka::Predicate::Property(GroupPropertyName, ActiveROIName);
}

bool IsLabelsImage(const mitk::Image* image)
{
  if (image->GetDimension() != 3) { return false; }

  const auto pixelType = image->GetPixelType().GetTypeAsString();
  if (pixelType == "scalar (short)")
  {
    auto itkImage = itk::Image<short, 3>::New();
    mitk::CastToItkImage(image, itkImage);
    const auto labels =
      Imeka::Geometry::IsActuallyIntegers(itkImage.GetPointer());
    return labels.size() < 100;
  }
  else if (pixelType == "scalar (float)")
  {
    auto itkImage = itk::Image<float, 3>::New();
    mitk::CastToItkImage(image, itkImage);
    const auto labels =
      Imeka::Geometry::IsActuallyIntegers(itkImage.GetPointer());
    return !labels.empty() && labels.size() < 100;
  }

  return false;
}

bool IsTract(const mitk::DataNode* node)
{
  return GetTractPredicate()->CheckNode(node);
}

bool IsTractGroup(const mitk::DataNode* node)
{
  return GetTractGroupPredicate()->CheckNode(node);
}

bool IsActiveROI(const mitk::DataNode* node)
{
  return GetActiveROIPredicate()->CheckNode(node);
}

void SetupNodeDataAndMappers(
  mitk::DataNode* node,
  mitk::FilteredFiberBundle* fiber,
  FiberNodeData& fiberNodeData)
{
  std::cout << "SetupNodeDataAndMappers: Setting up fiber node " << node->GetName() << "\n";
  
  node->SetBoolProperty("pickable", false);
  node->SetBoolProperty("Fiber2DfadeEFX", false);

  // MITK 2025: Set visibility for all renderers (3D + 2D)
  // SetVisibility for 2D is handled by Mappers2DSettingsWidget
  // But we need to explicitly enable 3D visibility
  std::cout << "  Setting global visibility to true\n";
  node->SetVisibility(true);  // Sets default/global visibility
  
  std::cout << "  Setting 2D visibility via Mappers2DSettingsWidget\n";
  Mappers2DSettingsWidget::SetVisibility(node);  // Sets 2D visibility
  Mappers2DSettingsWidget::SetFiberThickness(node);
  
  std::cout << "  Checking visibility after setup:\n";
  bool vis = false;
  node->GetVisibility(vis, nullptr);
  std::cout << "    Global (nullptr): " << vis << "\n";

  // TODO Do we really need (or even use) this?
  fiberNodeData.fiberMapperData.fiberBundle = fiber;

  // 2D
  auto mapper2D = mitk::MitkFiberMapper2D::New();
  fiberNodeData.fiberMapper2D = mapper2D;
  mapper2D->SetFiberMapperData(&fiberNodeData.fiberMapperData);
  node->SetMapper(1, mapper2D);
  mitk::MitkFiberMapper2D::SetDefaultProperties(node);

  // 3D
  auto mapper3D = mitk::MitkFiberMapper3D::New();
  mapper3D->SetFiberMapperData(&fiberNodeData.fiberMapperData);
  fiberNodeData.fiberMapper3D = mapper3D;
  node->SetMapper(2, mapper3D);
  mitk::MitkFiberMapper3D::SetDefaultProperties(node);
  
  // MITK 2025: Force 3D visibility to true after SetDefaultProperties
  // SetDefaultProperties may have set "visible" to false
  std::cout << "  Forcing 3D renderer visibility to true\n";
  auto allRenderWindows = mitk::RenderingManager::GetInstance()->GetAllRegisteredRenderWindows();
  for (auto renderWindow : allRenderWindows)
  {
    auto renderer = mitk::BaseRenderer::GetInstance(renderWindow);
    if (renderer && renderer->GetMapperID() == mitk::BaseRenderer::Standard3D)
    {
      node->SetBoolProperty("visible", true, renderer);
      std::cout << "    Set visibility=true for 3D renderer: " << renderer->GetName() << "\n";
    }
  }
}

mitk::FilteredFiberBundle::Pointer Union(const ConstNodes& nodes)
{
  unsigned int nbPoints = 0;
  std::vector<mitk::FilteredFiberBundle*> fiberBundles;
  fiberBundles.reserve(nodes.size());
  std::set<mitk::FilteredFiberBundle::Coloring> colorTypes;

  for (auto& node : nodes)
  {
    if (!node->IsVisible(nullptr)) { continue; }
    const auto fibers =
      dynamic_cast<mitk::FilteredFiberBundle*>(node->GetData());
    nbPoints += fibers->GetNumberOfPoints();
    fiberBundles.push_back(fibers);
    colorTypes.insert(fibers->GetCurrentColoring());
  }

  // Save colors only if NOT Orientation
  vtkSmartPointer<vtkUnsignedCharArray> colors = nullptr;
  if (colorTypes.size() > 1
    || *colorTypes.begin() != mitk::FilteredFiberBundle::Orientation)
  {
    colors = mitk::FilteredFiberBundle::GetNewColorArray(nbPoints);
  }

  auto lines = vtkSmartPointer<vtkCellArray>::New();
  auto points = vtkSmartPointer<vtkPoints>::New();

  for (auto& fiber : fiberBundles)
  {
    fiber->ExportDataTo(lines, points, colors);
  }

  mitk::FilteredFiberBundle::Pointer newFibers = nullptr;
  if (points->GetNumberOfPoints() > 0)
  {
    newFibers = mitk::FilteredFiberBundle::New(lines, points, colors);
  }

  if (!newFibers || newFibers->GetNumberOfPoints() == 0)
  {
    QMessageBox::information(
      QApplication::activeWindow(), "Brain Analysis",
      "There's no visible fiber on the screen.");
    return nullptr;
  }

  return newFibers;
}

mitk::FilteredFiberBundle::Pointer Union(
  const QList<mitk::DataNode::Pointer>& nodes)
{
  const ConstNodes nodesVector(std::begin(nodes), std::end(nodes));
  return Union(nodesVector);
}

} // namespace Fiber

} // namespace Imeka
