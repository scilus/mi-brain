
#ifndef IMEKA_BOUNDING_OBJECT_INTERACTOR_EMITTER_HPP_INCLUDED
#define IMEKA_BOUNDING_OBJECT_INTERACTOR_EMITTER_HPP_INCLUDED

#include <QObject>

#include <mitkAffineImageCropperInteractor.h>

#include "ImekaBoundingObjectExports.h"

namespace mitk
{
  class BoundingObject;
  //class StateMachineAction;
}

namespace Imeka
{

namespace BoundingObject
{

class ImekaBoundingObject_EXPORT InteractorEmitter
  : public QObject, public mitk::AffineImageCropperInteractor
{
  Q_OBJECT

public:
  mitkClassMacro(InteractorEmitter, mitk::AffineImageCropperInteractor);
  mitkNewMacro1Param(Self, mitk::DataNode*);

signals:
  void Selected(mitk::BoundingObject *);
  void HasBeenChanged(mitk::BoundingObject *);
  void Unselected(mitk::BoundingObject *);

protected:
  InteractorEmitter(mitk::DataNode*);

  virtual bool CheckOverObject(const mitk::InteractionEvent*) override;
  virtual void SelectObject(
    mitk::StateMachineAction*, mitk::InteractionEvent*) override;
  virtual void DeselectObject(
    mitk::StateMachineAction*, mitk::InteractionEvent*) override;
  virtual void InitTranslate(mitk::StateMachineAction*, mitk::InteractionEvent*) override;
  virtual void InitRotate(mitk::StateMachineAction*, mitk::InteractionEvent*) override;
  virtual void InitDeformation(mitk::StateMachineAction*, mitk::InteractionEvent*) override;
  virtual void TranslateObject(
    mitk::StateMachineAction*, mitk::InteractionEvent*) override;
  virtual void RotateObject(
    mitk::StateMachineAction*, mitk::InteractionEvent*) override;
  virtual void DeformObject(
    mitk::StateMachineAction*, mitk::InteractionEvent*) override;

private:
  mitk::BoundingObject* m_BoundingObject;
  bool m_NodeIsSelected;
  bool m_NodeIsInited;
};

} // namespace BoundingObject

} // namespace Imeka

#endif // IMEKA_BOUNDING_OBJECT_INTERACTOR_EMITTER_HPP_INCLUDED
