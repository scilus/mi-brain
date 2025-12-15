
#ifndef IMEKA_FIBER_SURFACE_COLORS_HPP_INCLUDED
#define IMEKA_FIBER_SURFACE_COLORS_HPP_INCLUDED

#include "FiberBundle/FilteredFiberBundle.hpp"

#include "ImekaCommon/Callback.hpp"
#include "ImekaCommon/DataManager.hpp"
#include "../FiberNodeData.hpp"

#include "ImekaFiberExports.h"

namespace Imeka
{

namespace Fiber
{

enum SurfaceColorTypes { FILE, UNIFORM, MAP, FIBER, NONE };

class ImekaFiber_EXPORT SurfaceColors
{
public:
  SurfaceColors(Imeka::DataManager&, Imeka::Callback&, NodeDataMap&);

  void SetAnat(mitk::DataNode*);
  void SetSurfaceActions(mitk::DataNode* node);

private:
  void ColorFromFile(mitk::DataNode* node);
  void ColorFromMap(mitk::DataNode* node);
  void ColorFromFibers(mitk::DataNode* node);

  Imeka::DataManager& m_DM;
  Imeka::Callback& m_Callback;
  NodeDataMap& m_FibersNodeData;
  mitk::DataNode* m_AnatNode;
};

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_SURFACE_COLORS_HPP_INCLUDED
