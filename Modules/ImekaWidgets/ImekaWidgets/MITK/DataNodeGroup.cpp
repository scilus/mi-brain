
#include "DataNodeGroup.hpp"

#include <mitkPlaneGeometry.h>
#include <mitkProportionalTimeGeometry.h>

namespace Imeka
{

namespace Common
{

DataNodeGroup::DataNodeGroup()
{
  // DataNodeGroup must at least have a geometry or mitkIOUtils will
  // refuse to save it when it saves the scene
  this->Expand(1);
}

void DataNodeGroup::InitializeTimeGeometry(unsigned int timeSteps)
{
  auto geometry2D = mitk::PlaneGeometry::New();
  geometry2D->Initialize();

  // The geometry is propagated automatically to all time steps,
  // if EvenlyTimed is true...
  auto timeGeometry = mitk::ProportionalTimeGeometry::New();
  timeGeometry->Initialize(geometry2D, timeSteps);
  SetTimeGeometry(timeGeometry);
}

} // namespace Common

} // namespace Imeka
