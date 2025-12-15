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
