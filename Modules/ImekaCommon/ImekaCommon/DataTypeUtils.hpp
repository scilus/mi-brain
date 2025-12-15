
#ifndef IMEKA_COMMON_DATA_TYPE_UTILS_HPP_INCLUDED
#define IMEKA_COMMON_DATA_TYPE_UTILS_HPP_INCLUDED

#include <mitkPixelType.h>

namespace Imeka
{

namespace Common
{

template<unsigned int Channels>
const mitk::PixelType GetMitkPixelType(const int vtkScalarType)
{
  switch (vtkScalarType)
  {
    case VTK_UNSIGNED_CHAR:
      return mitk::MakePixelType<unsigned char, unsigned char, Channels>();
    case VTK_SHORT:
      return mitk::MakePixelType<short, short, Channels>();
    case VTK_UNSIGNED_SHORT:
      return mitk::MakePixelType<unsigned short, unsigned short, Channels>();
    case VTK_INT:
      return mitk::MakePixelType<int, int, Channels>();
    case VTK_UNSIGNED_INT:
      return mitk::MakePixelType<unsigned int, unsigned int, Channels>();
    case VTK_LONG:
      return mitk::MakePixelType<long, long, Channels>();
    case VTK_UNSIGNED_LONG:
      return mitk::MakePixelType<unsigned long, unsigned long, Channels>();
    case VTK_FLOAT:
      return mitk::MakePixelType<float, float, Channels>();
    case VTK_DOUBLE:
      return mitk::MakePixelType<double, double, Channels>();

    case VTK_BIT: case VTK_CHAR: default:
      return mitk::MakePixelType<char, char, Channels>();
  }
}

const mitk::PixelType GetGrayscalePixelType(const int vtkScalarType)
{
  return GetMitkPixelType<1>(vtkScalarType);
}

const mitk::PixelType GetColorPixelType(const int vtkScalarType)
{
  return GetMitkPixelType<3>(vtkScalarType);
}

} // Common

} // Imeka

#endif // IMEKA_COMMON_DATA_TYPE_UTILS_HPP_INCLUDED
