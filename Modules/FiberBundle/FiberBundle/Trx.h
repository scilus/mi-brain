#ifndef IMEKA_TRX_READER_HPP_INCLUDED
#define IMEKA_TRX_READER_HPP_INCLUDED

#include <string>

#include <vtkType.h>

#include "FiberBundleExports.h"

namespace mitk { class FilteredFiberBundle; }

// Returns the number of points read
vtkIdType FiberBundle_EXPORT ReadTrx(
  const std::string&, mitk::FilteredFiberBundle*);

#endif // IMEKA_TRX_READER_HPP_INCLUDED
