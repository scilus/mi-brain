
#ifndef MIBRAIN_CUT_FIBERS_ABSTRACT_ACTION_HPP_INCLUDED
#define MIBRAIN_CUT_FIBERS_ABSTRACT_ACTION_HPP_INCLUDED

#include <mitkDataNode.h>
#include <mitkIContextMenuAction.h>

class CutFibersAbstractAction : public mitk::IContextMenuAction
{
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  CutFibersAbstractAction()
    : m_DS(nullptr)
  {}

  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override;
  void SetDataStorage(mitk::DataStorage* ds) override { m_DS = ds; }
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

protected:
  virtual std::string GetCutName() const = 0;

private:
  CutFibersAbstractAction(const CutFibersAbstractAction &);
  CutFibersAbstractAction & operator=(const CutFibersAbstractAction &);

  mitk::DataStorage* m_DS;
};

#endif // MIBRAIN_CUT_FIBERS_ABSTRACT_ACTION_HPP_INCLUDED
