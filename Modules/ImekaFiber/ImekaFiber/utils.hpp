#ifndef IMEKA_FIBER_UTILS_HPP_INCLUDED
#define IMEKA_FIBER_UTILS_HPP_INCLUDED

#include <FiberBundle/FilteredFiberBundle.hpp>
#include <mitkImageCast.h>
#include <mitkNodePredicateAnd.h>
#include <mitkNodePredicateOr.h>
#include <mitkNodePredicateProperty.h>

#include "FiberBundle/itkTractDensityImageFilter.h"

#include "ImekaCommon/mitkNodePredicateImageInfo.hpp"
#include "ImekaCommon/types.hpp"
#include "FiberNodeData.hpp"
#include "types.hpp"

#include "ImekaFiberExports.h"

namespace Imeka
{

namespace Fiber
{

extern const char* GroupPropertyName;
extern const char* TractGroupName;
extern const char* ActiveROIName;

mitk::NodePredicateAnd::Pointer ImekaFiber_EXPORT GetAnatPredicate();
mitk::NodePredicateImageInfo::Pointer ImekaFiber_EXPORT GetRGBPredicate();
mitk::NodePredicateAnd::Pointer ImekaFiber_EXPORT GetTensorsPredicate();
mitk::NodePredicateAnd::Pointer ImekaFiber_EXPORT
  GetMaximaPredicate(const bool ignoreNotAPeaks=false);
mitk::NodePredicateAnd::Pointer ImekaFiber_EXPORT IsMaskPredicate();
mitk::NodePredicateOr::Pointer ImekaFiber_EXPORT GetROIPredicate();
mitk::NodePredicateOr::Pointer ImekaFiber_EXPORT GetTractPredicate();
mitk::NodePredicateProperty::Pointer ImekaFiber_EXPORT GetTractGroupPredicate();
mitk::NodePredicateProperty::Pointer ImekaFiber_EXPORT GetActiveROIPredicate();

bool IsLabelsImage(const mitk::Image*);

bool ImekaFiber_EXPORT IsTract(const mitk::DataNode*);
bool ImekaFiber_EXPORT IsTractGroup(const mitk::DataNode*);
bool ImekaFiber_EXPORT IsActiveROI(const mitk::DataNode*);

void ImekaFiber_EXPORT SetupNodeDataAndMappers(
  mitk::DataNode*, mitk::FilteredFiberBundle*, FiberNodeData&);

mitk::FilteredFiberBundle::Pointer ImekaFiber_EXPORT Union(const ConstNodes&);
mitk::FilteredFiberBundle::Pointer ImekaFiber_EXPORT Union(
  const QList<mitk::DataNode::Pointer>&);

template <class OutPixType>
typename itk::Image<OutPixType, 3>::Pointer GetTractDensityImage(
  mitk::FilteredFiberBundle::Pointer fibers,
  const mitk::DataNode* trackingMaskNode,
  const bool binary)
{
  if (!fibers) { return nullptr; }

  typedef itk::Image<OutPixType, 3> OutImageType;

  auto generator = itk::TractDensityImageFilter<OutImageType>::New();
  generator->SetFiberBundle(mitk::FiberBundle::Pointer(fibers.GetPointer()));
  generator->SetBinaryOutput(binary);
  generator->SetOutputAbsoluteValues(true);
  generator->SetUpsamplingFactor(1.0);

  if (trackingMaskNode)
  {
    auto mitkImage = dynamic_cast<mitk::Image *>(trackingMaskNode->GetData());
    auto itkImage = OutImageType::New();
    
    // Geometry-only copy to avoid pixel type mismatch crashes
    typename OutImageType::RegionType region;
    typename OutImageType::RegionType::SizeType size;
    size[0] = mitkImage->GetDimension(0);
    size[1] = mitkImage->GetDimension(1);
    size[2] = mitkImage->GetDimension(2);
    region.SetSize(size);
    itkImage->SetRegions(region);
    itkImage->SetSpacing(mitkImage->GetGeometry()->GetSpacing());
    itkImage->SetOrigin(mitkImage->GetGeometry()->GetOrigin());
    
    itk::Matrix<double, 3, 3> direction;
    for (int i=0; i<3; ++i)
      for (int j=0; j<3; ++j)
        direction[j][i] = mitkImage->GetGeometry()->GetMatrixColumn(i)[j] / mitkImage->GetGeometry()->GetSpacing()[i];
    itkImage->SetDirection(direction);

    generator->SetInputImage(itkImage);
    generator->SetUseImageGeometry(true);
  }

  std::cout << "Fibers::GetTractDensityImage: fibers=" << fibers->GetNumFibers() << ", points=" << fibers->GetNumberOfPoints() << "\n";
  try
  {
    generator->Update();
  }
  catch (itk::ExceptionObject& e)
  {
    std::cerr << "Fibers::GetTractDensityImage: ITK exception: " << e.GetDescription() << "\n";
    return nullptr;
  }
  return generator->GetOutput();
}

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_UTILS_HPP_INCLUDED
