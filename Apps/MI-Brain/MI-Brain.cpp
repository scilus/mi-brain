
#include <mitkBaseApplication.h>
  
#include <QVariant>

int main(int argc, char** argv)
{
  // Create a QApplication instance first
  mitk::BaseApplication app(argc, argv);
  app.setApplicationName("MI-Brain");
  app.setOrganizationName("Imeka");

  // Preload the org.mitk.gui.qt.ext plug-in (and hence also QmitkExt) to speed
  // up a clean-cache start. This also works around bugs in older gcc and glibc implementations,
  // which have difficulties with multiple dynamic opening and closing of shared libraries with
  // many global static initializers. It also helps if dependent libraries have weird static
  // initialization methods and/or missing de-initialization code.
  QStringList preloadLibs;
  preloadLibs << "liborg_mitk_gui_qt_ext";
  app.setPreloadLibraries(preloadLibs);

  app.setProperty(
    mitk::BaseApplication::PROP_APPLICATION,
    "org.imeka.mibrain.app");
  return app.run();
}
