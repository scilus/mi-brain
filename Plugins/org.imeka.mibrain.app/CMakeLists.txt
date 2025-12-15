project(org_imeka_mibrain_app)

include_directories(${CMAKE_BINARY_DIR})

set(PACKAGES Qt5|Network)
if(WIN32)
  find_package(OpenSSL)
  if(NOT OPENSSL_FOUND AND NOT SSL_EAY_RELEASE AND NOT LIB_EAY_RELEASE)
    message( "Could not find OpenSSL, the Updater will not work. Please set the SSL_EAY_RELEASE and LIB_EAY_RELEASE CMake variables to the respective libraries." )
  else(WIN32)
    list(APPEND PACKAGES OpenSSL)
  endif()
endif()

set(MODULES )
if(Imeka_MIBRAIN_LICENSE)
  list(APPEND MODULES ImekaLicense)
endif()

mitk_create_plugin(
  EXPORT_DIRECTIVE MIBRAINAPP_EXPORT
  INCLUDE_DIRS ${OPENSSL_INCLUDE_DIR}
  EXPORTED_INCLUDE_SUFFIXES src
  PACKAGE_DEPENDS ${PACKAGES}
  MODULE_DEPENDS ${MODULES}
  WARNINGS_NO_ERRORS
)
