#ifndef MIBRAIN_LOAD_TEXT_ACTION_HPP_INCLUDED
#define MIBRAIN_LOAD_TEXT_ACTION_HPP_INCLUDED

#include <mitkDataNode.h>
#include <mitkIContextMenuAction.h>

class LoadTextAction : public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  static const QString DEFAULT_DIR;

  LoadTextAction()
    : m_DS(nullptr)
  {}

  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override;
  void SetDataStorage(mitk::DataStorage* ds) override { m_DS = ds; }
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

private:
  LoadTextAction(const LoadTextAction &);
  LoadTextAction & operator=(const LoadTextAction &);

  mitk::DataStorage* m_DS;
};

#endif // MIBRAIN_LOAD_TEXT_ACTION_HPP_INCLUDED
