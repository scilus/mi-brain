set(CPP_FILES
  ImekaFiber/Coloring/FibersColors.cpp
  ImekaFiber/Coloring/SurfaceColors.cpp
  ImekaFiber/Filtering/ActiveROIsDialogBox.cpp
  ImekaFiber/Filtering/Filtering.cpp
  ImekaFiber/Filtering/FilteringUI.cpp
  ImekaFiber/Filtering/Result.cpp
  ImekaFiber/Filtering/SOFiltering.cpp
  ImekaFiber/Filtering/SurfaceFiltering.cpp
  ImekaFiber/Filtering/common.cpp
  ImekaFiber/Glyph/Maxima.cpp
  ImekaFiber/Glyph/TrackingMask.cpp
  ImekaFiber/Mapper/MapperData.cpp
  ImekaFiber/Mapper/MitkFiberMapper2D.cpp
  ImekaFiber/Mapper/MitkFiberMapper3D.cpp
  ImekaFiber/Mapper/Mappers2DSettingsWidget.cpp
  ImekaFiber/Mapper/PeakImageMapper2D.cpp
  ImekaFiber/Mapper/mitkFiberShaderController.cpp
  ImekaFiber/Mapper/vtkFiberMapper.cpp
  ImekaFiber/FibersManager.cpp
  ImekaFiber/FiberNodeData.cpp
  ImekaFiber/GroupNodes.cpp
  ImekaFiber/MaximaData.cpp
  ImekaFiber/RTT.cpp
  ImekaFiber/StreamlinesCutter.cpp
  ImekaFiber/Saver.cpp
  ImekaFiber/Surfaces.cpp
  ImekaFiber/types.hpp
  ImekaFiber/utils.cpp
)
if(BUILD_TESTING)
  list(APPEND CPP_FILES
    ImekaFiber/Testing/FibersManagerTester.cpp
    ImekaFiber/Testing/TractsTester.cpp
  )
endif()

set(MOC_H_FILES
  ImekaFiber/Filtering/ActiveROIsDialogBox.hpp
  ImekaFiber/Filtering/FilteringUI.hpp
  ImekaFiber/Mapper/Mappers2DSettingsWidget.hpp
  ImekaFiber/FibersManager.hpp
  ImekaFiber/MaximaData.hpp
)

set(UI_FILES
  ImekaFiber/Mapper/Mappers2DSettingsWidget.ui
)

set(QRC_FILES
)

