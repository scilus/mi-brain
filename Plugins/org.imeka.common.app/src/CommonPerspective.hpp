
#ifndef PLITK_COMMON_DEFAULT_PERSPECTIVE_HPP_INCLUDED
#define PLITK_COMMON_DEFAULT_PERSPECTIVE_HPP_INCLUDED

#include <berryIPerspectiveFactory.h>

#include <QObject>

#include "org_imeka_common_app_Export.h"

struct COMMONAPP_EXPORT CommonPerspective
  : public QObject, public berry::IPerspectiveFactory
{
  Q_OBJECT
  Q_INTERFACES(berry::IPerspectiveFactory)

public:
  CommonPerspective() {}

  void CreateInitialLayout(berry::IPageLayout::Pointer layout);
};

#endif // PLITK_COMMON_DEFAULT_PERSPECTIVE_HPP_INCLUDED
