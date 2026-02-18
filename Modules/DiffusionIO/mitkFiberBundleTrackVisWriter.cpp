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

#include "mitkFiberBundleTrackVisWriter.h"
#include <vtkSmartPointer.h>
#include <vtkCleanPolyData.h>
#include <itksys/SystemTools.hxx>
#include "FiberBundle/mitkTrackvis.h"
#include <itkSize.h>
#include <vtkFloatArray.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <mitkAbstractFileWriter.h>
#include <mitkCustomMimeType.h>
#include "mitkDiffusionIOMimeTypes.h"
#include <itkMetaDataObject.h>

mitk::FiberBundleTrackVisWriter::FiberBundleTrackVisWriter()
    : mitk::AbstractFileWriter(mitk::FiberBundle::GetStaticNameOfClass(), mitk::DiffusionIOMimeTypes::FIBERBUNDLE_TRK_MIMETYPE_NAME(), "TrackVis Fiber Bundle Reader")
{
    RegisterService();
}

mitk::FiberBundleTrackVisWriter::FiberBundleTrackVisWriter(const mitk::FiberBundleTrackVisWriter & other)
    :mitk::AbstractFileWriter(other)
{}

mitk::FiberBundleTrackVisWriter::~FiberBundleTrackVisWriter()
{}

mitk::FiberBundleTrackVisWriter * mitk::FiberBundleTrackVisWriter::Clone() const
{
    return new mitk::FiberBundleTrackVisWriter(*this);
}

void mitk::FiberBundleTrackVisWriter::Write()
{

    std::ostream* out;
    std::ofstream outStream;

    if( this->GetOutputStream() )
    {
        out = this->GetOutputStream();
    }else{
        outStream.open( this->GetOutputLocation().c_str() );
        out = &outStream;
    }

    if ( !out->good() )
    {
        mitkThrow() << "Stream not good.";
    }

    try
    {
        const std::string& locale = "C";
        const std::string& currLocale = setlocale( LC_ALL, nullptr );
        setlocale(LC_ALL, locale.c_str());

        std::locale I("C");
        out->imbue(I);

        std::string filename = this->GetOutputLocation().c_str();

        mitk::FilteredFiberBundle::ConstPointer input =
          dynamic_cast<const mitk::FilteredFiberBundle *>(this->GetInput());
        std::string ext = itksys::SystemTools::GetFilenameLastExtension(this->GetOutputLocation().c_str());

        if(ext == "")
        {
            ext = ".trk";
            this->SetOutputLocation(this->GetOutputLocation() + ext);
        }

        // MITK 2025 FIX: Ensure metadata exists for correct geometry preservation.
        // If metadata is missing (e.g. fibers created in MITK), manually generate it
        // to force TrackVis writer to use the "ITSELF" path, avoiding the broken "ANAT" path
        // which introduces coordinate shifts.
        if (input->GetReferenceGeometry())
        {
             // We need mutable access to geometry to add metadata.
             // GetReferenceGeometry returns a pointer to the geometry, which we can modify.
             auto geo = input->GetReferenceGeometry();
             itk::MetaDataDictionary& dict = geo->GetMetaDataDictionary();
             
             if (dict.Find("a0") == dict.End())
             {
                  MITK_INFO << "Generating TRK metadata from geometry to ensure coordinate fidelity.";
                  
                  // 1. Voxel Size & Dims
                  mitk::Vector3D spacing = geo->GetSpacing();
                  
                  for(int i=0; i<3; ++i) {
                      double size = geo->GetExtent(i);
                      double vs = spacing[i];
                      double dim = (vs > 0) ? size / vs : 0;
                      
                      itk::EncapsulateMetaData<double>(dict, "vs" + std::to_string(i), vs);
                      itk::EncapsulateMetaData<double>(dict, "d" + std::to_string(i), std::round(dim));
                      itk::EncapsulateMetaData<double>(dict, "o" + std::to_string(i), 0.0);
                  }
                  
                  // 2. Matrix (LPS -> RAS)
                  auto transform = geo->GetIndexToWorldTransform();
                  auto matrix = transform->GetMatrix();
                  auto offset = transform->GetOffset();
                  
                  // Construct RAS matrix by flipping X and Y (rows 0 and 1)
                  double voxToWorld[16];
                  std::fill(voxToWorld, voxToWorld+16, 0.0);
                  voxToWorld[15] = 1.0;
                  
                  for(int r=0; r<3; ++r) {
                      double factor = (r < 2) ? -1.0 : 1.0;
                      for(int c=0; c<3; ++c) {
                          voxToWorld[r*4 + c] = factor * matrix(r,c);
                      }
                      voxToWorld[r*4 + 3] = factor * offset[r];
                  }
                  
                  for(int i=0; i<16; ++i) {
                      itk::EncapsulateMetaData<double>(dict, "a" + std::to_string(i), voxToWorld[i]);
                  }
                  
                  char voxelOrder[] = "RAS";
                  for(int i=0; i<3; ++i) {
                      itk::EncapsulateMetaData<char>(dict, "vo" + std::to_string(i), voxelOrder[i]);
                  }
             }
        }

        MITK_INFO << "Writing fiber bundle as TRK";
        TrackVisFiberReader trk;
        if (trk.Create(filename, input))
        {
          trk.WriteHdr();
          trk.Append(input);
        }
        else
        {
          // Erase file and explain why we can't save trk
          out = nullptr;
          outStream.close();
          remove(filename.c_str());
          mitkThrow() <<
			      "\nCan't write a .trk file from a .tck file without loading a "
            "reference anatomy.\nPlease load one if you want to save to .trk, "
            "or save to .tck.";
        }

        setlocale(LC_ALL, currLocale.c_str());
        MITK_INFO << "TrackVis Fiber bundle written to " << filename;
    }
    catch(...) { throw; }
}
