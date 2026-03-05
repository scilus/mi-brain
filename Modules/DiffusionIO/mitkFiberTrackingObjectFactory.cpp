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

#include "mitkFiberTrackingObjectFactory.h"


mitk::FiberTrackingObjectFactory::FiberTrackingObjectFactory()
  : CoreObjectFactoryBase()
{
}

mitk::FiberTrackingObjectFactory::~FiberTrackingObjectFactory()
{
}

mitk::Mapper::Pointer mitk::FiberTrackingObjectFactory::CreateMapper(mitk::DataNode* node, MapperSlotId id)
{
  mitk::Mapper::Pointer newMapper = nullptr;
  if (node->GetData())
  {
    const std::string fb = "FiberBundle";
    const std::string ffb = "FilteredFiberBundle";
    const std::string pi = "PeakImage";
    const std::string className = node->GetData()->GetNameOfClass();
    if (id == mitk::BaseRenderer::Standard2D)
    {
      if (className == fb || className == ffb)
      {
        newMapper = mitk::FiberBundleMapper2D::New();
        newMapper->SetDataNode(node);
      }
      else if (className == pi)
      {
        newMapper = mitk::PeakImageMapper2D::New();
        newMapper->SetDataNode(node);
      }
    }
    else if (id == mitk::BaseRenderer::Standard3D)
    {
      if (className == fb || className == ffb)
      {
        newMapper = mitk::FiberBundleMapper3D::New();
        newMapper->SetDataNode(node);
      }
      else if (className == pi)
      {
        newMapper = mitk::PeakImageMapper3D::New();
        newMapper->SetDataNode(node);
      }
    }
  }

  return newMapper;
}

void mitk::FiberTrackingObjectFactory::SetDefaultProperties(mitk::DataNode* node)
{
  // Lets give the same properties to both types of fiber bundles.
  // MI-Brain will uptade the properties when required.
  if (node->GetData())
  {
    const std::string fb = "FiberBundle";
    const std::string ffb = "FilteredFiberBundle";
    const std::string className = node->GetData()->GetNameOfClass();
    if (className == fb || className == ffb)
    {
      mitk::FiberBundleMapper3D::SetDefaultProperties(node);
      mitk::FiberBundleMapper2D::SetDefaultProperties(node);
    }
    else if (className == std::string("PeakImage"))
    {
      mitk::PeakImageMapper3D::SetDefaultProperties(node);
      mitk::PeakImageMapper2D::SetDefaultProperties(node);
    }
  }
}

std::string mitk::FiberTrackingObjectFactory::GetFileExtensions()
{
  std::string fileExtension;
  this->CreateFileExtensions(m_FileExtensionsMap, fileExtension);
  return fileExtension;
}

mitk::CoreObjectFactoryBase::MultimapType mitk::FiberTrackingObjectFactory::GetFileExtensionsMap()
{
  return m_FileExtensionsMap;
}

std::string mitk::FiberTrackingObjectFactory::GetSaveFileExtensions()
{
  std::string fileExtension;
  this->CreateFileExtensions(m_SaveFileExtensionsMap, fileExtension);
  return fileExtension;
}

mitk::CoreObjectFactoryBase::MultimapType mitk::FiberTrackingObjectFactory::GetSaveFileExtensionsMap()
{
  return m_SaveFileExtensionsMap;
}

void mitk::FiberTrackingObjectFactory::CreateFileExtensionsMap()
{

}

void mitk::FiberTrackingObjectFactory::RegisterIOFactories()
{
}

struct RegisterFiberTrackingObjectFactory{
  RegisterFiberTrackingObjectFactory()
    : m_Factory( mitk::FiberTrackingObjectFactory::New() )
  {
    mitk::CoreObjectFactory::GetInstance()->RegisterExtraFactory( m_Factory );
  }

  ~RegisterFiberTrackingObjectFactory()
  {
    mitk::CoreObjectFactory::GetInstance()->UnRegisterExtraFactory( m_Factory );
  }

  mitk::FiberTrackingObjectFactory::Pointer m_Factory;
};

static RegisterFiberTrackingObjectFactory registerFiberTrackingObjectFactory;
