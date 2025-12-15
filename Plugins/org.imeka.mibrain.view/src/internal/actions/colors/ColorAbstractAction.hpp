
#ifndef MIBRAIN_COLOR_ABSTRACT_ACTION_HPP_INCLUDED
#define MIBRAIN_COLOR_ABSTRACT_ACTION_HPP_INCLUDED

#include <mitkDataNode.h>
#include "FiberBundle/FilteredFiberBundle.hpp"
#include <mitkIContextMenuAction.h>

class ColorAbstractAction : public mitk::IContextMenuAction
{
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  ColorAbstractAction()
    : m_DS(nullptr)
  {}

  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override;
  void SetDataStorage(mitk::DataStorage* ds) override { m_DS = ds; }
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

protected:
  virtual bool NeedAnat() const = 0;
  virtual mitk::FilteredFiberBundle::Coloring GetColoring() const = 0;

private:
  ColorAbstractAction(const ColorAbstractAction &);
  ColorAbstractAction & operator=(const ColorAbstractAction &);

  mitk::DataStorage* m_DS;
};

#endif // MIBRAIN_COLOR_ABSTRACT_ACTION_HPP_INCLUDED
