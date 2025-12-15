
#ifndef IMEKA_COMMON_DATANODE_GROUP_HPP_INCLUDED
#define IMEKA_COMMON_DATANODE_GROUP_HPP_INCLUDED

#include <mitkBaseData.h>

#include "ImekaWidgetsExports.h"

namespace Imeka
{

namespace Common
{

class ImekaWidgets_EXPORT DataNodeGroup : public mitk::BaseData
{
public:
  mitkClassMacro(DataNodeGroup, mitk::BaseData);
  itkFactorylessNewMacro(Self)
  itkCloneMacro(Self)

  virtual void InitializeTimeGeometry(unsigned int timeSteps) override;

protected:
  DataNodeGroup();

  virtual bool VerifyRequestedRegion() override { return true; }
  virtual bool RequestedRegionIsOutsideOfTheBufferedRegion() override { return false; }
  virtual void SetRequestedRegionToLargestPossibleRegion() override {}
  virtual void SetRequestedRegion(const itk::DataObject * /*data*/) override {}
};

} // namespace Common

} // namespace Imeka

#endif // IMEKA_WIDGETS_REGION_OF_INTEREST_HPP_INCLUDED
