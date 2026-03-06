#include "ConvertToMultiLabelAction.hpp"

#include <QMenu>
#include <QAction>
#include <QCursor>

#include <mitkImage.h>
#include <mitkLabelSetImage.h>
#include <mitkLabelSetImageConverter.h>
#include <mitkImageAccessByItk.h>

#include "ImekaCommon/Predicate.hpp"
#include "ImekaCommon/DataManager.hpp"
#include "ImekaFiber/GroupNodes.hpp"
#include "ImekaFiber/utils.hpp"
#include "utils.hpp"

void ConvertToMultiLabelAction::Run(const QList<mitk::DataNode::Pointer> &selectedNodes)
{
  if (selectedNodes.empty()) return;

  Imeka::DataManager DM(m_DS);
  auto refAnatNode = DM.GetAnat();

  if (!refAnatNode)
  {
    std::cout << "ConvertToMultiLabelAction: No reference anatomy selected in the Fiber Analysis plugin. Cannot convert.\n";
    return;
  }

  for (auto node : selectedNodes)
  {
    auto image = dynamic_cast<mitk::Image*>(node->GetData());
    if (!image) continue;

    try {
      // Use mitk::ConvertImageToLabelSetImage to convert the ROI to Multi-Label Segmentation.
      auto mls = mitk::ConvertImageToLabelSetImage(image);
      
      auto newNode = mitk::DataNode::New();
      newNode->SetData(mls);
      newNode->SetName(node->GetName() + " (Multi-Label)");
      
      // Explicitly set ROI properties so mi-brain treats it as a selectable ROI
      newNode->SetBoolProperty("SelectionROI", true);
      
      // Place it under the reference anatomy as requested
      DM.AddNode(newNode, refAnatNode);
      
      std::cout << "ConvertToMultiLabelAction: Converted " << node->GetName() 
                << " to Multi-Label Segmentation as child of " << refAnatNode->GetName() << ".\n";
    }
    catch (const std::exception& e) {
      std::cout << "ConvertToMultiLabelAction: Exception: " << e.what() << "\n";
    }
  }
  mitk::RenderingManager::GetInstance()->RequestUpdateAll();
}
