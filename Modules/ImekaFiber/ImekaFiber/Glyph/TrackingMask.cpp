
#include "TrackingMask.hpp"

namespace Imeka
{

namespace Fiber
{

TrackingMask::TrackingMask(const float& minValue)
  : m_MinValue(minValue)
{}

bool TrackingMask::WithinMapThreshold(const unsigned long offset) const
{
  return offset < m_Glyph.size()
    && m_Glyph[offset] > m_MinValue;
}

} // namespace Fiber

} // namespace Imeka

