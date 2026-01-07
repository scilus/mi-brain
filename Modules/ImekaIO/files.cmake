set(CPP_FILES
  ImekaIO/ImekaIOModuleActivator.cpp
  ImekaIO/MimeType.cpp
  ImekaIO/BoundingObject/BoundingObjectReader.cpp
  ImekaIO/BoundingObject/BoundingObjectWriter.cpp
  ImekaIO/BoundingObject/BoundingObjectSerializer.cpp
  # TODO: DicomWriter.cpp needs ITK API updates for MITK 2025.12
  # ImekaIO/Dicom/DicomWriter.cpp
  ImekaIO/DataNodeGroup/DataNodeGroupSerializer.cpp
  ImekaIO/DataNodeGroup/DataNodeGroupWriter.cpp
  ImekaIO/DataNodeGroup/DataNodeGroupReader.cpp
  ImekaIO/FDFLoader/FDFReader.cpp
  ImekaIO/VFFLoader/VFFReader.cpp
)

set(MOC_H_FILES
)

set(UI_FILES
)

set(QRC_FILES
)

