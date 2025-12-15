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
  add_subdirectory(${module_dir})
endforeach()