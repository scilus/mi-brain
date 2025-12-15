
#ifndef IMEKA_COMMON_TYPES_HPP_INCLUDED
#define IMEKA_COMMON_TYPES_HPP_INCLUDED

#include <vector>

namespace mitk { class DataNode; }

typedef std::vector<mitk::DataNode*> Nodes;
typedef std::vector<const mitk::DataNode*> ConstNodes;

#endif // IMEKA_COMMON_TYPES_HPP_INCLUDED
