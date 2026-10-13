# Dire à TRX d'utiliser la version de Eigen3 fournie par ITK (sinon, il y a un conflit de version entre ITK et TRX)
#set(TRX_EIGEN3_TARGET ITK::ITKEigen3Module)

set(trx-cpp_DIR "/data/clan2201/trx-cpp/build")

find_dependency(Eigen3)
find_dependency(libzip)

find_package(trx-cpp CONFIG REQUIRED)

find_package(Eigen3 REQUIRED)

target_link_libraries(${MODULE_NAME}
  PRIVATE
  trx-cpp::trx
)

MITK_CREATE_MODULE(
  DiffusionIO
  INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS FiberBundle MitkDICOM MitkSceneSerializationBase
  AUTOLOAD_WITH MitkCore
  PACKAGE_DEPENDS PUBLIC DCMTK|dcmtract+dcmiod+dcmdata+ofstd+oflog
)
