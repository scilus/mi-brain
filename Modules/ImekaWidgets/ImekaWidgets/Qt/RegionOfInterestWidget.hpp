
#ifndef IMEKA_WIDGETS_REGION_OF_INTEREST_HPP_INCLUDED
#define IMEKA_WIDGETS_REGION_OF_INTEREST_HPP_INCLUDED

#include <mitkBoundingObject.h>
#include <mitkDataNode.h>
#include <mitkDataStorage.h>

#include <QWidget>

#include "ImekaCommon/DataManager.hpp"

#include "ui_RegionOfInterestWidget.h"

#include "ImekaWidgetsExports.h"

namespace mitk { class BoundingObject; };

namespace Imeka
{

namespace Widgets
{

class ImekaWidgets_EXPORT RegionOfInterestWidget : public QWidget,
  public Ui::RegionOfInterestWidget
{
  Q_OBJECT

static const QString DEFAULT_DIR;

public:
  enum UI_STATE
  {
    DISABLED = 0,
    NODE_SELECTED,
    ROI_EXISTS
  };

  RegionOfInterestWidget(QWidget* = 0);

  void SetDataStorage(mitk::DataStorage*);
  void ShowActionButtons();
  void HideActionButtons();
  void SetParentNode(mitk::DataNode::Pointer);
  mitk::DataNode* GetParentNode() { return m_ParentNode; }
  void SetAnat(mitk::DataNode::Pointer);
  void AddTypeToRemove(const QString& t) { m_TypesToRemove.append(t); }
  void SetOneBOMax(const bool b) { m_OneBOMax = b; }
  void IsHelperObject(const bool b) { m_IsHelperObject = b; }
  void UpdateROIInformationForActiveROI() { UpdateROIInformation(m_SelectedBO); }

  void CreateBO(
    const std::string&,
    std::function<void(mitk::DataNode*)> = std::function<void(mitk::DataNode*)>());

  void FinishAdding(
    mitk::BoundingObject *,
    mitk::DataNode::Pointer,
    mitk::BaseGeometry * = nullptr,
    const bool = true);

signals:
  void RequestUIUpdate(const UI_STATE);
  void RequestROIInformationUpdate(mitk::BoundingObject *);
  void DataStorageChanged();
  void SelectedNodeChanged();
  void PreROICreated(mitk::BoundingObject*);
  void ROICreated(mitk::BoundingObject*);
  void ROIHasBeenMoved(mitk::BoundingObject*);
  void ROIHasBeenResized(mitk::BoundingObject*);

public slots:
  void on_createROIButton_clicked();
  void on_importROIButton_clicked();
  void on_exportROIButton_clicked();
  void UpdateROIInformation(mitk::BoundingObject *);
  void UpdateUI(const UI_STATE);
  void UpdateBoundingObjectInteractors();

  // Qt autoconnect by name
  void on_ROIx_valueChanged(const double& d) { moveROI(0, d); }
  void on_ROIy_valueChanged(const double& d) { moveROI(1, d); }
  void on_ROIz_valueChanged(const double& d) { moveROI(2, d); }
  void on_ROIWidth_valueChanged(const double& d) { resizeROI(0, d); }
  void on_ROIHeight_valueChanged(const double& d){ resizeROI(1, d); }
  void on_ROIDepth_valueChanged(const double& d){ resizeROI(2, d); }

private:
  void moveROI(const unsigned int, const double &);
  void resizeROI(const unsigned int, const double &);
  void CheckBOExistence();
  unsigned int GetTime();

  Imeka::DataManager m_DM;
  mitk::DataNode* m_ParentNode;
  mitk::BaseData* m_Anat;
  mitk::BoundingObject* m_SelectedBO;

  QStringList m_TypesToRemove;
  bool m_OneBOMax;
  bool m_IsHelperObject;
  bool m_AlreadyEmited;
};

} // namespace Widgets

} // namespace Imeka

#endif // IMEKA_WIDGETS_REGION_OF_INTEREST_HPP_INCLUDED

