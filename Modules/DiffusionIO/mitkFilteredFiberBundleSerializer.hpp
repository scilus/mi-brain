#ifndef IMEKA_FILTERED_FIBER_BUNDLE_SERIALIZER_HPP_INCLUDED
#define IMEKA_FILTERED_FIBER_BUNDLE_SERIALIZER_HPP_INCLUDED

#include "mitkBaseDataSerializer.h"

namespace mitk
{

class FilteredFiberBundleSerializer : public BaseDataSerializer
{
  public:
    mitkClassMacro(FilteredFiberBundleSerializer, BaseDataSerializer);
    itkFactorylessNewMacro(Self)
    itkCloneMacro(Self)
    virtual std::string Serialize() override;
  protected:
    FilteredFiberBundleSerializer();
    virtual ~FilteredFiberBundleSerializer() override;
};

} // namespace mitk

#endif // IMEKA_FILTERED_FIBER_BUNDLE_SERIALIZER_HPP_INCLUDED
