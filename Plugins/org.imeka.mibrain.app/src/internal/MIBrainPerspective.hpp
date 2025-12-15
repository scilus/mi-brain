
#ifndef MI_BRAIN_PERSPECTIVE_HPP_INCLUDED
#define MI_BRAIN_PERSPECTIVE_HPP_INCLUDED

#include <QObject>
#include <berryIPerspectiveFactory.h>

struct MIBrainPerspective
  : public QObject, public berry::IPerspectiveFactory
{
  Q_OBJECT
  Q_INTERFACES(berry::IPerspectiveFactory)

public:
  MIBrainPerspective() {}

  void CreateInitialLayout(berry::IPageLayout::Pointer layout) override;
};

#endif // MI_BRAIN_PERSPECTIVE_HPP_INCLUDED
