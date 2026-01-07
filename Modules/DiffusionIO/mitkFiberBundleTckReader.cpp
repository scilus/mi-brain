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

#include "mitkFiberBundleTckReader.hpp"

#include <itksys/SystemTools.hxx>

#include <mitkDiffusionIOMimeTypes.h>
#include "FiberBundle/FilteredFiberBundle.hpp"
#include "FiberBundle/Tck.hpp"

mitk::FiberBundleTckReader::FiberBundleTckReader()
  : mitk::AbstractFileReader( mitk::DiffusionIOMimeTypes::FIBERBUNDLE_TCK_MIMETYPE_NAME(), "tck Fiber Bundle Reader (MRtrix format)" )
{
  m_ServiceReg = this->RegisterService();
}

mitk::FiberBundleTckReader::FiberBundleTckReader(const FiberBundleTckReader &other)
  :mitk::AbstractFileReader(other)
{
}

mitk::FiberBundleTckReader * mitk::FiberBundleTckReader::Clone() const
{
  return new FiberBundleTckReader(*this);
}

std::vector<itk::SmartPointer<mitk::BaseData>> mitk::FiberBundleTckReader::DoRead()
{

  std::vector<itk::SmartPointer<mitk::BaseData> > result;
  try
  {
    const std::string& locale = "C";
    const std::string& currLocale = setlocale( LC_ALL, nullptr );
    setlocale(LC_ALL, locale.c_str());

    std::string filename = this->GetInputLocation();

    std::string ext = itksys::SystemTools::GetFilenameLastExtension(filename);
    ext = itksys::SystemTools::LowerCase(ext);

    if (ext==".tck")
    {
      MITK_INFO << "Loading tractogram (MRtrix format): " << itksys::SystemTools::GetFilenameName(filename);

      auto fiber = FilteredFiberBundle::New();
      if (ReadTck(this->GetInputLocation(), fiber) == 0)
      {
        throw std::length_error(
          "There's 0 point in this file. It won't be added to the DataManager "
          "because there's nothing to be done with this file.");
      }
      
      result.push_back(fiber.GetPointer());
    }

    setlocale(LC_ALL, currLocale.c_str());
    MITK_INFO << "Fiber bundle read";
    return result;
  }
  catch(...)
  {
    throw;
  }
  return result;
}
