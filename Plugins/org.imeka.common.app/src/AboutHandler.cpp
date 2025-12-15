#include "AboutHandler.hpp"

#include "AboutDialog.hpp"
#include "PlitkVersion.hpp"

berry::Object::Pointer AboutHandler::Execute(
  const berry::SmartPointer<const berry::ExecutionEvent>&)
{
  auto about = new AboutDialog(QApplication::activeWindow());
  about->Version(PLITK_VERSION);
  about->MITKVersion(MITK_VERSION);
  about->open();
  return berry::Object::Pointer();
}
