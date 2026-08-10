#include "ConvertToBinaryROIAction.hpp"

#include <mitkImageCast.h>
#include <mitkLevelWindowProperty.h>
#include <itkBinaryThresholdImageFilter.h>

// MITK Multilabel headers
#include <mitkLabelSetImage.h>
#include <mitkLabelSetImageConverter.h>

#include "ImekaCommon/Predicate.hpp"
#include "ImekaFiber/GroupNodes.hpp"
#include "ImekaFiber/utils.hpp"
#include "utils.hpp"

void ConvertToBinaryROIAction::Run(const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  Imeka::DataManager DM(m_DS);

  for (auto node : selectedNodes)
  {
    if (!node) continue;

    mitk::Image::Pointer mitkImage = dynamic_cast<mitk::Image*>(node->GetData());
    
    // If it's not a direct mitk::Image, check if it's a MultiLabelSegmentation
    if (mitkImage.IsNull())
    {
      auto mls = dynamic_cast<mitk::MultiLabelSegmentation*>(node->GetData());
      if (mls)
      {
        mitkImage = mitk::ConvertLabelSetImageToImage(mls);
      }
    }

    if (mitkImage.IsNull())
    {
      std::cout << "ConvertToBinaryROIAction: Node " << node->GetName() 
                << " is neither an Image nor a MultiLabelSegmentation.\n";
      continue;
    }

    try {
      // Thresholding: everything > 0 becomes 1
      typedef itk::Image<unsigned char, 3> BinaryImageType;
      typedef itk::Image<float, 3> InputImageType;
      
      auto itkInput = InputImageType::New();
      mitk::CastToItkImage(mitkImage, itkInput);

      auto thresholder = itk::BinaryThresholdImageFilter<InputImageType, BinaryImageType>::New();
      thresholder->SetInput(itkInput);
      thresholder->SetLowerThreshold(0.5); 
      thresholder->SetInsideValue(1);
      thresholder->SetOutsideValue(0);
      thresholder->Update();

      auto mitkBinary = mitk::Image::New();
      mitk::CastToMitkImage(thresholder->GetOutput(), mitkBinary);

      auto newNode = mitk::DataNode::New();
      newNode->SetData(mitkBinary);
      newNode->SetName(node->GetName() + " (Binary ROI)");
      newNode->SetBoolProperty("binary", true);
      newNode->SetBoolProperty("SelectionROI", true);
      newNode->SetBoolProperty("segmentation", true);
      newNode->SetBoolProperty("org.mitk.views.segmentation.ismask", true);

      mitk::LevelWindow lw;
      lw.SetWindowBounds(0, 1);
      newNode->SetProperty("levelwindow", mitk::LevelWindowProperty::New(lw));

      // Add to ROIs category
      auto rois = DM.GetAll(Imeka::Predicate::Property(
        Imeka::Fiber::GroupNodes::CategoryPropertyName,
        Imeka::Fiber::GroupNodes::ROIsCategoryName));
      
      if (rois.empty())
      {
        DM.AddNode(newNode);
      }
      else
      {
        DM.AddNode(newNode, rois[0]);
      }
    }
    catch (const std::exception& e) {
      std::cout << "ConvertToBinaryROIAction: Exception: " << e.what() << "\n";
    }
  }
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}
