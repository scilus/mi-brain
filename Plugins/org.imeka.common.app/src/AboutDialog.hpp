
#ifndef IMEKA_APP_ABOUT_DIALOG_HPP_INCLUDED
#define IMEKA_APP_ABOUT_DIALOG_HPP_INCLUDED

#include "ui_AboutDialog.h"

#include "org_imeka_common_app_Export.h"

class COMMONAPP_EXPORT AboutDialog
  : public QDialog, Ui::AboutDialog
{
  Q_OBJECT

public:
  explicit AboutDialog(QWidget* = nullptr);
  void Version(const char*);
  void MITKVersion(const char*);
};

#endif // IMEKA_APP_ABOUT_DIALOG_HPP_INCLUDED
