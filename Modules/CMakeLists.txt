# Modules must be listed according to their dependencies
set(module_dirs
  ImekaCommon
  ImekaReport
  ImekaGeometry
  ImekaBoundingObject
  ImekaWidgets
  FiberBundle #MITK
  ImekaFiber
  ImekaIO
  DiffusionIO #MITK
)

foreach(module_dir ${module_dirs})
  message("DEBUG: Adding module subdirectory: ${module_dir}")
  add_subdirectory(${module_dir})
endforeach()