
#include "CSVWriter.hpp"

namespace Imeka
{

namespace Report
{

const char* CSVWriter::EXTENSION = ".csv";
const char* CSVWriter::FILTER = "Coma Separated Value file (*.csv)";

CSVWriter::CSVWriter(const char sep)
  : m_SEP(sep)
{}

void CSVWriter::AddNewPage()
{
  m_CSV << "\n\n\n";
}

void CSVWriter::WriteEmptyLine()
{
  m_CSV << "\n";
}

void CSVWriter::WriteText(const QString &text)
{
  m_CSV << text.toStdString();
}

void CSVWriter::WriteCenteredText(const QString &text)
{
  const std::string centered(20, ' ');
  m_CSV << centered << text.toStdString() << "\n\n";
}

void CSVWriter::WriteParagraph(const QString &text)
{
  WriteText(text + "\n\n");
}

void CSVWriter::InsertTable(
  const QString &title,
  const unsigned int nbCols,
  const QStringList &content)
{
  WriteText("Title: " + title + "\n");

  unsigned int i = 0;
  foreach (const QString &qStr, content)
  {
    m_CSV << qStr.toStdString() << m_SEP;
    if (++i % nbCols == 0) { m_CSV << "\n"; }
  }
  WriteEmptyLine();
}

void CSVWriter::InsertTable(
  const QString &title,
  const float *, const float *,
  const unsigned int nbRows,
  const QStringList &content)
{
  InsertTable(title, content.size() / nbRows, content);
}

const char* CSVWriter::GetExtension() const
{
  return EXTENSION;
}

const char* CSVWriter::GetFilter() const
{
  return FILTER;
}

void CSVWriter::Save(const std::string &filename) const
{
  std::ofstream file(filename.c_str(), std::ios::trunc);
  file << m_CSV.str();
}

}

}

