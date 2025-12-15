#include "MaximaData.hpp"

#include "Mapper/PeakImageMapper2D.hpp"

namespace Imeka
{

namespace Fiber
{

void MaximaData::Add(const mitk::DataNode* node)
{
  if (!node) { return; }
  if (m_Maximas.find(node) == m_Maximas.end())
  {
    m_Maximas[node] = Maxima();
    m_Maximas[node].SetImage(dynamic_cast<mitk::Image*>(node->GetData()));
  }
}

void MaximaData::AddPeaksMapper(mitk::DataNode* node)
{
  auto fibers = m_Maximas[node].ToFiberBundle();
  fibers->ColorFibersByOrientation();

  auto mapper2D = mitk::PeakImageMapper2D::New();
  mapper2D->SetFiberBundle(fibers);
  node->SetMapper(1, mapper2D);
  mitk::PeakImageMapper2D::SetDefaultProperties(node);

  // The 3D mapper shows everything (the whole brain).
  // TODO We can probably make only 3 slices appear in 3D but I don't know how.
  node->SetMapper(2, nullptr);
  //node->SetBoolProperty("Enable3DPeaks", true);
}

Imeka::Fiber::Maxima&
MaximaData::Get(const mitk::DataNode* node)
{
  return m_Maximas[node];
}

void MaximaData::ReloadAll()
{
  for (auto& p : m_Maximas)
  {
    p.second.Reload();
  }
  emit Reloaded();
}

void MaximaData::Remove(const mitk::DataNode* node)
{
  auto it = m_Maximas.find(node);
  if (it != m_Maximas.end())
  {
    m_Maximas.erase(it);
  }
}

} // namespace Fiber

} // namespace Imeka
