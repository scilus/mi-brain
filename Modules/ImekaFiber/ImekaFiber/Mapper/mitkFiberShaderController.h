/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#ifndef mitkFiberShaderController_h
#define mitkFiberShaderController_h

#include <vtkCommand.h>
#include <mitkCommon.h>
#include <mitkBaseRenderer.h>
#include <mitkDataNode.h>

#include "ImekaFiberExports.h"

namespace mitk {

  /**
   * @brief Controller for VTK 9.4 shaders for 2D fiber slicing.
   *
   * This class handles the UpdateShaderEvent to inject uniforms and provides
   * GLSL logic for vertex and fragment shaders.
   */
  class ImekaFiber_EXPORT mitkFiberShaderController : public vtkCommand
  {
  public:
    static mitkFiberShaderController* New();
    vtkTypeMacro(mitkFiberShaderController, vtkCommand)

    /**
     * @brief Execute the command.
     *
     * This method is called when the UpdateShaderEvent is fired.
     * It sets the uniforms for the shader program.
     */
    void Execute(vtkObject* caller, unsigned long eventId, void* callData) override;

    void SetRenderer(mitk::BaseRenderer* renderer);
    void SetDataNode(mitk::DataNode* node);

    static std::string GetVertexShaderCode();
    static std::string GetFragmentShaderCode();

  protected:
    mitkFiberShaderController();
    ~mitkFiberShaderController() override;

  private:
    mitk::BaseRenderer* m_Renderer;
    mitk::DataNode* m_DataNode;
  };

} // namespace mitk

#endif // mitkFiberShaderController_h
