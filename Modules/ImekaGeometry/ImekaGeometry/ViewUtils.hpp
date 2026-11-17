
#ifndef IMEKA_GEOMETRY_VIEW_UTILS_HPP_INCLUDED
#define IMEKA_GEOMETRY_VIEW_UTILS_HPP_INCLUDED

#include <vector>

#include <mitkSliceNavigationController.h>
#include <mitkAnatomicalPlanes.h>

#include <ImekaGeometryExports.h>

class vtkCamera;
namespace mitk { class BaseRenderer; };

namespace Imeka
{

namespace View
{

typedef mitk::AnatomicalPlane ViewDirection;

mitk::BaseRenderer IMEKAGEOMETRY_EXPORT* GetRenderer(const ViewDirection);
mitk::BaseRenderer IMEKAGEOMETRY_EXPORT* GetRenderer(const std::string&);
mitk::BaseRenderer IMEKAGEOMETRY_EXPORT* GetAxialRenderer();
mitk::BaseRenderer IMEKAGEOMETRY_EXPORT* GetSagittalRenderer();
mitk::BaseRenderer IMEKAGEOMETRY_EXPORT* GetCoronalRenderer();
std::vector<mitk::BaseRenderer*> IMEKAGEOMETRY_EXPORT Get2DRenderers();
std::vector<vtkCamera*> IMEKAGEOMETRY_EXPORT Get2DCameras();
mitk::BaseRenderer IMEKAGEOMETRY_EXPORT* Get3DRenderer();

//Do NOT use these function if you have access to StdMultiWidget
mitk::Point3D IMEKAGEOMETRY_EXPORT GetCrosshairPosition();
void IMEKAGEOMETRY_EXPORT SetCrosshairPosition(const mitk::Point3D&);

} // namespace View

} // namespace Imeka

#endif // IMEKA_GEOMETRY_VIEW_UTILS_HPP_INCLUDED
