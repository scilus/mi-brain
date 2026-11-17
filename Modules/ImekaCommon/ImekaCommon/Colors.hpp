
#ifndef IMEKA_COMMON_COLORS_HPP_INCLUDED
#define IMEKA_COMMON_COLORS_HPP_INCLUDED

#include <QColor>

#include <vector>

#include "types.hpp"
#include "ImekaCommonExports.h"

namespace mitk { class DataNode; }

namespace Imeka
{

namespace Color
{

std::vector<QColor> IMEKACOMMON_EXPORT
GetNColors(const unsigned int);

void IMEKACOMMON_EXPORT ShuffleColors(const Nodes nodes);

void IMEKACOMMON_EXPORT
Apply(mitk::DataNode*, const QColor&);

} // namespace Color

} // namespace Imeka

#endif // IMEKA_COMMON_COLORS_HPP_INCLUDED
