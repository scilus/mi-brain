
#include "DataStorageUtils.hpp"

#include <mitkDataStorage.h>
#include <mitkImage.h>
#include <mitkNodePredicateDataType.h>
#include <mitkNodePredicateProperty.h>
#include "mitkFiberBundle.h"

mitk::DataStorage* DataStorageSingleton::dataStorage = nullptr;

mitk::DataNode* GetAnatNode()
{
  if (!DataStorageSingleton::dataStorage) { return nullptr; }

  auto isAnat = mitk::NodePredicateProperty::New(
    "Anat", mitk::BoolProperty::New(true));
  const auto anatNodes = DataStorageSingleton::dataStorage->GetSubset(isAnat);
  if (anatNodes->Size() == 1)
  {
    return anatNodes->GetElement(0);
  }
  return nullptr;
}

mitk::BaseGeometry::Pointer GetTransformFromTrk()
{
  const auto fiberNodes = DataStorageSingleton::dataStorage->GetSubset(
    mitk::NodePredicateDataType::New("FilteredFiberBundle"));
  for (const auto& node : *fiberNodes)
  {
    std::string path;
    node->GetStringProperty("full_path", path);
    if (path.substr(path.find_last_of(".") + 1) == "trk")
    {
      auto geo = dynamic_cast<mitk::FiberBundle*>(
        node->GetData())->GetReferenceGeometry();
      if (geo)
      {
        const auto dictionary = geo->GetMetaDataDictionary();
        if (dictionary.Find("a0") != dictionary.End())
        {
          return geo;
        }
      }
    }
  }
  return nullptr;
}

mitk::BaseGeometry::Pointer FiberBundle_EXPORT
GetTransformFromAnat()
{
  mitk::DataNode* anatNode = GetAnatNode();
  if (!anatNode) { return nullptr; }
  mitk::Image* anat = dynamic_cast<mitk::Image*>(anatNode->GetData());
  if (!anat) { return nullptr; }

  return anat->GetGeometry()->Clone();
}
