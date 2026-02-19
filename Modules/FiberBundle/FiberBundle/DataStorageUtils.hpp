#ifndef DATA_STORAGE_SINGLETON_HPP_INCLUDED
#define DATA_STORAGE_SINGLETON_HPP_INCLUDED

#include <mitkBaseGeometry.h>
#include "FiberBundleExports.h"

namespace mitk { class DataStorage; class DataNode; }

// Class to hold the current DataStorage.
// Yes, it's a huge ugly hack
class FiberBundle_EXPORT DataStorageSingleton
{
public:
  DataStorageSingleton() {};

  static mitk::DataStorage* dataStorage;
};

namespace mitk {
  FiberBundle_EXPORT mitk::DataNode* GetAnatNode();
  FiberBundle_EXPORT mitk::BaseGeometry::Pointer GetTransformFromTrk();
  FiberBundle_EXPORT mitk::BaseGeometry::Pointer GetTransformFromAnat();
}

#endif // DATA_STORAGE_SINGLETON_HPP_INCLUDED
