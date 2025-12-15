
#ifndef IMEKA_BOUNDING_OBJECT_FACTORY_HPP_INCLUDED
#define IMEKA_BOUNDING_OBJECT_FACTORY_HPP_INCLUDED

#include <string>

#include <QStringList>

#include <mitkBoundingObject.h>
#include <mitkDataNode.h>

#include "ImekaBoundingObjectExports.h"

class QWidget;
namespace mitk { class Image; }

namespace Imeka
{

namespace BoundingObject
{

class ImekaBoundingObject_EXPORT BoundingObjectFactory
{
public:
  typedef std::vector<std::string> BoundingObjectNamesType;
  class InvalidBoundingObjectName {};

  ~BoundingObjectFactory() { }

  static BoundingObjectFactory* get()
  {
    static BoundingObjectFactory instance;
    return &instance;
  }

  const BoundingObjectNamesType& names() const
  {
    return m_BoundingObjectNames;
  }

  mitk::BoundingObject::Pointer createBoundingObject(const std::string&);
  mitk::DataNode::Pointer CreateBoundingObjectFromDialog(
    QWidget* parent,
    const bool isHelperObject = false,
    const QStringList& typesToRemove = QStringList());

  bool typeExists(const std::string &) const;

private:
  BoundingObjectFactory();
  BoundingObjectFactory(const BoundingObjectFactory&) {}
  BoundingObjectFactory& operator=(const BoundingObjectFactory&)
  {
    return *this;
  }

  BoundingObjectNamesType m_BoundingObjectNames;

  enum TYPES
  {
    TYPES_BEGIN = 0,
    CUBOID = TYPES_BEGIN,
    ELLIPSOID,
    CYLINDER,
    CONE,
    TYPES_END
  };
};

} // namespace BoundingObject

} // namespace Imeka

#endif // IMEKA_BOUNDING_OBJECT_FACTORY_HPP_INCLUDED

