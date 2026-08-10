
#include "utils.hpp"

#include <QApplication>
#include <QMessageBox>

#include "ImekaFiber/utils.hpp"

mitk::DataNode* GetAnatNodeWarn(
  Imeka::DataManager& DM,
  const bool noMaxima)
{
  const auto anatNode = DM.GetAnat();
  if (!anatNode)
  {
    QMessageBox::information(QApplication::activeWindow(), "MI-Brain",
      "Can't run this action if you haven't loaded a reference anatomy.");
    return nullptr;
  }

  if (noMaxima && Imeka::Fiber::GetMaximaPredicate()->CheckNode(anatNode))
  {
    QMessageBox::information(QApplication::activeWindow(), "MI-Brain",
      "Maxima image can't be used as a reference anatomy for this operation.");
    return nullptr;
  }

  return anatNode;
}
