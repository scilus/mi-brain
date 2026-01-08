set(CPP_FILES
  mitkDiffusionModuleActivator.cpp
  mitkFiberTrackingObjectFactory.cpp

  mitkDiffusionIOMimeTypes.cpp

  # TODO MITK 2025: Fix TinyXML2 migration
  # mitkFiberBundleDicomReader.cpp
  # mitkFiberBundleDicomWriter.cpp
  mitkFiberBundleTckReader.cpp
  mitkFiberBundleTckWriter.cpp
  mitkFiberBundleTrackVisReader.cpp
  mitkFiberBundleTrackVisWriter.cpp
  # TODO MITK 2025: Fix TinyXML2 migration
  # mitkFiberBundleVtkReader.cpp
  # mitkFiberBundleVtkWriter.cpp
  mitkFiberBundleSerializer.cpp
  mitkFilteredFiberBundleSerializer.cpp
  mitkFiberBundleMapper2D.cpp
  mitkFiberBundleMapper3D.cpp
  mitkPeakImageMapper2D.cpp
  mitkPeakImageMapper3D.cpp
)

