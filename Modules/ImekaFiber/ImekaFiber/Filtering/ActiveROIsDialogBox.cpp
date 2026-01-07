
#include "ActiveROIsDialogBox.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QFontMetrics>
#include <QGridLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>

#include <boost/algorithm/string.hpp>

#include "ImekaCommon/Predicate.hpp"

namespace Imeka
{

namespace Fiber
{

const ActiveROIsDialogBox::AutoSetupLines*
  ActiveROIsDialogBox::LinesToAutoSetup = nullptr;

ActiveROIsDialogBox::ActiveROIsDialogBox(QWidget* parent)
  : QDialog(parent)
{
  // Title of each column
  m_Title1 = new QLabel("ROI name");
  m_Title2 = new QLabel("Color");
  m_Title3 = new QLabel("Segment selection");
  m_Title4 = new QLabel("NOT?");
  m_Title1->setAlignment(Qt::AlignHCenter);
  m_Title2->setAlignment(Qt::AlignHCenter);
  m_Title3->setAlignment(Qt::AlignHCenter);
  m_Title4->setAlignment(Qt::AlignHCenter);

  // Scrolling area
  m_ScrollArea = new QScrollArea();
  m_ScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  m_ScrollArea->setMinimumWidth(400);
  m_ScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

  // Button to close the QDialog and create the tractGroup
  auto confirmButton = new QPushButton("Confirm");
  confirmButton->setFixedSize(200, 25);
  auto buttonLayout = new QGridLayout;
  buttonLayout->setAlignment(confirmButton, Qt::AlignHCenter);
  buttonLayout->addWidget(confirmButton);
  m_OkButton = new QWidget;
  m_OkButton->setLayout(buttonLayout);
  connect(confirmButton, &QPushButton::released, [this](){ accept(); });

  // Layout of the QDialog
  auto mainLayout = new QVBoxLayout;
  mainLayout->addWidget(m_ScrollArea);
  mainLayout->addWidget(m_OkButton);
  setLayout(mainLayout);
}

bool ActiveROIsDialogBox::GenerateDialogBox(
  TractGroup& tractGroup,
  const mitk::DataNode* newTractGroup,
  const Nodes& allROINodes,
  const bool hasAnat)
{
  m_TractGroup = newTractGroup;

  // Add the titles to the layout
  auto m_Layout = new QGridLayout;
  m_Layout->addWidget(m_Title1, 0, 1);
  m_Layout->addWidget(m_Title2, 0, 2);
  m_Layout->addWidget(m_Title3, 0, 3);
  m_Layout->addWidget(m_Title4, 0, 4);

  std::map<mitk::DataNode*, UILine> UILines;
  const auto fm = m_Title1->fontMetrics();
  const int absoluteMinWidth = [this, fm]() {
    // The layout must be IN another widget otherwise all values == 0
    auto tmpWidget = new QWidget;
    auto layout = new QGridLayout;
    tmpWidget->setLayout(layout);
    m_ScrollArea->setWidget(tmpWidget);
    const int hSpace =
      // 3 * layout->horizontalSpacing() I don't know why we don't need it
        layout->contentsMargins().left()
      + layout->contentsMargins().right();

    return hSpace
      + fm.horizontalAdvance(m_Title1->text())
      + 200 // Min name label size
      + fm.horizontalAdvance(m_Title3->text())
      + fm.horizontalAdvance(m_Title4->text()) * 2;
  }();
  int currentMinWidth = absoluteMinWidth;
  int lineIdx = 1;
  for (const auto ROINode : GetOrderedIndexes(allROINodes, hasAnat))
  {
    // Will create a default one if the ROI didn't exist in this tractGroup
    const auto line = tractGroup.lines[ROINode->GetData()];
    const auto uiLine = GetUILine(allROINodes, line, ROINode);
    UILines[ROINode] = uiLine;
 
    currentMinWidth = std::max(
      currentMinWidth,
      absoluteMinWidth + fm.horizontalAdvance(uiLine.name->text()) - 200);
    m_Layout->addWidget(uiLine.enabled, lineIdx, 0);
    m_Layout->addWidget(uiLine.name, lineIdx, 1);

    // Disale the line if 1) no anat 2) ROI is a surface
    if (IsSurfaceAndWeCare(ROINode, hasAnat))
    {
      uiLine.enabled->setDisabled(true);
      uiLine.name->setDisabled(true);
      m_Layout->addWidget(
        new QLabel(" Need a reference anatomy."),
        lineIdx, 2, 1, 3);
    }
    else
    {
      m_Layout->addWidget(uiLine.color, lineIdx, 2, Qt::AlignHCenter);
      m_Layout->addWidget(uiLine.segmentSelection, lineIdx, 3);
      m_Layout->addWidget(uiLine._not, lineIdx, 4, Qt::AlignHCenter);
    }
    ++lineIdx;
  }

  // If there is too many ROI, we need a scroll bar to naviguate
  auto ActiveROIslist = new QWidget;
  ActiveROIslist->setLayout(m_Layout);
  m_ScrollArea->setWidget(ActiveROIslist);
  m_ScrollArea->setMinimumWidth(currentMinWidth);

  const auto answer = exec();
  if (answer == QDialog::Rejected) { return false; }

  // The dialog just closed, so we can fill tractGroup with informations
  //////////////////////////////////////////////////////////////////////

  bool atLeastOneModif = false;
  unsigned int row = 1;
  tractGroup.modified = false;
  for (const auto ROINode_UILine : UILines)
  {
    const auto ROINode = ROINode_UILine.first;
    const auto& uiLine = ROINode_UILine.second;

    auto& oldLine = tractGroup.lines.at(ROINode->GetData());

    TractGroup::Line line = {
      oldLine.activeROINode, // nullptr for new TG
      uiLine.enabled->isChecked(),
      static_cast<SelectionMode>(uiLine.segmentSelection->currentIndex()),
      uiLine._not->isChecked()
    };

    tractGroup.modified |= (line != oldLine); // At least one different
    atLeastOneModif |= tractGroup.modified;
    oldLine = line;

    ++row;
  }

  return atLeastOneModif;
}

void ActiveROIsDialogBox::AutoSetup(
  TractGroup& tractGroup,
  const Nodes& allROINodes,
  const bool hasAnat)
{
  if (LinesToAutoSetup->empty()) { return; }

  for (const auto ROINode : GetOrderedIndexes(allROINodes, hasAnat))
  {
    auto& line = tractGroup.lines[ROINode->GetData()];

    auto it = LinesToAutoSetup->find(ROINode);
    if (it != LinesToAutoSetup->end())
    {
      auto setup = it->second;
      line.activeROINode = setup.activeROINode;
      line.enabled = setup.enabled;
      line.mode = setup.mode;
      line.isNot = setup.isNot;
    }
    else
    {
      line.activeROINode = nullptr;
      line.enabled = false;
      line.mode = AnyPart;
      line.isNot = false;
    }
  }
  tractGroup.modified = true;
}

ActiveROIsDialogBox::UILine ActiveROIsDialogBox::GetUILine(
  const Nodes& ROIsNodes,
  const TractGroup::Line& line,
  const mitk::DataNode* ROINode) const
{
  // Auto select if only one ROI
  QCheckBox* enabled = new QCheckBox();
  enabled->setChecked(line.enabled || ROIsNodes.size() == 1);

  QPalette whitePalette;
  whitePalette.setColor(QPalette::Window, Qt::white);

  auto name = new ClickableLabel(ROINode->GetName().c_str());
  name->setMinimumSize(200, 25);
  name->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
  name->setAlignment(Qt::AlignLeft);
  name->setAlignment(Qt::AlignVCenter);
  name->setAutoFillBackground(true);
  name->setPalette(whitePalette);
  name->setStyleSheet("border: 1px solid black");
  connect(name, &ClickableLabel::clicked, [enabled](){
    enabled->setChecked(!enabled->isChecked());
  });

  QLabel* color = new QLabel("");
  color->setAutoFillBackground(true);
  float ROIColor[3];
  ROINode->GetColor(ROIColor);
  QPalette colorPalette;
  colorPalette.setColor(QPalette::Window, QColor::fromRgb(
    ROIColor[0] * 255, ROIColor[1] * 255, ROIColor[2] * 255));
  color->setPalette(colorPalette);
  color->setFixedSize(25, 25);

  QComboBox* segmentSelection = new QComboBox();
  segmentSelection->addItem("Any part");
  segmentSelection->addItem("Either end");
  segmentSelection->addItem("Both end");
  segmentSelection->addItem("No end");
  segmentSelection->setCurrentIndex(static_cast<int>(line.mode));

  auto _not = new QCheckBox();
  _not->setChecked(line.isNot);

  return{ enabled, name, color, segmentSelection, _not };
}

const mitk::DataNode* ActiveROIsDialogBox::GetTractGroup()
{
  return m_TractGroup;
}

Nodes GetOrderedIndexes(
  const Nodes& ROIs,
  const bool hasAnat)
{
  // Sort the indexes with a comparator on the name vector, AND put the
  // sutface at the end if there's no anat.
  Nodes orderedNodes = ROIs;
  std::sort(begin(orderedNodes), end(orderedNodes),
    [hasAnat](mitk::DataNode* n1, mitk::DataNode* n2)
  {
    const bool _1 = IsSurfaceAndWeCare(n1, hasAnat);
    const bool _2 = IsSurfaceAndWeCare(n2, hasAnat);
    if (_1 ^ _2)
    {
      if (_1) { return false; }
      return true;
    }
    const auto l1 = QString::fromStdString(n1->GetName()).toLower();
    const auto l2 = QString::fromStdString(n2->GetName()).toLower();
    return l1 < l2;
  });

  return orderedNodes;
}

bool IsSurfaceAndWeCare(
  const mitk::DataNode* node,
  const bool hasAnat)
{
  if (hasAnat) { return false; }
  return dynamic_cast<mitk::Surface*>(node->GetData()) != nullptr
    && !Imeka::Predicate::IsBoundingObjectPredicate()->CheckNode(node);
}

} // namespace Fiber

} // namespace Imeka
