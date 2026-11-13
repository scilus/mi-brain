#ifndef IMEKA_TRX_READER_HPP_INCLUDED
#define IMEKA_TRX_READER_HPP_INCLUDED

#include <map>
#include <string>
#include <vector>

#include <vtkType.h>

#include "FiberBundleExports.h"

namespace mitk { class FilteredFiberBundle; }
class vtkPolyData;

// Returns the number of points read
vtkIdType FiberBundle_EXPORT ReadTrx(
  const std::string&, mitk::FilteredFiberBundle*);

// Writes the streamlines of the polydata (MITK world, LPS mm) to a .trx file.
// Each group (name -> streamline indices in the polydata) is written as a TRX
// group. Characters that TRX can't store in a group name ('.', '/', '\\', ...)
// are replaced by '_'.
void FiberBundle_EXPORT WriteTrx(
  const std::string&,
  vtkPolyData*,
  const std::map<std::string, std::vector<vtkIdType>>& groups = {});

#endif // IMEKA_TRX_READER_HPP_INCLUDED
