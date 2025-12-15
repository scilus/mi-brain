
#ifndef IMEKA_REPORT_CSV_HPP_INCLUDED
#define IMEKA_REPORT_CSV_HPP_INCLUDED

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <boost/algorithm/string/join.hpp>
#include <boost/lexical_cast.hpp>

#include <QStringList>

#include "Document.hpp"

#include "ImekaReportExports.h"

namespace Imeka
{

namespace Report
{

class ImekaReport_EXPORT CSVWriter : public Document
{
  public:
    static const char* EXTENSION;
    static const char* FILTER;

    CSVWriter(const char sep = ',');

    void WriteHeader(const QStringList &strings)
    {
      m_CSV << strings.join(QString(m_SEP)).toStdString() << std::endl;
    }

    template <class IT>
    void WriteHeader(const IT begin, const IT end)
    {
      std::vector<std::string> strings;
      for (IT it = begin; it != end; ++it)
      {
        strings.push_back(boost::lexical_cast<std::string>(*it));
      }

      m_CSV << boost::algorithm::join(
        strings, std::string() + m_SEP) << std::endl;
    }

    template <class IT>
    void WriteArray(const IT begin, const IT end)
    {
      WriteHeader(begin, end);
    }

    template <class IT, class Func>
    void WriteStruct(const IT &begin, const IT &end, Func toString)
    {
      for (IT it = begin; it != end; ++it)
      {
        const std::vector<std::string> line = toString(*it);
        m_CSV << boost::algorithm::join(
          line, std::string() + m_SEP) << std::endl;
      }
    }

    template <class Container, class Func>
    void WriteStruct(const Container &container, Func toString)
    {
      WriteStruct(container.begin(), container.end(), toString);
    }

    virtual void SetFont(const std::string &, const float) {}
    virtual void AddNewPage();
    virtual void AddCompanyHeader() {}

    virtual void WriteEmptyLine();
    virtual void WriteText(const QString &);

    template<typename T>
    void WriteText(const T& text)
    {
      m_CSV << text;
    }

    virtual void WriteCenteredText(const QString &);
    virtual void WriteParagraph(const QString &);

    virtual void InsertTable(
      const QString &, const unsigned int, const QStringList &);
    virtual void InsertTable(
      const QString &, const float *, const float *,
      const unsigned int, const QStringList &);

    virtual void InsertImageFromBuffer(
      const unsigned char *, const unsigned int,
      const unsigned int, const ImageType, const float) {}
    virtual void InsertImageFromFile(const std::string &, const float = 1.0) {}
    virtual void InsertQtImage(const QPixmap &, const float = 1.0) {}

    virtual const char* GetExtension() const;
    virtual const char* GetFilter() const;
    virtual void Save(const std::string &) const;

    template<typename T>
    CSVWriter& operator<<(const T& str)
    {
      WriteText(str);
      return *this;
    }

  private:
    std::ostringstream m_CSV;
    const char m_SEP;
};
} // namespace Report

} // namespace Imeka

#endif // IMEKA_REPORT_CSV_HPP_INCLUDED

