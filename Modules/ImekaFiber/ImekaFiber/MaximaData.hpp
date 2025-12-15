#ifndef IMEKA_FIBER_MAXIMA_DATA_HPP_INCLUDED
#define IMEKA_FIBER_MAXIMA_DATA_HPP_INCLUDED

#include <QObject>

#include "ImekaFiber/Glyph/Maxima.hpp"

#include "ImekaFiberExports.h"

namespace Imeka
{

namespace Fiber
{

class ImekaFiber_EXPORT MaximaData : public QObject
{
  Q_OBJECT

public:
  static MaximaData& Instance()
  {
    static MaximaData instance;
    return instance;
  }

  MaximaData(MaximaData const&) = delete;             // Copy construct
  MaximaData(MaximaData&&) = delete;                  // Move construct
  MaximaData& operator=(MaximaData const&) = delete;  // Copy assign
  MaximaData& operator=(MaximaData &&) = delete;      // Move assign

  void Add(const mitk::DataNode* node);
  void AddPeaksMapper(mitk::DataNode* node);
  Imeka::Fiber::Maxima& Get(const mitk::DataNode*);
  void Remove(const mitk::DataNode* node);

signals:
  void Reloaded();

public slots:
  void ReloadAll();

protected:
  MaximaData() {}
  ~MaximaData() {}

private:
  std::map<const mitk::DataNode*, Imeka::Fiber::Maxima> m_Maximas;
};

} // namespace Fiber

} // namespace Imeka

#endif // IMEKA_FIBER_MAXIMA_DATA_HPP_INCLUDED
