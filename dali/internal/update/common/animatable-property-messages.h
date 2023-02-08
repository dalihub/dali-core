#ifndef DALI_INTERNAL_COMMON_ANIMATABLE_PROPERTY_MESSAGES_H
#define DALI_INTERNAL_COMMON_ANIMATABLE_PROPERTY_MESSAGES_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/property-resetter.h>
#include <dali/internal/update/manager/update-manager.h>

namespace Dali::Internal
{
// Messages for AnimatableProperty<T>

template<class T>
void BakeMessage(EventThreadServices&                     eventThreadServices,
                 const SceneGraph::PropertyOwner&         propertyOwner,
                 const SceneGraph::AnimatableProperty<T>& property,
                 typename ParameterType<T>::PassingType   newValue)
{
  using LocalType = MessageDoubleBuffered1<SceneGraph::AnimatableProperty<T>, T>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&property,
                      &SceneGraph::AnimatableProperty<T>::Bake,
                      newValue);

  OwnerPointer<SceneGraph::PropertyResetterBase> resetter(
    new SceneGraph::BakerResetter(const_cast<SceneGraph::PropertyOwner*>(&propertyOwner),
                                  const_cast<SceneGraph::AnimatableProperty<T>*>(&property),
                                  SceneGraph::BakerResetter::Lifetime::BAKE));
  AddResetterMessage(eventThreadServices.GetUpdateManager(), resetter);
}

template<class T>
void BakeRelativeMessage(EventThreadServices&                     eventThreadServices,
                         const SceneGraph::PropertyOwner&         propertyOwner,
                         const SceneGraph::AnimatableProperty<T>& property,
                         const T&                                 delta)
{
  using LocalType = MessageDoubleBuffered1<SceneGraph::AnimatableProperty<T>, const T&>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&property,
                      &SceneGraph::AnimatableProperty<T>::BakeRelative,
                      delta);

  OwnerPointer<SceneGraph::PropertyResetterBase> resetter(
    new SceneGraph::BakerResetter(const_cast<SceneGraph::PropertyOwner*>(&propertyOwner),
                                  const_cast<SceneGraph::AnimatableProperty<T>*>(&property),
                                  SceneGraph::BakerResetter::Lifetime::BAKE));
  AddResetterMessage(eventThreadServices.GetUpdateManager(), resetter);
}

template<class T>
void SetXComponentMessage(EventThreadServices&                       eventThreadServices,
                          const SceneGraph::PropertyOwner&           propertyOwner,
                          const SceneGraph::AnimatableProperty<T>&   property,
                          typename ParameterType<float>::PassingType newValue)
{
  using LocalType = MessageDoubleBuffered1<SceneGraph::AnimatableProperty<T>, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&property,
                      &SceneGraph::AnimatableProperty<T>::BakeX,
                      newValue);

  OwnerPointer<SceneGraph::PropertyResetterBase> resetter(
    new SceneGraph::BakerResetter(const_cast<SceneGraph::PropertyOwner*>(&propertyOwner),
                                  const_cast<SceneGraph::AnimatableProperty<T>*>(&property),
                                  SceneGraph::BakerResetter::Lifetime::BAKE));
  AddResetterMessage(eventThreadServices.GetUpdateManager(), resetter);
}

template<class T>
void SetYComponentMessage(EventThreadServices&                       eventThreadServices,
                          const SceneGraph::PropertyOwner&           propertyOwner,
                          const SceneGraph::AnimatableProperty<T>&   property,
                          typename ParameterType<float>::PassingType newValue)
{
  using LocalType = MessageDoubleBuffered1<SceneGraph::AnimatableProperty<T>, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&property,
                      &SceneGraph::AnimatableProperty<T>::BakeY,
                      newValue);

  OwnerPointer<SceneGraph::PropertyResetterBase> resetter(
    new SceneGraph::BakerResetter(const_cast<SceneGraph::PropertyOwner*>(&propertyOwner),
                                  const_cast<SceneGraph::AnimatableProperty<T>*>(&property),
                                  SceneGraph::BakerResetter::Lifetime::BAKE));
  AddResetterMessage(eventThreadServices.GetUpdateManager(), resetter);
}

template<class T>
void SetZComponentMessage(EventThreadServices&                       eventThreadServices,
                          const SceneGraph::PropertyOwner&           propertyOwner,
                          const SceneGraph::AnimatableProperty<T>&   property,
                          typename ParameterType<float>::PassingType newValue)
{
  using LocalType = MessageDoubleBuffered1<SceneGraph::AnimatableProperty<T>, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&property,
                      &SceneGraph::AnimatableProperty<T>::BakeZ,
                      newValue);

  OwnerPointer<SceneGraph::PropertyResetterBase> resetter(
    new SceneGraph::BakerResetter(const_cast<SceneGraph::PropertyOwner*>(&propertyOwner),
                                  const_cast<SceneGraph::AnimatableProperty<T>*>(&property),
                                  SceneGraph::BakerResetter::Lifetime::BAKE));
  AddResetterMessage(eventThreadServices.GetUpdateManager(), resetter);
}

template<class T>
void SetWComponentMessage(EventThreadServices&                       eventThreadServices,
                          const SceneGraph::PropertyOwner&           propertyOwner,
                          const SceneGraph::AnimatableProperty<T>&   property,
                          typename ParameterType<float>::PassingType newValue)
{
  using LocalType = MessageDoubleBuffered1<SceneGraph::AnimatableProperty<T>, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(&property,
                      &SceneGraph::AnimatableProperty<T>::BakeW,
                      newValue);

  OwnerPointer<SceneGraph::PropertyResetterBase> resetter(
    new SceneGraph::BakerResetter(const_cast<SceneGraph::PropertyOwner*>(&propertyOwner),
                                  const_cast<SceneGraph::AnimatableProperty<T>*>(&property),
                                  SceneGraph::BakerResetter::Lifetime::BAKE));
  AddResetterMessage(eventThreadServices.GetUpdateManager(), resetter);
}

} // namespace Dali::Internal

#endif //DALI_INTERNAL_COMMON_ANIMATABLE_PROPERTY_MESSAGES_H
