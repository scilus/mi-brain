
#ifndef IMEKA_MAKE_TIMED_SEQUENCE_ACTION_HPP_INCLUDED
#define IMEKA_MAKE_TIMED_SEQUENCE_ACTION_HPP_INCLUDED

#include <QObject>

#include <mitkIContextMenuAction.h>

class MakeTimedSequenceAction : public QObject, public mitk::IContextMenuAction
{
  Q_OBJECT
  Q_INTERFACES(mitk::IContextMenuAction)

public:
  MakeTimedSequenceAction()
    : m_DS(nullptr)
  {}

  void Run(const QList<mitk::DataNode::Pointer> &selectedNodes) override;
  void SetDataStorage(mitk::DataStorage* ds) override { m_DS = ds; }
  void SetSmoothed(bool) override {}
  void SetDecimated(bool) override {}
  void SetFunctionality(berry::QtViewPart*) override {}

private:
  MakeTimedSequenceAction(const MakeTimedSequenceAction &);
  MakeTimedSequenceAction & operator=(const MakeTimedSequenceAction &);

  mitk::DataStorage* m_DS;
};

#endif // IMEKA_MAKE_TIMED_SEQUENCE_ACTION_HPP_INCLUDED

