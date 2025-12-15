#ifndef IMEKA_APP_ABOUT_HANDLER_HPP_INCLUDED
#define IMEKA_APP_ABOUT_HANDLER_HPP_INCLUDED

#include <berryAbstractHandler.h>

#include "org_imeka_common_app_Export.h"

// To work, the commandId "org.blueberry.ui.help.aboutAction" must be linked
// this class in a plugin.xml. We can't use signal and slot as usual.
class COMMONAPP_EXPORT AboutHandler : public berry::AbstractHandler
{
  Q_OBJECT

public:
  Object::Pointer Execute(
    const berry::SmartPointer<const berry::ExecutionEvent>&) override;
};

#endif // IMEKA_APP_ABOUT_HANDLER_HPP_INCLUDED
