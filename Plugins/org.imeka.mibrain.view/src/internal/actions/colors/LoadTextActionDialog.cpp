#include "LoadTextActionDialog.hpp"

LoadTextActionDialog::LoadTextActionDialog(
  QWidget * parent,
  const std::vector<float>& arr
)
  : m_RawValues(arr)
  , m_Values([arr]() {
    auto values = arr;
    std::sort(values.begin(), values.end());
    return values;
  }())
  , m_ArrMinMax(std::make_pair(m_Values[0], m_Values[m_Values.size() - 1]))
  , m_IQRMinMax(IQR(m_Values))
{
  Q_UNUSED(parent);

  setupUi(this);

  this->lblNbLoaded->setText(
    QString("%1 values loaded ").arg(m_RawValues.size()));
  this->dsbMin->setMinimum(m_ArrMinMax.first);
  this->dsbMax->setMaximum(m_ArrMinMax.second);

  connect(this->rbManual, &QAbstractButton::toggled, [this]() {
    this->dsbMin->setEnabled(true);
    this->dsbMax->setEnabled(true);
    Process();
  });
  connect(this->rbIQR, &QAbstractButton::toggled, [this]() {
    this->dsbMin->setDisabled(true);
    this->dsbMax->setDisabled(true);
    this->dsbMin->setValue(m_IQRMinMax.first);
    this->dsbMax->setValue(m_IQRMinMax.second);
  });
  connect(this->rbRaw, &QAbstractButton::toggled, [this]() {
    this->dsbMin->setDisabled(true);
    this->dsbMax->setDisabled(true);
    this->dsbMin->setValue(m_ArrMinMax.first);
    this->dsbMax->setValue(m_ArrMinMax.second);
  });

  auto valueChangedDouble = QOverload<double>::of(&QDoubleSpinBox::valueChanged);
  connect(this->dsbMin, valueChangedDouble, [this]() {
    Process();
  });
  connect(this->dsbMax, valueChangedDouble, [this]() {
    Process();
  });

  this->rbRaw->setChecked(true);
}

std::pair<float, float> LoadTextActionDialog::IQR(
  const std::vector<float>& arr) const
{
  float q1 = 0.0;
  float q3 = 0.0;
  const auto half = static_cast<unsigned int>(arr.size()) / 2;
  const auto half_half = half / 2;
  if (arr.size() % 2 == 0)
  {
    // 1 2 3 4 5
    //   |   |
    q1 = arr[half - half_half];
    q3 = arr[half + half_half];
  }
  else
  {
    // 1 2 3 4
    //  |   |
    q1 = (arr[half - half_half] + arr[half - half_half + 1]) / 2.0;
    q3 = (arr[half + half_half] + arr[half + half_half + 1]) / 2.0;
  }
  const auto iqr = q3 - q1;
  return std::make_pair(q1 - 1.5 * iqr, q3 + 1.5 * iqr);
}

void LoadTextActionDialog::ReadMinMax(float& min, float& max) const
{
  // We can use the spinBoxes value only on Manual here otherwise the
  // precision can be horrible.
  min = this->dsbMin->value();
  max = this->dsbMax->value();
  if (this->rbIQR->isChecked())
  {
    min = m_IQRMinMax.first;
    max = m_IQRMinMax.second;
  }
  else if (this->rbRaw->isChecked())
  {
    min = m_ArrMinMax.first;
    max = m_ArrMinMax.second;
  }
}

void LoadTextActionDialog::Process()
{
  float min, max;
  ReadMinMax(min, max);

  unsigned int nbBelow = 0;
  unsigned int nbAbove = 0;
  for (auto v : m_Values)
  {
    if (v < min)
    {
      nbBelow++;
    }
    else if (v > max)
    {
      nbAbove++;
    }
  }

  this->lblLowClamp->setText(QString("%1 values clamped").arg(nbBelow));
  this->lblHighClamp->setText(QString("%1 values clamped").arg(nbAbove));
}

vtkSmartPointer<vtkFloatArray> LoadTextActionDialog::GetArray() const
{
  float min, max;
  ReadMinMax(min, max);

  auto arr = vtkSmartPointer<vtkFloatArray>::New();
  arr->Allocate(m_RawValues.size());
  for (auto v : m_RawValues)
  {
    if (v < min)
    {
      arr->InsertNextValue(min);
    }
    else if (v > max)
    {
      arr->InsertNextValue(max);
    }
    else
    {
      arr->InsertNextValue(v);
    }
  }
  return arr;
}
