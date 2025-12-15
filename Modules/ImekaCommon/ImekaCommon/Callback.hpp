
#ifndef IMEKA_COMMON_CALLBACK_HPP_INCLUDED
#define IMEKA_COMMON_CALLBACK_HPP_INCLUDED

#include <itkEventObject.h>

#include <functional>
#include <map>
#include <vector>

#include "DataManager.hpp"
#include "Property/Property.hpp"

#include "ImekaCommonExports.h"

namespace itk { class Object; }
namespace mitk { class DataNode; }

namespace Imeka
{

// Add a callback to a mitk::DataNode when a property is modified or to an
// itk::Object when it is modified.
// For practical reasons, all callbacks are disabled when
// - A callback is already running.
// - You set the "DisableCallback" property to True
// so there can't be a callback triggering a callback triggering ...
class ImekaCommon_EXPORT Callback
{
private:
  typedef std::function<void(mitk::DataNode*)> CallbackFunction;

  class Command
  {
  public:
    Command(
      const std::string& name,
      CallbackFunction f,
      mitk::DataNode* node,
      itk::Object* observedObject,
      bool& disableCallbacks)
      : m_Name(name)
      , m_Function(f)
      , m_Node(node)
      , m_ObservedObject(observedObject)
      , m_DisableAllCallbacks(disableCallbacks)
      , m_ObserverID(0)
    {}

    void SetObserverID(const unsigned long oberverID)
    {
      m_ObserverID = oberverID;
    }

    std::string Name() const { return m_Name; }

    void RemoveObserver()
    {
      m_ObservedObject->RemoveObserver(m_ObserverID);
    }

    void Execute(const itk::Object*, const itk::EventObject &)
    {
      bool temporaryDisable = false;
      m_Node->GetBoolProperty("DisableCallback", temporaryDisable);

      if (!m_DisableAllCallbacks && !temporaryDisable)
      {
        m_DisableAllCallbacks = true;
        m_Function(m_Node);
        m_DisableAllCallbacks = false;
      }
    }

  private:
    const std::string m_Name;
    CallbackFunction m_Function;
    mitk::DataNode* m_Node;
    itk::Object* m_ObservedObject;
    bool& m_DisableAllCallbacks;
    unsigned long m_ObserverID;
  };

  typedef std::vector<Command*> Commands;

public:
  Callback() : m_DisableAllCallbacks(false) {}

  void AllowOneMoreLevelOfCallback()
  {
    m_DisableAllCallbacks = false;
  }

  // Call lambda on any modification of node's property.
  void Add(
    const std::string& propertyName,
    mitk::DataNode* node,
    CallbackFunction f)
  {
    SetCallback(
      propertyName, node, node->GetProperty(propertyName.c_str()), f);
  }

  template <typename T>
  void Add(
    const std::string& propertyName,
    const T defaultValue,
    mitk::DataNode* node,
    CallbackFunction f)
  {
    // Note to programmer: I tried using the defaultValue to not call the
    // lambda function if value == defaultValue but it's really complicated
    // with the various types of properties.
    if (!Imeka::Property::HasProperty(node, propertyName))
    {
      auto property = Imeka::Property::New(defaultValue);
      node->AddProperty(propertyName.c_str(), property);
    }
    Add(propertyName, node, f);
  }

  // Call lambda on any modification of an ITK object
  void Add(mitk::DataNode* node, itk::Object* itkObject, CallbackFunction f)
  {
    SetCallback("", node, itkObject, f);
  }

  // Remove all callbacks from a particular node/object
  void Remove(const mitk::DataNode*, const std::string& = "");

  void SetVisibilityCallback(mitk::DataNode*, Imeka::DataManager&);
  void SetVisibilityCallback(
    mitk::DataNode*,
    std::function<void()>,
    std::function<void()>,
    std::function<void()>,
    Imeka::DataManager&);

private:
  void SetCallback(
    const std::string&, mitk::DataNode*, itk::Object*, CallbackFunction);

  std::map<mitk::DataNode*, Commands> m_Callbacks;
  bool m_DisableAllCallbacks;
};

} // namespace Imeka

#endif /// IMEKA_COMMON_CALLBACK_HPP_INCLUDED
