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

#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <itkMetaDataObject.h>

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

std::vector<itk::SmartPointer<mitk::BaseData>> mitk::FiberBundleTrackVisReader::DoRead()
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
      
      // MITK 2025 FIX: reader.Read() calls SetFiberPolyData() which calls UpdateFiberGeometry()
      // which replaces the geometry (with vox_to_ras transform from TRK header) with a simple 
      // bounding-box geometry. We must restore the proper geometry from the reference geometry.
      // Additionally, we correct the 0.5 voxel shift and scaling issue introduced by the reader.
      if (auto refGeometry = fiber->GetReferenceGeometry())
      {
        MITK_INFO << "MITK 2025: Restoring proper TRK geometry with vox_to_ras transform";

        // 1. Get current transform (T_bad) from reference geometry
        auto tBad = vtkSmartPointer<vtkMatrix4x4>::New();
        const auto& itkMat = refGeometry->GetIndexToWorldTransform()->GetMatrix();
        const auto& itkOff = refGeometry->GetIndexToWorldTransform()->GetOffset();
        for(int i=0; i<3; ++i) {
          for(int j=0; j<3; ++j) tBad->SetElement(i,j, itkMat(i,j));
          tBad->SetElement(i,3, itkOff[i]);
        }

        // 2. Get voxel size from metadata to construct correction matrix
        double spacing[3] = {1.0, 1.0, 1.0};
        bool hasVS = true;
        const auto& dictionary = refGeometry->GetMetaDataDictionary();
        for (int i=0; i<3; ++i) {
             std::string key = "vs" + std::to_string(i);
             if (dictionary.Find(key) == dictionary.End()) {
                 hasVS = false; break;
             }
             itk::MetaDataObject<double>::ConstPointer entry = 
                 dynamic_cast<const itk::MetaDataObject<double>*>(dictionary[key]);
             if (entry) spacing[i] = entry->GetMetaDataObjectValue();
        }

        if (hasVS) {
            MITK_INFO << "Applying voxel-shift correction for TrackVis format";
            
            // 3. Construct affine_inv to undo the scaling and shifting:
            // affine was: Scale(1/vs) * Translate(-0.5)
            // affine_inv: Translate(0.5) * Scale(vs)
            auto affineInv = vtkSmartPointer<vtkMatrix4x4>::New();
            affineInv->Identity();
            affineInv->SetElement(0, 0, spacing[0]);
            affineInv->SetElement(1, 1, spacing[1]);
            affineInv->SetElement(2, 2, spacing[2]);
            affineInv->SetElement(0, 3, 0.5 * spacing[0]);
            affineInv->SetElement(1, 3, 0.5 * spacing[1]);
            affineInv->SetElement(2, 3, 0.5 * spacing[2]);
            
            // 4. Compute T_good = T_bad * affineInv
            // T_bad = Flip * VoxToWorld * affine
            // T_good = T_bad * affineInv = Flip * VoxToWorld
            auto tGood = vtkSmartPointer<vtkMatrix4x4>::New();
            vtkMatrix4x4::Multiply4x4(tBad, affineInv, tGood);
            
            // 5. Compute correction for points: P_good = T_good * T_bad^-1 * P_bad
            auto tBadInv = vtkSmartPointer<vtkMatrix4x4>::New();
            vtkMatrix4x4::Invert(tBad, tBadInv);
            
            auto correction = vtkSmartPointer<vtkMatrix4x4>::New();
            vtkMatrix4x4::Multiply4x4(tGood, tBadInv, correction);
            
            // 6. Apply correction to all points
            auto points = fiber->GetFiberPolyData()->GetPoints();
            auto transformFilter = vtkSmartPointer<vtkTransform>::New();
            transformFilter->SetMatrix(correction);
            for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i)
            {
                double p[3];
                points->GetPoint(i, p);
                double pNew[3];
                transformFilter->TransformPoint(p, pNew);
                points->SetPoint(i, pNew);
            }
            
            // 7. Set corrected geometry
            auto newGeometry = refGeometry->Clone();
            newGeometry->SetIndexToWorldTransformByVtkMatrix(tGood);
            fiber->SetReferenceGeometry(newGeometry);
            fiber->SetGeometry(newGeometry);
        }
        else
        {
             MITK_WARN << "Could not find voxel size metadata. Skipping TrackVis shift correction.";
             fiber->SetGeometry(refGeometry->Clone());
        }
      }
      else
      {
        MITK_WARN << "MITK 2025: No reference geometry found after TRK read - fibers may not align with anatomy";
      }
      
      // Debug: Check fiber bundle contents
      MITK_INFO << "TRK loaded successfully. Fiber count: " << fiber->GetNumFibers() 
                << ", Points: " << fiber->GetNumberOfPoints();
      
      // MITK 2025: Request 3D update so mapper knows to render the fibers
      fiber->RequestUpdate3D();
      MITK_INFO << "Requested 3D update for fiber bundle";
      
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
