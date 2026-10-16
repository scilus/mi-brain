# Dire à TRX d'utiliser la version de Eigen3 fournie par ITK (sinon, il y a un conflit de version entre ITK et TRX)
#set(TRX_EIGEN3_TARGET ITK::ITKEigen3Module)

MITK_CREATE_MODULE(
  DiffusionIO
  INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS FiberBundle MitkDICOM MitkSceneSerializationBase
  AUTOLOAD_WITH MitkCore
  PACKAGE_DEPENDS PUBLIC DCMTK|dcmtract+dcmiod+dcmdata+ofstd+oflog
)

target_link_libraries(
    ${MODULE_TARGET}
    PRIVATE
    trx-cpp::trx
)
