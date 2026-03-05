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

#include "mitkFiberBundleTrackVisReader.h"

#include <itksys/SystemTools.hxx>

#include <mitkDiffusionIOMimeTypes.h>
#include "FiberBundle/mitkTrackvis.h"

mitk::FiberBundleTrackVisReader::FiberBundleTrackVisReader()
  : mitk::AbstractFileReader( mitk::DiffusionIOMimeTypes::FIBERBUNDLE_TRK_MIMETYPE_NAME(), "TrackVis Fiber Bundle Reader" )
{
  m_ServiceReg = this->RegisterService();
}

mitk::FiberBundleTrackVisReader::FiberBundleTrackVisReader(const FiberBundleTrackVisReader &other)
  :mitk::AbstractFileReader(other)
{
}

mitk::FiberBundleTrackVisReader * mitk::FiberBundleTrackVisReader::Clone() const
{
  return new FiberBundleTrackVisReader(*this);
}

std::vector<itk::SmartPointer<mitk::BaseData> > mitk::FiberBundleTrackVisReader::DoRead()
{

  std::vector<itk::SmartPointer<mitk::BaseData> > result;
  try
  {
    const std::string& locale = "C";
    const std::string& currLocale = setlocale( LC_ALL, nullptr );
    setlocale(LC_ALL, locale.c_str());

    std::string filename = this->GetInputLocation();
    MITK_INFO << "Loading tractogram (TrackVis format): " << itksys::SystemTools::GetFilenameName(filename);

    std::string ext = itksys::SystemTools::GetFilenameLastExtension(filename);
    ext = itksys::SystemTools::LowerCase(ext);

    if (ext==".trk")
    {
      auto fiber = FilteredFiberBundle::New();
      TrackVisFiberReader reader;
      reader.Open(this->GetInputLocation().c_str());
      const auto errCode = reader.Read(fiber.GetPointer());
      if (errCode == 0)
      {
        const auto emptyFileWarning =
          "There's 0 point in this file. It won't be added to the DataManager "
          "because there's nothing to be done with this file.";
        throw std::length_error(emptyFileWarning);
      }
      else if (errCode == -1)
      {
        const auto noDSWarning =
          "MI-Brain error unrelated to the current trk. "
          "The TrackVisFiberReader doesn't know the DataStorage.";
        throw std::length_error(noDSWarning);
      }
      else if (errCode == -2)
      {
        const auto corruptedHeaderWarning =
          "The trk file you're trying to load is impossible to read because "
          "the header is corrupted.";
        throw std::length_error(corruptedHeaderWarning);
      }
      else if (errCode == -3)
      {
        const auto noReadPointsWarning =
          "Error reading points. Trk file is corrupted.";
        throw std::length_error(noReadPointsWarning);
      }
      else if (errCode == -4)
      {
        const auto noReadPropertiesWarning =
          "Error reading properties. Trk file is corrupted.";
        throw std::length_error(noReadPropertiesWarning);
      }

      if (auto refGeometry = fiber->GetReferenceGeometry())
      {
        fiber->SetGeometry(refGeometry->Clone());
      }

      result.push_back(fiber.GetPointer());
      return result;
    }

    setlocale(LC_ALL, currLocale.c_str());
    MITK_INFO << "Fiber bundle read";
  }
  catch(...)
  {
    throw;
  }
  return result;
}
