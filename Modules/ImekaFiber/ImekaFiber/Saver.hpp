#ifndef IMEKA_FIBER_SAVING_HPP_INCLUDED
#define IMEKA_FIBER_SAVING_HPP_INCLUDED

#include <vector>

#include "FiberBundle/FilteredFiberBundle.hpp"

#include "ImekaCommon/DataManager.hpp"
#include "ImekaCommon/types.hpp"
#include "FiberNodeData.hpp"

#include "ImekaFiberExports.h"

namespace mitk { class DataNode; class FilteredFiberBundle; }

namespace Imeka
{

namespace Fiber
{

class ImekaFiber_EXPORT Saver
{
public:
  static const QString DEFAULT_DIR;

  static Saver& Instance()
  {
    static Saver instance;
    return instance;
  }

  Saver(Saver const&) = delete;
  Saver(Saver&&) = delete;
  Saver& operator=(Saver const&) = delete;
  Saver& operator=(Saver &&) = delete;

  void AddNodes(NodeDataMap*, const bool);
  void RemoveRTTNodes() { m_RTTDataMap = nullptr; }

  // Next Save won't ask the user with a SaveDialog. Mostly for TESTING.
  void SetSavePath(const std::string& path) { m_SaveTo = path; }

  void Save(const bool, Imeka::DataManager& DM);
  void Save(const mitk::DataNode*, const bool, Imeka::DataManager& DM);
  void Save(
    const mitk::DataNode*,
    const mitk::DataNode*,
    const bool, Imeka::DataManager& DM);

  mitk::FilteredFiberBundle::Pointer FiberBundleFromVisibleStreamlines() const;
  mitk::FilteredFiberBundle::Pointer FiberBundleFromVisibleStreamlines(
    const mitk::DataNode*, const bool) const;
  mitk::FilteredFiberBundle::Pointer FiberBundleFromVisibleStreamlines(
    const mitk::DataNode*, const mitk::DataNode*, const bool) const;

protected:
  Saver();
  ~Saver() {}

private:
  bool Warn(const mitk::FilteredFiberBundle*) const;
  unsigned int TotalNbPoints() const;
  void SaveToFile(const mitk::FilteredFiberBundle*, const std::string&);
  void SaveToDM(mitk::FilteredFiberBundle*, Imeka::DataManager&) const;

  NodeDataMap* m_DatasetDataMap;
  NodeDataMap* m_RTTDataMap;
  std::string m_SaveTo; // Tests only
};

void ImekaFiber_EXPORT ExportFibersStatsToCSV(const ConstNodes&, const bool);

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_SAVING_HPP_INCLUDED
