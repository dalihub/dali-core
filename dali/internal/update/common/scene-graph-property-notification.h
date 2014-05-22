#ifndef __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_NOTIFICATION_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_NOTIFICATION_H__

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
#include <dali/public-api/object/property-notification.h>
#include <dali/internal/event/common/property-notification-impl.h>
#include <dali/internal/update/common/property-base.h>
#include <dali/internal/common/owner-container.h>
#include <dali/internal/common/message.h>
#include <dali/internal/update/common/double-buffered.h>

namespace Dali
{

namespace Internal
{

class ProxyObject;
class PropertyNotification;

namespace SceneGraph
{

class PropertyNotification;

typedef OwnerContainer< PropertyNotification* > PropertyNotificationContainer;
typedef PropertyNotificationContainer::Iterator PropertyNotificationIter;
typedef PropertyNotificationContainer::ConstIterator PropertyNotificationConstIter;
typedef bool(*ConditionFunction)(const Dali::PropertyInput& value, Dali::Internal::PropertyNotification::RawArgumentContainer& args);

/**
 * PropertyNotifications are used to inspect properties of scene graph objects, as part of a scene
 * managers "update" phase. When a condition has been met the application receives a notification signal.
 */
class PropertyNotification
{
public:

  typedef Dali::PropertyNotification::NotifyMode NotifyMode;
  typedef Dali::Internal::PropertyNotification::ConditionType ConditionType;
  typedef Dali::Internal::PropertyNotification::RawArgumentContainer RawArgumentContainer;
  typedef const void *(*GetPropertyFunction)( const SceneGraph::PropertyBase*, int );

  /**
   * Construct a new PropertyNotification
   * @param[in] proxy The proxy for a scene-graph object to inspect.
   * @param[in] propertyIndex The index of a property provided by the object.
   * @param[in] propertyType The type of property we're inspecting.
   * @param[in] componentIndex Index to the component of a complex property such as a Vector
   * @param[in] condition The condition type (e.g. LessThan, GreaterThan...)
   * @param[in] arguments The arguments which accompany the condition.
   * @param[in] notifyMode The notification mode setting
   * @return A new PropertyNotification object.
   */
  static PropertyNotification* New(ProxyObject& proxy,
                                   Property::Index propertyIndex,
                                   Property::Type propertyType,
                                   int componentIndex,
                                   ConditionType condition,
                                   RawArgumentContainer& arguments,
                                   NotifyMode notifyMode);

  /**
   * Virtual destructor
   */
  virtual ~PropertyNotification();

  /**
   * Sets Notify Mode, whether to notify if the condition is true
   * and if the condition is false.
   *
   * @param[in] notifyMode The notification mode setting
   */
  void SetNotifyMode( NotifyMode notifyMode );

  /**
   * Check this property notification condition,
   * and if true then dispatch notification.
   * @param[in] bufferIndex The current update buffer index.
   * @return Whether the validity of this notification has changed.
   */
  bool Check( BufferIndex bufferIndex );

  /**
   * Returns the validity of the last condition check
   *
   * @return the validity
   */
  bool GetValidity() const;

protected:

  /**
   * Construct the PropertyNotification
   * @param[in] proxy The proxy for a scene-graph object to inspect.
   * @param[in] propertyIndex The index of a property provided by the object.
   * @param[in] propertyType The type of property we're inspecting.
   * @param[in] componentIndex Index to the component of a complex property such as a Vector
   * @param[in] condition The condition type (e.g. LessThan, GreaterThan...)
   * @param[in] arguments The arguments which accompany the condition.
   * @param[in] notifyMode The notification mode setting
   */
  PropertyNotification(ProxyObject& proxy,
                       Property::Index propertyIndex,
                       Property::Type propertyType,
                       int componentIndex,
                       ConditionType condition,
                       RawArgumentContainer& arguments,
                       NotifyMode notifyMode);

private:

  /**
   * Checks if bool is LessThan
   * @param[in] value The value being examined.
   * @param[in] arg The supplied arguments for the condition.
   * @return Condition result (true if condition met, false if not)
   */
  static bool EvalFalse( const Dali::PropertyInput& value, Dali::Internal::PropertyNotification::RawArgumentContainer& arg );

  // Undefined
  PropertyNotification(const PropertyNotification&);

  // Undefined
  PropertyNotification& operator=(const PropertyNotification& rhs);

protected:

  ProxyObject* mProxy;                          ///< Not owned by the property notification. Valid until ProxyDestroyed() is called.
  Property::Index mPropertyIndex;               ///< The index of this property.
  Property::Type mPropertyType;                 ///< The type of property this is.
  const PropertyInputImpl* mProperty;           ///< The scene graph property
  int mComponentIndex;                          ///< Used for accessing float components of Vector3/4
  ConditionType mConditionType;                 ///< The ConditionType
  RawArgumentContainer mArguments;              ///< The arguments.
  bool mValid;                                  ///< Whether this property notification is currently valid or not.
  char mNotifyValidity[2];                      ///< Whether to notify on invalid and/or valid
  ConditionFunction mConditionFunction;         ///< The Condition Function pointer to be evaluated.
};

// Messages for PropertyNotification

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_PROPERTY_NOTIFICATION_H__
