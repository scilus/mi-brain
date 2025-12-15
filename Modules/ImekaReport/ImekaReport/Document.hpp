
#ifndef IMEKA_REPORT_DOCUMENT_HPP_INCLUDED
#define IMEKA_REPORT_DOCUMENT_HPP_INCLUDED

#include <QPixmap>
#include <QString>
#include <QStringList>

#include "ImekaReportExports.h"

namespace Imeka
{

namespace Report
{

enum ImageType { GrayValues = 1, Color = 3, ColorAlpha = 4 };

struct Parameters
{
  std::string fontName;
  unsigned int fontSize;
  float marginX, marginY;
  char separator;

  Parameters()
    : fontName("Helvetica")
    , fontSize(10)
    , marginX(50)
    , marginY(50)
    , separator(',')
  {}
};

class ImekaReport_EXPORT Document
{
public:
  Document();
  virtual ~Document();

  virtual void SetFont(const std::string &, const float) = 0;
  virtual void AddNewPage() = 0;

  virtual void AddCompanyHeader() = 0;

  virtual void WriteEmptyLine() = 0;
  virtual void WriteText(const QString &text) = 0;
  virtual void WriteCenteredText(const QString &text) = 0;
  virtual void WriteParagraph(const QString &text) = 0;

  virtual void InsertTable(
    const QString &title,
    const unsigned int nbRows,
    const QStringList &content) = 0;

  virtual void InsertTable(
    const QString &title,
    const float *begin, const float *end,
    const unsigned int nbRows,
    const QStringList &content) = 0;

  virtual void InsertImageFromBuffer(
    const unsigned char *imageData,
    const unsigned int width, const unsigned int height,
    const ImageType type, const float scale) = 0;

  template <class Pixel>
  void InsertImageFromMemory(
    const Pixel* imageData,
    const unsigned int width, const unsigned int height,
    const ImageType type, const float scale = 1.0)
  {
    InsertImageFromBuffer(
      reinterpret_cast<const unsigned char *>(imageData),
      width, height, type, scale);
  }

  virtual void InsertImageFromFile(
    const std::string &filename,
    const float scale = 1.0) = 0;

  virtual void InsertQtImage(const QPixmap &, const float = 1.0) = 0;

  virtual const char* GetExtension() const = 0;
  virtual const char* GetFilter() const = 0;
  virtual void Save(const std::string &filename) const = 0;

protected:
  static const QString m_HeaderText;
};

} // namespace Report

} // namespace Imeka

#endif // IMEKA_REPORT_DOCUMENT_HPP_INCLUDED

