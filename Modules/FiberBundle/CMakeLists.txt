MITK_CREATE_MODULE( FiberBundle
  PACKAGE_DEPENDS PUBLIC Qt6|Core OpenMP|OpenMP_CXX VTK|CommonComputationalGeometry
  DEPENDS MitkCore
  WARNINGS_NO_ERRORS
)

add_subdirectory(Testing)
