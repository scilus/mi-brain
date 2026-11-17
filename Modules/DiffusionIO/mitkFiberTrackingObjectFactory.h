/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#ifndef MITKFIBERTRACKINGOBJECTFACTORY_H
#define MITKFIBERTRACKINGOBJECTFACTORY_H

#include "mitkCoreObjectFactory.h"

#include "FiberBundle/mitkFiberBundle.h"
#include <mitkFiberBundleMapper3D.h>
#include <mitkFiberBundleMapper2D.h>
#include <mitkPeakImageMapper2D.h>
#include <mitkPeakImageMapper3D.h>

namespace mitk {

class FiberTrackingObjectFactory : public CoreObjectFactoryBase
{
  public:
    mitkClassMacro(FiberTrackingObjectFactory, CoreObjectFactoryBase)
    itkFactorylessNewMacro(Self)
    itkCloneMacro(Self)

    ~FiberTrackingObjectFactory() override;

    Mapper::Pointer CreateMapper(mitk::DataNode* node, MapperSlotId slotId) override;
    void SetDefaultProperties(mitk::DataNode* node) override;

protected:
    FiberTrackingObjectFactory();
};

}


#endif // MITKFIBERTRACKINGOBJECTFACTORY_H
