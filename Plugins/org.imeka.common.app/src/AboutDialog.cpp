#include "AboutDialog.hpp"

AboutDialog::AboutDialog(QWidget* parent)
: QDialog(parent)
{
  setupUi(this);
  setWindowTitle("UdeS - SCIL");
  Version("2026.12");
  MITKVersion("2026.04");

  // Remove the help button
  setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

void AboutDialog::Version(const char* version)
{
  mibrainVersionLabel->setText(version);
}

void AboutDialog::MITKVersion(const char* version)
{
  mitkVersionLabel->setText(version);
}

