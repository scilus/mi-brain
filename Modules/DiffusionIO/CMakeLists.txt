MITK_CREATE_MODULE( DiffusionIO
  INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS FiberBundle MitkDICOM MitkSceneSerializationBase
  AUTOLOAD_WITH MitkCore
  PACKAGE_DEPENDS PUBLIC DCMTK|dcmtract+dcmiod+dcmdata+ofstd+oflog
)

