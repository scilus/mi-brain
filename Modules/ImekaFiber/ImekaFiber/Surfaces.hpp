#ifndef IMEKA_COMMON_SURFACES_HPP_INCLUDED
#define IMEKA_COMMON_SURFACES_HPP_INCLUDED

#include <mitkImage.h>
#include <mitkSurface.h>

#include "ImekaCommonExports.h"

namespace Imeka
{

namespace Surface
{

/* This file and the .cpp were in ImekaCommon because that's where they
   shoud be, but when I put them there, some tests stop running on Linux.
   Nobody knows why and it's not really important so here they are. */

mitk::Surface::Pointer ImekaCommon_EXPORT ImageToSurface(
  const mitk::Image*, const mitk::Image*);
mitk::Image::Pointer ImekaCommon_EXPORT SurfaceToImage(
  const mitk::Image*, const mitk::Surface*);

} // namespace Surface

} // namespace Imeka

#endif // IMEKA_COMMON_SURFACES_HPP_INCLUDED
