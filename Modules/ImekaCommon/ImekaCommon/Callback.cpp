
#include "Callback.hpp"

#include <itkCommand.h>

#include <mitkDataNode.h>

namespace Imeka
{

void Callback::SetCallback(
  const std::string& propertyName,
  mitk::DataNode* node,
  itk::Object* itkObject,
  CallbackFunction f)
{
  auto innerCommand = new Command(
    propertyName, f, node, itkObject, m_DisableAllCallbacks);
  m_Callbacks[node].push_back(innerCommand);

  auto command = itk::MemberCommand<Command>::New();
  command->SetCallbackFunction(innerCommand, &Command::Execute);
  const auto observerID =
    itkObject->AddObserver(itk::ModifiedEvent(), command);
  innerCommand->SetObserverID(observerID);
}

void Callback::Remove(
  const mitk::DataNode* node,
  const std::string& propertyName)
{
  // Asking all the callers to be non-const is inconvenient
  auto nonConstNode = const_cast<mitk::DataNode*>(node);

  // RemoveAllObservers() should NEVER be called in our code. MITK removes
  // some observers on shutdown with specific IDs and it will crash. I tried
  // it on MITK 2016.03 and it crashes. We may want to try it again on later
  // version to simplify this code.

  const auto it = m_Callbacks.find(nonConstNode);
  if (it != m_Callbacks.end())
  {
    auto& commands = it->second;
    for (auto itC = std::begin(commands); itC != std::end(commands); )
    {
      auto command = *itC;
      if (propertyName == "" || command->Name() == propertyName)
      {
        command->RemoveObserver();
        itC = commands.erase(itC);
        delete command;
      }
      else
      {
        ++itC;
      }
    }
    if (commands.empty()) { m_Callbacks.erase(it); }
  }
}

void Callback::AddVisibilityCallback(
  mitk::DataNode* node,
  Imeka::DataManager& DM)
{
  AddVisibilityCallback(node, [](){}, [](){}, [](){}, DM);
}

void Callback::AddVisibilityCallback(
  mitk::DataNode* node,
  std::function<void()> onVisible,
  std::function<void()> onHide,
  std::function<void()> after,
  Imeka::DataManager& DM)
{
  Add("visible", node,
    [&DM, onVisible, onHide, after](mitk::DataNode* node)
  {
    if (node->IsVisible(nullptr))
    {
      DM.ShowAllParents(node);
      DM.ShowAllChildren(node);
      onVisible();
    }
    else
    {
      DM.HideAllChildren(node);
      onHide();
    }
    after();
  });
}

} // namespace Imeka
