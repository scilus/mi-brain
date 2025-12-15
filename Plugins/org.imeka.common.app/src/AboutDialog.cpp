#include "AboutDialog.hpp"

AboutDialog::AboutDialog(QWidget* parent)
: QDialog(parent)
{
  setupUi(this);
  setWindowTitle("Imeka Solutions Inc.");

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

