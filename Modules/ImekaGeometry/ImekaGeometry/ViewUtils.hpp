
#ifndef IMEKA_GEOMETRY_VIEW_UTILS_HPP_INCLUDED
#define IMEKA_GEOMETRY_VIEW_UTILS_HPP_INCLUDED

#include <vector>

#include <mitkSliceNavigationController.h>

#include <ImekaGeometryExports.h>

class vtkCamera;
namespace mitk { class BaseRenderer; };

namespace Imeka
{

namespace View
{

typedef mitk::SliceNavigationController::ViewDirection ViewDirection;

mitk::BaseRenderer ImekaGeometry_EXPORT * GetRenderer(const ViewDirection);
mitk::BaseRenderer ImekaGeometry_EXPORT * GetRenderer(const std::string&);
mitk::BaseRenderer ImekaGeometry_EXPORT * GetAxialRenderer();
mitk::BaseRenderer ImekaGeometry_EXPORT * GetSagittalRenderer();
mitk::BaseRenderer ImekaGeometry_EXPORT * GetCoronalRenderer();
std::vector<mitk::BaseRenderer*> ImekaGeometry_EXPORT Get2DRenderers();
std::vector<vtkCamera*> ImekaGeometry_EXPORT Get2DCameras();
mitk::BaseRenderer ImekaGeometry_EXPORT * Get3DRenderer();

//Do NOT use these function if you have access to StdMultiWidget
mitk::Point3D ImekaGeometry_EXPORT GetCrosshairPosition();
void ImekaGeometry_EXPORT SetCrosshairPosition(const mitk::Point3D&);

} // namespace View

} // namespace Imeka

#endif // IMEKA_GEOMETRY_VIEW_UTILS_HPP_INCLUDED
