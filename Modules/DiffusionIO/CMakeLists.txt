include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../FiberBundle/FiberBundle)

MITK_CREATE_MODULE( DiffusionIO
  INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS FiberBundle MitkDICOM MitkSceneSerializationBase
  AUTOLOAD_WITH MitkCore
  PACKAGE_DEPENDS PUBLIC DCMTK
)

