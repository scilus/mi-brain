
#ifndef IMEKA_FIBER_GLYPH_RAW_MEMORY_LOADER_HPP_INCLUDED
#define IMEKA_FIBER_GLYPH_RAW_MEMORY_LOADER_HPP_INCLUDED

#include <vector>

namespace Imeka
{

namespace Fiber
{

template <class T, unsigned int D>
class RawMemoryLoader
{
private:
  typedef itk::Image<T, D> ImageType;

public:
  RawMemoryLoader() {}
  ~RawMemoryLoader() {};

  std::vector<T> Load(
    typename ImageType::Pointer im,
    const unsigned int nbElements)
  {
    return std::vector<T>(
      im->GetBufferPointer(),
      im->GetBufferPointer() + nbElements);
  }
};

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_GLYPH_RAW_MEMORY_LOADER_HPP_INCLUDED

