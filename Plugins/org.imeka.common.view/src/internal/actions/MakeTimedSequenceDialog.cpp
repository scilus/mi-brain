
#include "MakeTimedSequenceDialog.hpp"

#include "ui_MakeTimedSequenceDialog.h"

MakeTimedSequenceDialog::MakeTimedSequenceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MakeTimedSequenceDialog)
{
    ui->setupUi(this);
}

MakeTimedSequenceDialog::~MakeTimedSequenceDialog()
{
    delete ui;
}

QString MakeTimedSequenceDialog::getName()
{
  return ui->EditName->text();
}

bool MakeTimedSequenceDialog::getEraseAll()
{
  return (ui->chkEraseAll->checkState() == Qt::Checked);
}
