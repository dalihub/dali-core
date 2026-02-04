#ifndef DALI_INTERNAL_COMMON_ANIMATABLE_PROPERTY_MESSAGES_H
#define DALI_INTERNAL_COMMON_ANIMATABLE_PROPERTY_MESSAGES_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

/**
 * Special message to first bake a property, then create a resetter for the
 * property in the update thread to reduce load on event thread.
 * @tparam T The type of the property
 * @tparam P2 The type of the parameter to the second object method
 */
template<class T, typename P>
class MessageBakeReset : public MessageBase
{
public:
  using MemberFunction = void (T::*)(typename ParameterType<P>::PassingType);

  /**
   * Create a message.
   * @note The object is expected to be const in the thread which sends this message.
   * However it can be modified when Process() is called in a different thread.
   * @param[in] updateManager
   * @param[in] propertyOwner The owner of the property
   * @param[in] property The property to update
   * @param[in] member The member function of the property object (bake/set)
   * @param[in] p The value to update the property to
   */
  MessageBakeReset(SceneGraph::UpdateManager&             updateManager,
                   const SceneGraph::PropertyOwner&       propertyOwner,
                   const T*                               property,
                   MemberFunction                         member,
                   typename ParameterType<P>::PassingType p)
  : MessageBase(),
    mUpdateManager(updateManager),
    mPropertyOwner(propertyOwner),
    object(const_cast<T*>(property)),
    memberFunction(member),
    param(p)
  {
    DALI_ASSERT_DEBUG(property && "nullptr passed into message as property");
  }

  /**
   * Virtual destructor
   */
  ~MessageBakeReset() override = default;

  /**
   * @copydoc MessageBase::Process
   */
  void Process() override
  {
    // Bake/set the property
    (object->*memberFunction)(param);

    // Create the resetter in the Update thread.
    OwnerPointer<SceneGraph::PropertyResetterBase> resetter(
      new SceneGraph::BakerResetter(const_cast<SceneGraph::PropertyOwner*>(&mPropertyOwner),
                                    object,
                                    SceneGraph::BakerResetter::Lifetime::BAKE));
    mUpdateManager.AddPropertyResetter(resetter);
  }

private:
  SceneGraph::UpdateManager&            mUpdateManager;
  const SceneGraph::PropertyOwner&      mPropertyOwner;
  T*                                    object;
  MemberFunction                        memberFunction;
  typename ParameterType<P>::HolderType param;
};

template<typename T>
void BakeMessage(EventThreadServices&                     eventThreadServices,
                 const SceneGraph::PropertyOwner&         propertyOwner,
                 const SceneGraph::AnimatableProperty<T>& property,
                 typename ParameterType<T>::PassingType   newValue)
{
  using LocalType = MessageBakeReset<SceneGraph::AnimatableProperty<T>, T>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(eventThreadServices.GetUpdateManager(),
                      propertyOwner,
                      &property,
                      &SceneGraph::AnimatableProperty<T>::Bake,
                      newValue);
}

template<class T>
void BakeRelativeMessage(EventThreadServices&                     eventThreadServices,
                         const SceneGraph::PropertyOwner&         propertyOwner,
                         const SceneGraph::AnimatableProperty<T>& property,
                         const T&                                 delta)
{
  using LocalType = MessageBakeReset<SceneGraph::AnimatableProperty<T>, T>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(eventThreadServices.GetUpdateManager(),
                      propertyOwner,
                      &property,
                      &SceneGraph::AnimatableProperty<T>::BakeRelative,
                      delta);
}

template<class T>
void SetXComponentMessage(EventThreadServices&                       eventThreadServices,
                          const SceneGraph::PropertyOwner&           propertyOwner,
                          const SceneGraph::AnimatableProperty<T>&   property,
                          typename ParameterType<float>::PassingType newValue)
{
  using LocalType = MessageBakeReset<SceneGraph::AnimatableProperty<T>, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(eventThreadServices.GetUpdateManager(),
                      propertyOwner,
                      &property,
                      &SceneGraph::AnimatableProperty<T>::BakeX,
                      newValue);
}

template<class T>
void SetYComponentMessage(EventThreadServices&                       eventThreadServices,
                          const SceneGraph::PropertyOwner&           propertyOwner,
                          const SceneGraph::AnimatableProperty<T>&   property,
                          typename ParameterType<float>::PassingType newValue)
{
  using LocalType = MessageBakeReset<SceneGraph::AnimatableProperty<T>, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(eventThreadServices.GetUpdateManager(),
                      propertyOwner,
                      &property,
                      &SceneGraph::AnimatableProperty<T>::BakeY,
                      newValue);
}

template<class T>
void SetZComponentMessage(EventThreadServices&                       eventThreadServices,
                          const SceneGraph::PropertyOwner&           propertyOwner,
                          const SceneGraph::AnimatableProperty<T>&   property,
                          typename ParameterType<float>::PassingType newValue)
{
  using LocalType = MessageBakeReset<SceneGraph::AnimatableProperty<T>, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(eventThreadServices.GetUpdateManager(),
                      propertyOwner,
                      &property,
                      &SceneGraph::AnimatableProperty<T>::BakeZ,
                      newValue);
}

template<class T>
void SetWComponentMessage(EventThreadServices&                       eventThreadServices,
                          const SceneGraph::PropertyOwner&           propertyOwner,
                          const SceneGraph::AnimatableProperty<T>&   property,
                          typename ParameterType<float>::PassingType newValue)
{
  using LocalType = MessageBakeReset<SceneGraph::AnimatableProperty<T>, float>;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot(sizeof(LocalType));

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new(slot) LocalType(eventThreadServices.GetUpdateManager(),
                      propertyOwner,
                      &property,
                      &SceneGraph::AnimatableProperty<T>::BakeW,
                      newValue);
}

} // namespace Dali::Internal

#endif // DALI_INTERNAL_COMMON_ANIMATABLE_PROPERTY_MESSAGES_H
