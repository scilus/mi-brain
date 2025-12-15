
#ifndef MIBRAIN_FLIP_FIBERS_ABSTRACT_ACTION_HPP_INCLUDED
#define MIBRAIN_FLIP_FIBERS_ABSTRACT_ACTION_HPP_INCLUDED

#include <mitkDataNode.h>
#include <mitkIContextMenuAction.h>

class FlipFibersAbstractAction : public mitk::IContextMenuAction
{
public:
  FlipFibersAbstractAction()
    : m_DS(nullptr) 
  {}

  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override;
  void SetDataStorage(mitk::DataStorage* ds) override { m_DS = ds; }
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

protected:
  virtual int GetAxis() const = 0;

private:
  FlipFibersAbstractAction(const FlipFibersAbstractAction &);
  FlipFibersAbstractAction & operator=(const FlipFibersAbstractAction &);

  mitk::DataStorage* m_DS;
};

#endif // MIBRAIN_FLIP_FIBERS_ABSTRACT_ACTION_HPP_INCLUDED
