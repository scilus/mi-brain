
#ifndef IMEKA_FIBER_GLYPH_MAXIMA_HPP_INCLUDED
#define IMEKA_FIBER_GLYPH_MAXIMA_HPP_INCLUDED

#include "Glyph.hpp"

#include "VectorLoader.hpp"

#include "ImekaFiberExports.h"

namespace Imeka
{

namespace Fiber
{

class ImekaFiber_EXPORT Maxima
  : public Glyph<float, 4, VectorLoader<float, 4>, V>
{
public:
  virtual void SetImage(const mitk::Image* im);
  bool ValidOffset(const unsigned long offset) const;
  V PickDirection(const unsigned long offset) const;
  V AdvecIntegrate(
    const V& direction,
    const unsigned long offset,
    const mitk::ScalarType fa,
    const double& puncture) const;

  mitk::FilteredFiberBundle::Pointer ToFiberBundle();

private:
  mitk::FilteredFiberBundle::Pointer m_PeaksFibers;
};

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_GLYPH_MAXIMA_HPP_INCLUDED
