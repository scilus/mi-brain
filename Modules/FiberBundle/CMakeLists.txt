MITK_CREATE_MODULE( FiberBundle
  PACKAGE_DEPENDS PUBLIC Qt6|Core OpenMP|OpenMP_CXX VTK|CommonComputationalGeometry
  DEPENDS MitkCore
  WARNINGS_NO_ERRORS
)

target_link_libraries(
    ${MODULE_TARGET}
    PRIVATE
    trx-cpp::trx
)

add_definitions(-DBOOST_TIMER_ENABLE_DEPRECATED)
