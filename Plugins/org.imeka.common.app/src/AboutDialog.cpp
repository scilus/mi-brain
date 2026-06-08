#include "AboutDialog.hpp"
#include "PlitkVersion.hpp"

AboutDialog::AboutDialog(QWidget* parent)
: QDialog(parent)
{
  setupUi(this);
  setWindowTitle("UdeS - SCIL");
  Version(PLITK_VERSION);
  MITKVersion(MITK_VERSION);

  // Remove the help button
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void AboutDialog::Version(const char* version)
{
  imekaVersionLabel->setText(version);
}

void AboutDialog::MITKVersion(const char* version)
{
  mitkVersionLabel->setText(version);
}

