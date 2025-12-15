
#ifndef TIME_SEQUENCE_DIALOG_HPP_INCLUDED
#define TIME_SEQUENCE_DIALOG_HPP_INCLUDED

#include <QDialog>

namespace Ui {
  class MakeTimedSequenceDialog;
}

class MakeTimedSequenceDialog : public QDialog
{
  Q_OBJECT

public:
  explicit MakeTimedSequenceDialog(QWidget *parent = 0);
  ~MakeTimedSequenceDialog();

  QString getName();
  bool getEraseAll();

private:
  Ui::MakeTimedSequenceDialog *ui;
};

#endif // TIME_SEQUENCE_DIALOG_HPP_INCLUDED
