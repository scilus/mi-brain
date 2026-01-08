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
#include "mitkFiberBundleTrackVisReader.h"
#include "mitkFiberBundleTckReader.hpp"


mitk::FiberTrackingObjectFactory::FiberTrackingObjectFactory()
  : CoreObjectFactoryBase()
{
}

mitk::FiberTrackingObjectFactory::~FiberTrackingObjectFactory()
{
}

mitk::Mapper::Pointer mitk::FiberTrackingObjectFactory::CreateMapper(mitk::DataNode* node, MapperSlotId id)
{
  std::cout << "FiberTrackingObjectFactory::CreateMapper called for node: " 
            << (node ? node->GetName() : "nullptr") 
            << ", MapperSlotId: " << id << "\n";
  
  mitk::Mapper::Pointer newMapper = nullptr;
  if (node->GetData())
  {
    const std::string fb = "FiberBundle";
    const std::string ffb = "FilteredFiberBundle";
    const std::string pi = "PeakImage";
    const std::string className = node->GetData()->GetNameOfClass();
    std::cout << "  Data class name: " << className << "\n";
    
    if (id == mitk::BaseRenderer::Standard2D)
    {
      std::cout << "  Mapper type: Standard2D\n";
      if (className == fb || className == ffb)
      {
        std::cout << "  Creating FiberBundleMapper2D\n";
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
      std::cout << "  Mapper type: Standard3D\n";
      if (className == fb || className == ffb)
      {
        std::cout << "  Creating FiberBundleMapper3D for " << className << "\n";
        newMapper = mitk::FiberBundleMapper3D::New();
        newMapper->SetDataNode(node);
        std::cout << "  FiberBundleMapper3D created successfully\n";
      }
      else if (className == pi)
      {
        newMapper = mitk::PeakImageMapper3D::New();
        newMapper->SetDataNode(node);
      }
    }
  }
  else
  {
    std::cout << "  ERROR: node->GetData() is nullptr!\n";
  }

  if (newMapper.IsNull())
  {
    std::cout << "  Returning nullptr mapper\n";
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
  return fileExtension.c_str();
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
  // Create instances of readers to trigger their self-registration
  // The readers register themselves via RegisterService() in their constructors
  std::cout << "RegisterIOFactories: Instantiating TRK and TCK readers\n";
  
  // TRK reader (TrackVis format)
  auto trkReader = mitk::FiberBundleTrackVisReader();
  std::cout << "  TRK reader instantiated\n";
  
  // TCK reader (MRtrix format)
  auto tckReader = mitk::FiberBundleTckReader();
  std::cout << "  TCK reader instantiated\n";
}

struct RegisterFiberTrackingObjectFactory{
  RegisterFiberTrackingObjectFactory()
    : m_Factory( mitk::FiberTrackingObjectFactory::New() )
  {
    std::cout << "FiberTrackingObjectFactory instance created\n";
    auto coreFactory = mitk::CoreObjectFactory::GetInstance();
    std::cout << "CoreObjectFactory instance: " << coreFactory << "\n";
    if (coreFactory)
    {
      std::cout << "Registering FiberTrackingObjectFactory with CoreObjectFactory\n";
      coreFactory->RegisterExtraFactory( m_Factory );
      std::cout << "FiberTrackingObjectFactory registered successfully\n";
    }
    else
    {
      std::cout << "ERROR: CoreObjectFactory::GetInstance() returned null!\n";
    }
  }

  ~RegisterFiberTrackingObjectFactory()
  {
    std::cout << "Unregistering FiberTrackingObjectFactory\n";
    mitk::CoreObjectFactory::GetInstance()->UnRegisterExtraFactory( m_Factory );
  }

  mitk::FiberTrackingObjectFactory::Pointer m_Factory;
};

static RegisterFiberTrackingObjectFactory registerFiberTrackingObjectFactory;
