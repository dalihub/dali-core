#ifndef __DALI_INTERNAL_PROPERTY_NOTIFICATION_H__
#define __DALI_INTERNAL_PROPERTY_NOTIFICATION_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
 *
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/object/property-notification.h>
#include <dali/internal/event/common/property-conditions-impl.h>

namespace Dali
{

class PropertyCondition;

namespace Internal
{

namespace SceneGraph
{
class PropertyNotification;
class UpdateManager;
}

class Actor;
class PropertyNotification;
class Object;
class PropertyNotificationManager;

typedef IntrusivePtr<PropertyNotification> PropertyNotificationPtr;

/**
 * PropertyNotification is a proxy for a SceneGraph::PropertyNotification object.
 * The UpdateManager owns the PropertyNotification object, but the lifetime of the PropertyNotification is
 * indirectly controlled by the PropertyNotification.
 */
class PropertyNotification : public BaseObject
{
public:

  typedef Dali::PropertyNotification::NotifyMode NotifyMode;
  typedef PropertyCondition::Type ConditionType;
  typedef PropertyCondition::ArgumentContainer ArgumentContainer;

  /**
   * RawArgumentContainer provides fast direct access to arguments for condition evaluation.
   */
  typedef Dali::Vector<float> RawArgumentContainer;

  /**
   * Create a new PropertyNotification object.
   * @param[in] target The target object to inspect
   * @param[in] componentIndex Index to the component of a complex property such as a Vector
   * @param[in] condition The condition for this notification.
   * @return A smart-pointer to the newly allocated PropertyNotification.
   */
  static PropertyNotificationPtr New(Property& target,
                                     int componentIndex,
                                     const Dali::PropertyCondition& condition);

public:

  /**
   * @copydoc Dali::PropertyNotification::NotifySignal()
   */
  Dali::PropertyNotifySignalType& NotifySignal();

  /**
   * Emit the Notify signal
   */
  void EmitSignalNotify();

  /**
   * Enables PropertyNotification
   */
  void Enable();

  /**
   * Disables PropertyNotification
   */
  void Disable();

  /**
   * Sets the result from the property condition test for use by applications that connect to the notification signal
   *
   * @param[in] result The result of the property condition test
   */
  void SetNotifyResult(bool result);

  /**
   * @copydoc Dali::PropertyNotification::GetCondition
   */
  const Dali::PropertyCondition& GetCondition() const;

  /**
   * @copydoc Dali::PropertyNotification::GetTarget
   */
  Dali::Handle GetTarget() const;

  /**
   * @copydoc Dali::PropertyNotification::GetTargetProperty
   */
  Property::Index GetTargetProperty() const;

  /**
   * @copydoc Dali::PropertyNotification::SetNotifyMode
   */
  void SetNotifyMode( NotifyMode mode );

  /**
   * @copydoc Dali::PropertyNotification::GetNotifyMode
   */
  NotifyMode GetNotifyMode();

  /**
   * @copydoc Dali::PropertyNotification::GetNotifyResult
   */
  bool GetNotifyResult() const;

  /**
   * Compare the passed sceneObject to the one created by this instance
   * @param[in] sceneObject The SceneGraph::PropertyNotification pointer to compare
   * @return true if sceneObject is the same as the one created by this instance
   */
   bool CompareSceneObject( const SceneGraph::PropertyNotification* sceneObject );

protected:

  /**
   * Construct a new PropertyNotification.
   * @param[in] updateManager The UpdateManager associated with this PropertyNotification.
   * @param[in] propertyNotificationManager The property notification manager object.
   * @param[in] target The target property.
   * @param[in] componentIndex Index to the component of a complex property such as a Vector
   * @param[in] condition The condition for this notification.
   */
  PropertyNotification(SceneGraph::UpdateManager& updateManager,
                       PropertyNotificationManager& propertyNotificationManager,
                       Property& target,
                       int componentIndex,
                       const Dali::PropertyCondition& condition);

  /**
   * Helper to create a scene-graph PropertyNotification
   */
  void CreateSceneObject();

  /**
   * Helper to destroy a scene-graph PropertyNotification
   */
  void DestroySceneObject();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~PropertyNotification();

private:

  // Undefined
  PropertyNotification(const PropertyNotification&);

  // Undefined
  PropertyNotification& operator=(const PropertyNotification& rhs);

protected:

  SceneGraph::UpdateManager& mUpdateManager;
  const SceneGraph::PropertyNotification* mPropertyNotification;

  Dali::PropertyNotifySignalType mNotifySignal;

private:

  PropertyNotificationManager& mPropertyNotificationManager;  ///< Reference to the property notification manager
  Object*                      mObject;                       ///< Target object, not owned by PropertyNotification.
  Property::Index              mObjectPropertyIndex;          ///< Target object's property index of interest.
  Property::Type               mPropertyType;                 ///< The type of property to evaluate
  int                          mComponentIndex;               ///< Index to a specific component of a complex property such as a Vector
  Dali::PropertyCondition      mCondition;                    ///< The PropertyCondition handle.
  RawArgumentContainer         mRawConditionArgs;             ///< The Raw Condition args. (float type)
  NotifyMode                   mNotifyMode;                   ///< The current notification mode.
  bool                         mNotifyResult;                 ///< The result of the last condition check that caused a signal emit
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::PropertyNotification& GetImplementation(Dali::PropertyNotification& pub)
{
  DALI_ASSERT_ALWAYS( pub && "PropertyNotification handle is empty" );

  BaseObject& handle = pub.GetBaseObject();

  return static_cast<Internal::PropertyNotification&>(handle);
}

inline const Internal::PropertyNotification& GetImplementation(const Dali::PropertyNotification& pub)
{
  DALI_ASSERT_ALWAYS( pub && "PropertyNotification handle is empty" );

  const BaseObject& handle = pub.GetBaseObject();

  return static_cast<const Internal::PropertyNotification&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_PROPERTY_NOTIFICATION_H__
