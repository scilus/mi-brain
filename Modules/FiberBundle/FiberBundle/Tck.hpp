#ifndef IMEKA_TCK_READER_WRITER_HPP_INCLUDED
#define IMEKA_TCK_READER_WRITER_HPP_INCLUDED

#include <string>

#include "FiberBundleExports.h"

namespace mitk { class FilteredFiberBundle; }
class vtkPolyData;

int FiberBundle_EXPORT ReadTck(
  const std::string&, mitk::FilteredFiberBundle*);

std::string GetHeader(vtkPolyData*);
void FiberBundle_EXPORT WriteTck(
  const std::string&, vtkPolyData*);

#endif // IMEKA_TCK_READER_WRITER_HPP_INCLUDED
