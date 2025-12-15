
#ifndef IMEKA_FIBER_TESTING_FIBER_MANAGER_TESTER_HPP_INCLUDED
#define IMEKA_FIBER_TESTING_FIBER_MANAGER_TESTER_HPP_INCLUDED

#include <mitkStandaloneDataStorage.h>
#include <mitkTestFixture.h>

#include "ImekaCommon/Callback.hpp"
#include "ImekaCommon/DataManager.hpp"
#include "ImekaFiber/FibersManager.hpp"
#include "ImekaFiber/GroupNodes.hpp"
#include "plitkTestingConfig.hpp"

#include "ImekaFiberExports.h"

class ImekaFiber_EXPORT FibersManagerTester : public mitk::TestFixture
{
public:
  FibersManagerTester();
  void setUp() override;
  void tearDown() override;
  ~FibersManagerTester();

protected:
  typedef Imeka::Fiber::ActiveROIsDialogBox::AutoSetupLines AutoSetupLines;

  mitk::Image::Pointer GetAnat(
    const mitk::Vector3D& spacing = 1.0,
    const mitk::Point3D& origin = 0.0,
    bool reverse = false);

  std::string GetDataFilePath(const std::string& testData)
  {
    return std::string(PLITK_DATA_DIR) + "/" + testData;
  }

  mitk::DataNode* AddAnatNode(mitk::Image* anat);

  /* Create a 10 streamlines of 2 points each
  (0.0, 4.5) ---------- (10.0, 4.5)
  (0.0, 4.0) ---------  (9.0, 4.0)
  ...        -...-
  (0.0, 0.0) -          (1.0, 0.0)
  Use `oblique` to add a third point going up.   */
  mitk::FilteredFiberBundle::Pointer Get1_10Fibers(
    const mitk::BaseGeometry*, const bool);

  mitk::DataNode* AddFibersNode(
    mitk::FilteredFiberBundle* fibers,
    const char* nodeName="FibersNode");

  mitk::Image::Pointer LoadImage(const std::string& path);
  mitk::FilteredFiberBundle::Pointer LoadFibers(const std::string& path);

  std::pair<mitk::DataNode*, mitk::BoundingObject*> CreateSO(
    const mitk::Vector3D& spacing,
    const mitk::Point3D& origin,
    const std::string& nodeName = "") const;

  std::pair<mitk::DataNode*, mitk::BoundingObject*> SetupTG(
    mitk::DataNode* fibersNode,
    const mitk::Vector3D& spacing,
    const mitk::Point3D& origin,
    const std::string& nodeName = "New Tract Group");
  mitk::DataNode* CreateTG(
    mitk::DataNode*,
    const Nodes&,
    const std::string& nodeName) const;
  mitk::DataNode* CreateTG(
    mitk::DataNode*,
    const AutoSetupLines&,
    const std::string& nodeName) const;
  void EditTG(mitk::DataNode*, const AutoSetupLines&) const;

  void MoveSO(mitk::BoundingObject* SO, const mitk::Point3D& p);

  void AssertColorAre(const mitk::DataNode*, const std::vector<QColor>&) const;
  void AssertColorIs(const mitk::DataNode*, const std::vector<QColor>&) const;
  void AssertColorIs(const QColor&, const unsigned char*) const;
  void AssertColorIs(const QColor&, const QColor&) const;

private:
  void NodeAddedProxy(const mitk::DataNode* node);
  void NodeRemovedProxy(const mitk::DataNode* node);

protected:
  mitk::DataStorage::Pointer m_DS;
  Imeka::Callback m_Callback;
  Imeka::DataManager* m_DM;
  Imeka::Fiber::GroupNodes* m_Groups;
  Imeka::Fiber::FibersManager* m_FM;
  bool m_BlockNodeAdded, m_BlockNodeRemoved;
};

#endif // IMEKA_FIBER_TESTING_FIBER_MANAGER_TESTER_HPP_INCLUDED
