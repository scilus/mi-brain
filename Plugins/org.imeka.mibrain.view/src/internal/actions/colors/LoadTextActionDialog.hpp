#ifndef LOAD_TEXT_ACTION_DIALOG_HPP_INCLUDED
#define LOAD_TEXT_ACTION_DIALOG_HPP_INCLUDED

#include <QDialog>
#include <vtkFloatArray.h>

#include "ui_LoadTextActionDialog.h"

class LoadTextActionDialog : public QDialog, public Ui::Dialog
{
  Q_OBJECT

public:
  LoadTextActionDialog(
    QWidget* parent,
    const std::vector<float>& arr
  );
  vtkSmartPointer<vtkFloatArray> GetArray() const;

private:
  std::pair<float, float> IQR(const std::vector<float>& arr) const;
  void ReadMinMax(float& min, float& max) const;
  void Process();

  const std::vector<float>& m_RawValues;
  const std::vector<float> m_Values;
  const std::pair<float, float> m_ArrMinMax;
  const std::pair<float, float> m_IQRMinMax;
};


#endif // LOAD_TEXT_ACTION_DIALOG_HPP_INCLUDED
