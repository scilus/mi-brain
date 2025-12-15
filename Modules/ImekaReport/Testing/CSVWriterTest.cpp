
#include <mitkTestingMacros.h>

#include <boost/tokenizer.hpp>

#include <QDir>
#include <QFile>
#include <QString>
#include <QStringList>

#include <cmath>
#include <iomanip>
#include <sstream>
#include <vector>

#include "ImekaReport/CSVWriter.hpp"

template <class T>
std::string lexical_cast(const T &val, const unsigned int precision = 0)
{
  std::stringstream ss;
  if (precision)
  {
    ss << std::setprecision(precision);
  }
  ss << val;
  return ss.str();
}

class CSVWriterTest
{
  template <typename T>
  struct SameData { T i, j, k, l; };

  struct ComplexData { int i; std::string s; float f; double d; };
  struct ComplexDataWriter
  {
    std::vector<std::string> operator()(const ComplexData &data) const
    {
      std::vector<std::string> line;
      line.push_back(std::to_string(data.i));
      line.push_back(data.s);
      line.push_back(lexical_cast(data.f, 8));
      line.push_back(lexical_cast(data.f, 16));
      return line;
    }
  };

  static const unsigned int NB_ROWS = 5;
  static const unsigned int NB_COLS = 4;

public:

  static void TestCSVWriter(const char* path)
  {
    const QDir basePath(path);
    if (!basePath.exists())
    {
      MITK_TEST_FAILED_MSG(<<"Base path doesn't exist.")
    }

    const QString intTruth = "inttruth.csv";
    if (!basePath.exists(intTruth))
    {
      MITK_TEST_FAILED_MSG(<<"The int test groundtruth doesn't exist.")
    }
    TestSameData<int>(basePath.absoluteFilePath("inttest.csv"),
      basePath.absoluteFilePath(intTruth), ' ');

    const QString doubleTruth = "doubletruth.csv";
    if (!basePath.exists(doubleTruth))
    {
      MITK_TEST_FAILED_MSG(<<"The double test groundtruth doesn't exist.")
    }
    TestSameData<double>(basePath.absoluteFilePath("doubletest.csv"),
      basePath.absoluteFilePath(doubleTruth), '\t');

    const QString writePath = basePath.absoluteFilePath("complextest.csv");
    const QString readPath = basePath.absoluteFilePath("complextruth.csv");
    const double PI = 3.141592653589793238462643383279502884;

    std::vector<ComplexData> v;
    const ComplexData c1 = {1, "one", static_cast<float>(1.0f * PI), 1.0 * PI};
    const ComplexData c2 = {2, "two", static_cast<float>(2.0f * PI), 2.0 * PI};
    const ComplexData c3 = {3, "thr", static_cast<float>(3.0f * PI), 3.0 * PI};
    const ComplexData c4 = {4, "fou", static_cast<float>(4.0f * PI), 4.0 * PI};
    const ComplexData c5 = {5, "fiv", static_cast<float>(5.0f * PI), 5.0 * PI};
    v.push_back(c1); v.push_back(c2); v.push_back(c3);
    v.push_back(c4); v.push_back(c5);

    Imeka::Report::CSVWriter writer(' ');
    writer.WriteHeader(QStringList() << ":i:" << ":s:" << ":f:" << ":d:");
    writer.WriteStruct(v, ComplexDataWriter());
    writer.Save(writePath.toStdString());

    CompareFile(writePath, readPath, true);

  }

  template <typename T>
  static void TestSameData(
    const QString &writePath,
    const QString &readPath,
    const char sep)
  {
    // Generate some data
    std::vector<SameData<T> > v;
    for (unsigned int i = 0; i < NB_ROWS; ++i)
    {
      const SameData<T> row = {
        static_cast<T>(i),
        static_cast<T>(i + 1),
        static_cast<T>(i + 2),
        static_cast<T>(i + 4)};
      v.push_back(row);
    }

    // Write it
    Imeka::Report::CSVWriter writer(sep);
    writer.WriteHeader(QStringList() << ":i:" << ":j:" << ":k:" << ":l:");
    for (unsigned int i = 0; i < NB_ROWS; ++i)
    {
      const SameData<T> row = v[i];
      const T data[] = {row.i, row.j, row.k, row.l};
      writer.WriteArray(data, data + NB_COLS);
    }
    writer.Save(writePath.toStdString());

    CompareFile(writePath, readPath, false);
  }

  static void CompareFile(
    const QString &path1
    , const QString &path2
    , const bool isComplex)
  {
    typedef boost::tokenizer<boost::char_separator<char>,
      std::istreambuf_iterator<char> > tokenizer;
    typedef std::istreambuf_iterator<char> StreamIt;

    boost::char_separator<char> sep(" \t\n");

    std::ifstream f1(path1.toStdString().c_str());
    if (!f1.is_open())
    {
      MITK_TEST_FAILED_MSG(<<"Unable to open created file for reading.")
    }
    tokenizer tokens1(StreamIt(f1), StreamIt(), sep);

    std::ifstream f2(path2.toStdString().c_str());
    if (!f2.is_open())
    {
      MITK_TEST_FAILED_MSG(<<"Unable to open groundtruth file for reading.")
    }
    tokenizer tokens2(StreamIt(f2), StreamIt(), sep);

    tokenizer::iterator it1 = tokens1.begin();
    tokenizer::iterator it2 = tokens2.begin();
    for (unsigned int i = 0; i < NB_COLS; ++i)
    {
      if (*it1++ != *it2++)
      {
        MITK_TEST_FAILED_MSG(<<"Headers are different.")
      }
    }

    if (!isComplex)
    {
      for (unsigned int i = 0; i < NB_ROWS * NB_COLS; ++i)
      {
        if (*it1++ != *it2++)
        {
          MITK_TEST_FAILED_MSG(<<"Data is different.")
        }
      }
    }
    else
    {
      for (unsigned int i = 0; i < NB_ROWS; ++i)
      {
        const bool badInt = *it1++ != *it2++;
        const bool badString = *it1++ != *it2++;
        const bool badFloat = std::fabs(boost::lexical_cast<float>(*it1++)
          - boost::lexical_cast<float>(*it2++)) > 0.0000001;
        const bool badDouble = std::fabs(boost::lexical_cast<double>(*it1++)
          - boost::lexical_cast<double>(*it2++)) > 0.00000000000001;

        if (badInt || badString || badFloat || badDouble)
        {
          MITK_TEST_FAILED_MSG(<<"Data is different.")
        }
      }
    }
    if (it1 != tokens1.end() || it2 != tokens2.end())
    {
      MITK_TEST_FAILED_MSG(<<"Should be at end of file.")
    }
  }
};

int CSVWriterTest(int argc, char* argv[])
{
  MITK_TEST_BEGIN("CSVWriter")

  MITK_TEST_OUTPUT(<<argc)

  MITK_TEST_CONDITION_REQUIRED(argc == 2,
    "File to load has been specified on commandline");

  CSVWriterTest::TestCSVWriter(argv[1]);

  MITK_TEST_END()
}

