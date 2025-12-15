
#ifndef IMEKA_GEOMETRY_BOUNDING_BOX_UTILS_HPP_INCLUDED
#define IMEKA_GEOMETRY_BOUNDING_BOX_UTILS_HPP_INCLUDED

#include <itkFixedArray.h>

#include <mitkBaseGeometry.h> // For mitk::BoundingBox

#include <ImekaGeometryExports.h>

namespace mitk { class BaseGeometry; class DataStorage; }

namespace Imeka
{

namespace Geometry
{

itk::FixedArray<double, 6> ImekaGeometry_EXPORT
GetBoundingBox(mitk::BaseGeometry*);

mitk::BoundingBox::BoundsArrayType ImekaGeometry_EXPORT
GetSceneBounds(mitk::DataStorage*);

class ImekaGeometry_EXPORT ReinitOnBoundsChangeScopeGuard
{
public:
  ReinitOnBoundsChangeScopeGuard(mitk::DataStorage*);
  ~ReinitOnBoundsChangeScopeGuard();

private:
  mitk::DataStorage* m_DS;
  mitk::BoundingBox::BoundsArrayType m_StartingBounds;
};

} // namespace Geometry

} // namespace Imeka

#endif // IMEKA_GEOMETRY_BOUNDING_BOX_UTILS_HPP_INCLUDED

