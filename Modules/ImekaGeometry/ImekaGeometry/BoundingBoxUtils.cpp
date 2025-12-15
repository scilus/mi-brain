
#include "BoundingBoxUtils.hpp"

#include <algorithm>

#include <mitkDataStorage.h>
#include <mitkGeometry3D.h>
#include <mitkNodePredicateNot.h>
#include <mitkNodePredicateProperty.h>

namespace Imeka
{

namespace Geometry
{

itk::FixedArray<double, 6> ImekaGeometry_EXPORT
GetBoundingBox(mitk::BaseGeometry* geometry)
{
  double
    minX = 99999.0, maxX = -99999.0,
    minY = 99999.0, maxY = -99999.0,
    minZ = 99999.0, maxZ = -99999.0;
  for (unsigned int i = 0; i < 2; ++i)
  {
    for (unsigned int j = 0; j < 2; ++j)
    {
      for (unsigned int k = 0; k < 2; ++k)
      {
        const auto corner = geometry->GetCornerPoint(i, j, k);
        minX = std::min(minX, corner[0]); maxX = std::max(maxX, corner[0]);
        minY = std::min(minY, corner[1]); maxY = std::max(maxY, corner[1]);
        minZ = std::min(minZ, corner[2]); maxZ = std::max(maxZ, corner[2]);
      }
    }
  }

  itk::FixedArray<double, 6> bbox;
  bbox[0] = minX; bbox[1] = maxX;
  bbox[2] = minY; bbox[3] = maxY;
  bbox[4] = minZ; bbox[5] = maxZ;
  return bbox;
}

mitk::BoundingBox::BoundsArrayType ImekaGeometry_EXPORT
GetSceneBounds(mitk::DataStorage* DS)
{
  auto usefulObjects = mitk::NodePredicateNot::New(
    mitk::NodePredicateProperty::New("includeInBoundingBox",
    mitk::BoolProperty::New(false)));
  const auto nodes = DS->GetSubset(usefulObjects);
  const auto boundsGeo = DS->ComputeBoundingGeometry3D(nodes, "visible");
  return boundsGeo->GetBoundsInWorld();
}

ReinitOnBoundsChangeScopeGuard::ReinitOnBoundsChangeScopeGuard(
  mitk::DataStorage* DS)
  : m_DS(DS)
  , m_StartingBounds(GetSceneBounds(DS))
{}

ReinitOnBoundsChangeScopeGuard::~ReinitOnBoundsChangeScopeGuard()
{
  const auto currentBounds = GetSceneBounds(m_DS);
  if (m_StartingBounds != currentBounds)
  {
    // GlobalReinit
    mitk::RenderingManager::GetInstance()
      ->InitializeViewsByBoundingObjects(m_DS);
  }
}

} // namespace Geometry

} // namespace Imeka
