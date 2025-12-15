
#ifndef IMEKA_FIBER_GLYPH_TRACKING_MASK_HPP_INCLUDED
#define IMEKA_FIBER_GLYPH_TRACKING_MASK_HPP_INCLUDED

#include "Glyph.hpp"

#include "RawMemoryLoader.hpp"

#include "ImekaFiberExports.h"

namespace Imeka
{

namespace Fiber
{

class ImekaFiber_EXPORT TrackingMask
  : public Glyph<float, 3, RawMemoryLoader<float, 3>>
{
public:
  TrackingMask(const float& minValue);
  ~TrackingMask() {}

  bool WithinMapThreshold(const unsigned long offset) const;

private:
  const float& m_MinValue;
};

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_GLYPH_TRACKING_MASK_HPP_INCLUDED
