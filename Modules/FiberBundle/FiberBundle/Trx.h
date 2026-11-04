#ifndef IMEKA_TRX_READER_HPP_INCLUDED
#define IMEKA_TRX_READER_HPP_INCLUDED

#include <string>

#include <vtkType.h>

#include "FiberBundleExports.h"

namespace mitk { class FilteredFiberBundle; }
class vtkPolyData;

// Returns the number of points read
vtkIdType FiberBundle_EXPORT ReadTrx(
  const std::string&, mitk::FilteredFiberBundle*);

// Writes the streamlines of the polydata (MITK world, LPS mm) to a .trx file
void FiberBundle_EXPORT WriteTrx(
  const std::string&, vtkPolyData*);

#endif // IMEKA_TRX_READER_HPP_INCLUDED
