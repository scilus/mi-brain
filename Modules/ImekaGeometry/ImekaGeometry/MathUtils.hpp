
#ifndef IMEKA_GEOMETRY_MATH_UTILS_HPP_INCLUDED
#define IMEKA_GEOMETRY_MATH_UTILS_HPP_INCLUDED

#include <limits>

#include <mitkVector.h>

template <class T>
bool FPEqual(
  const T fp1, const T fp2,
  const T epsilon = std::numeric_limits<T>::epsilon())
{
  if (fp1 < fp2)
  {
    return (fp2 - fp1) <= epsilon;
  }
  return (fp1 - fp2) <= epsilon;
}

// Utility function to return a normalized Vector3D because
// mitk::Vector3D::Normalize() returns void.
template <class T, unsigned int Dimension>
itk::Vector<T, Dimension>
Normalize(const itk::Vector<T, Dimension> &v)
{
  itk::Vector<T, Dimension> origin; origin.Fill(T());
  itk::Vector<T, Dimension> ret = v;
  if (v != origin)
  {
    ret.Normalize();
  }
  return ret;
}

inline mitk::Vector2D Vector2D(
  const mitk::ScalarType x,
  const mitk::ScalarType y)
{
  mitk::Vector2D v;
  v[0] = x;
  v[1] = y;
  return v;
}

inline mitk::Vector3D Vector3D(
  const mitk::ScalarType x,
  const mitk::ScalarType y,
  const mitk::ScalarType z)
{
  mitk::Vector3D v;
  v[0] = x;
  v[1] = y;
  v[2] = z;
  return v;
}

inline mitk::Point3D Point3D(
  const mitk::ScalarType x,
  const mitk::ScalarType y,
  const mitk::ScalarType z)
{
  mitk::Point3D p;
  p[0] = x;
  p[1] = y;
  p[2] = z;
  return p;
}

#endif
