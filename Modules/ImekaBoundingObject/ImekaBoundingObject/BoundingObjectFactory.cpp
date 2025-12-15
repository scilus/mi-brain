
#include "BoundingObjectFactory.hpp"

#include <QInputDialog>

#include <mitkCone.h>
#include <mitkCuboid.h>
#include <mitkCylinder.h>
#include <mitkEllipsoid.h>

#include "NodeUtils.hpp"

namespace Imeka
{

namespace BoundingObject
{

BoundingObjectFactory::BoundingObjectFactory()
{
  m_BoundingObjectNames.resize(TYPES_END);
  m_BoundingObjectNames[CUBOID] = "Cuboid";
  m_BoundingObjectNames[ELLIPSOID] = "Ellipsoid";
  m_BoundingObjectNames[CONE] = "Cone";
  m_BoundingObjectNames[CYLINDER] = "Cylinder";
}

mitk::BoundingObject::Pointer BoundingObjectFactory::createBoundingObject(
  const std::string& name)
{
  mitk::BoundingObject::Pointer p = nullptr;
  if (name == names()[CUBOID]) { p = mitk::Cuboid::New(); }
  else if (name == names()[ELLIPSOID]) { p = mitk::Ellipsoid::New(); }
  else if (name == names()[CYLINDER]) { p = mitk::Cylinder::New(); }
  else if (name == names()[CONE]) { p = mitk::Cone::New(); }
  return p;
}

mitk::DataNode::Pointer
BoundingObjectFactory::CreateBoundingObjectFromDialog(
  QWidget* parent,
  const bool isHelperObject,
  const QStringList& typesToRemove)
{
  QStringList items;
  for (const auto& name : names())
  {
    const QString boundingObjectName = QString::fromStdString(name);
    if (!typesToRemove.contains(boundingObjectName))
    {
      items << QObject::tr(name.c_str());
    }
  }

  bool ok;
  const QString item = QInputDialog::getItem(
    parent, QObject::tr("Select Bounding Object"),
    QObject::tr("Type of Bounding Object:"), items, 0, false, &ok);
  if (!ok) { return nullptr; }

  auto boundingObject = createBoundingObject(item.toStdString());
  return GetNewBoundingObjectNode(boundingObject, isHelperObject);
}

bool BoundingObjectFactory::typeExists(const std::string &type) const
{
  return std::find(names().begin(), names().end(), type) != names().end();
}

} // namespace BoundingObject

} // namespace Imeka

