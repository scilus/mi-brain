#ifndef IMEKA_FIBER_RTT_HPP_INCLUDED
#define IMEKA_FIBER_RTT_HPP_INCLUDED

#include <limits>
#include <vector>

#include <mitkImage.h>

#include <vtkSmartPointer.h>

#include "ImekaCommon/types.hpp"
#include "Glyph/Maxima.hpp"
#include "Glyph/TrackingMask.hpp"
#include "types.hpp"

#include "ImekaFiberExports.h"

namespace Imeka
{

namespace Fiber
{

enum RTTMode {DTI, HARDI};

class ImekaFiber_EXPORT RTT
{
public:
  static struct Parameters
  {
    float minFA;
    double maxAngle;
    double step;
    double puncture;
    double minLength, maxLength;
    unsigned int nbSeeds;
  } parameters;
  static TrackingMask trackingMask;

  RTT();

  void SetRTTFibers(const mitk::FilteredFiberBundle* fibers);
  void Clear();
  void SetTrackingMaskGeometry(const mitk::BaseGeometry*);
  void SetTensorsData(const mitk::Image* im);
  void SetMaxima(Maxima* maxima) { m_Maxima = maxima; }

  void SeedFromSelectionObject(const SelectionObject* selectionObject);
  void SeedFromMask(const mitk::Image* image);

private:
  enum Direction { Backward = -1, Forward = 1 };

  void Init(const unsigned int nbPossibleFibers);
  bool BoxIsInAnat(mitk::BaseGeometry* boxGeometry) const;

  void Seed(const mitk::Point3D& seed);
  void ComputeHARDI(const mitk::Point3D&, const Direction, const V&);

  bool ValidPosition(
    const unsigned long maximaOffset,
    const unsigned long tmOffset)
  {
    if (maximaOffset == std::numeric_limits<unsigned long>::max()
      || tmOffset == std::numeric_limits<unsigned long>::max())
    { return false; }

    return m_Maxima->ValidOffset(maximaOffset)
      && trackingMask.WithinMapThreshold(tmOffset);
  }

  // Implemented in hpp to increase chance of inlining
  V GetNextDirection(
    const V& currentDirection,
    const unsigned long maximaOffset,
    const unsigned long tmOffset,
    const Direction backwardForward)
  {
    V nextDirection = m_Maxima->AdvecIntegrate(
      currentDirection, maximaOffset,
      trackingMask.Get(tmOffset),
      parameters.puncture);
    nextDirection *= backwardForward;
    if (currentDirection * nextDirection < 0.0)
    {
      // Ensures the two vectors have the same directions
      nextDirection *= -1.0;
    }
    return nextDirection;
  }

  const mitk::FilteredFiberBundle* m_RTTFibers;
  vtkPoints* m_Points;
  vtkCellArray* m_Lines;
  vtkUnsignedCharArray* m_Colors;
  std::vector<itk::RGBPixel<unsigned char>> m_TmpColors;
  std::vector<mitk::Point3D> m_TmpPoints;

  const mitk::BaseGeometry* m_TrackingMaskGeo;
  unsigned int m_NbPointsToExceedMinLength;
  unsigned int m_NbPointsToExceedMaxLength;
  unsigned int m_LastNbFibers;

  const Maxima* m_Maxima;
};

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_RTT_HPP_INCLUDED
