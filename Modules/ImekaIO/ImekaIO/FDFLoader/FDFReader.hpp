
#ifndef IMEKA_IO_FDF_READER_HPP_INCLUDED
#define IMEKA_IO_FDF_READER_HPP_INCLUDED

#include <boost/lexical_cast.hpp>

#include <mitkAbstractFileReader.h>
#include <mitkException.h>

#include "ImekaIOExports.h"

namespace mitk { class RawImageFileReader; }

namespace Imeka
{

namespace IO
{

class ImekaIO_EXPORT FDFReader : public mitk::AbstractFileReader
{
private:
  static const unsigned int SPACING_TO_MM_RATIO;
  static const unsigned int TOTAL_ELEM_IN_3x3_MATRIX;
  static const unsigned int ELEM_3x3_MATRIX;

  class InvalidGeometry : public mitk::Exception
  {
    public: mitkExceptionClassMacro(InvalidGeometry, mitk::Exception);
  };

  struct FDFHeader
  {
    float rank;
    std::string spatial_rank;
    std::string storage;
    float bits;
    std::string type;
    std::vector<float> matrix;
    std::vector<std::string> abscissa;
    std::vector<std::string> ordinate;
    std::vector<float> span;
    std::vector<float> origin;
    std::vector<std::string> nucleus;
    std::vector<float> nucfreq;
    std::vector<float> location;
    std::vector<float> roi;
    float gap; // Covance
    std::string file;
    int slice_no;
    int slices;
    int echo_no;
    int echoes;
    float TE;
    float te;
    float TR;
    float tr;
    int ro_size;
    int pe_size;
    std::string sequence;
    std::string studyid;
    std::string position1;
    std::string position2;
    float TI;
    float ti;
    int array_index;
    float array_dim;
    float image;
    int display_order;
    int bigendian; // Covance
    float imagescale; // Covance
    float psi;
    float phi;
    float theta;
    float dro; // Covance
    float dpe; // Covance
    float dsl; // Covance
    float bvalue; // Covance
    std::vector<float> orientation;
    int checksum;
  };

public:
  FDFReader();
  virtual ~FDFReader(){}
  FDFReader(const FDFReader& other);
  virtual FDFReader* Clone() const override;
  virtual std::vector<itk::SmartPointer<mitk::BaseData>> DoRead() override;

private:
  template<typename OutputContainer>
  void LexicalPush(
    std::vector<std::string>::const_iterator begin,
    std::vector<std::string>::const_iterator end,
    OutputContainer& out)
  {
    out.reserve(std::distance(begin, end));
    for (auto it = begin; it != end; ++it)
    {
      out.push_back(
        boost::lexical_cast<typename OutputContainer::value_type>(*it));
    }
  };

  us::ServiceRegistration<mitk::IFileReader> m_ServiceReg;

  FDFHeader ReadHeader(std::ifstream& inf);
  void SetHeaderOptions(
    const FDFHeader &header,
    mitk::IFileReader::Options& options);
};

} // namespace IO

} // namespace Imeka

#endif // IMEKA_IO_FDF_READER_HPP_INCLUDED

