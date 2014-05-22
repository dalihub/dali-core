#ifndef __DALI_INTERNAL_OBJECT_REGISTRY_H__
#define __DALI_INTERNAL_OBJECT_REGISTRY_H__

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
#ifdef DEBUG_ENABLED
#include <dali/public-api/common/set-wrapper.h>
#endif
#include <dali/public-api/object/ref-object.h>
#include <dali/public-api/object/object-registry.h>
#include <dali/public-api/object/base-object.h>

namespace Dali
{

struct Vector2;

namespace Internal
{

namespace SceneGraph
{
class UpdateManager;
}

class ObjectRegistry;

typedef IntrusivePtr<ObjectRegistry> ObjectRegistryPtr;

/**
 * The ObjectRegistry notifies it's observers when an object is created.
 * There is a single instance of Object registry for each Dali-core instance.
 * All Dali Objects need to register with ObjectRegistry to be observed by
 * feedback plugin's and other observers.
 */
class ObjectRegistry : public BaseObject
{
public:

  /**
   * Create the objectRegistry
   */
  static ObjectRegistryPtr New();

  /**
   * Registers the Object into the Object Registry, Which notifies
   * about this object creation to its observers.
   * @pre The object is not already registered.
   * @param[in] object Pointer to the object.
   */
  void RegisterObject( Dali::BaseObject* object );

  /**
   * Unregisters the Object from the Object Registry, Which notifies
   * about this object destruction to its observers.
   * @pre The object is already registered.
   * @param[in] object Pointer to the object.
   */
  void UnregisterObject( Dali::BaseObject* object );

  /**
   * @copydoc Dali::ObjectRegistry::ObjectCreatedSignal()
   */
  Dali::ObjectRegistry::ObjectCreatedSignalV2& ObjectCreatedSignal()
  {
    return mObjectCreatedSignalV2;
  }

  /**
   * @copydoc Dali::ObjectRegistry::ObjectDestroyedSignal()
   */
  Dali::ObjectRegistry::ObjectDestroyedSignalV2& ObjectDestroyedSignal()
  {
    return mObjectDestroyedSignalV2;
  }

  /**
   * Connects a callback function with the object registry signals.
   * @param[in] object The object providing the signal.
   * @param[in] tracker Used to disconnect the signal.
   * @param[in] signalName The signal to connect to.
   * @param[in] functor A newly allocated FunctorDelegate.
   * @return True if the signal was connected.
   * @post If a signal was connected, ownership of functor was passed to CallbackBase. Otherwise the caller is responsible for deleting the unused functor.
   */
  static bool DoConnectSignal( BaseObject* object, ConnectionTrackerInterface* tracker, const std::string& signalName, FunctorDelegate* functor );

private:

  /**
   * Protected constructor; see also ObjectRegistry::New()
   */
  ObjectRegistry();

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  ~ObjectRegistry();

private:

  Dali::ObjectRegistry::ObjectCreatedSignalV2 mObjectCreatedSignalV2;
  Dali::ObjectRegistry::ObjectDestroyedSignalV2 mObjectDestroyedSignalV2;

#ifdef DEBUG_ENABLED
  std::set< Dali::BaseObject* > mDebugRegistry; ///< This allows us to assert that an object is only registered once (debug builds only)
#endif

};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::ObjectRegistry& GetImplementation(Dali::ObjectRegistry& objectRegistry)
{
  DALI_ASSERT_ALWAYS( objectRegistry && "ObjectRegistry handle is empty" );

  BaseObject& handle = objectRegistry.GetBaseObject();

  return static_cast<Internal::ObjectRegistry&>(handle);
}

inline const Internal::ObjectRegistry& GetImplementation(const Dali::ObjectRegistry& objectRegistry)
{
  DALI_ASSERT_ALWAYS( objectRegistry && "ObjectRegistry handle is empty" );

  const BaseObject& handle = objectRegistry.GetBaseObject();

  return static_cast<const Internal::ObjectRegistry&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_OBJECT_REGISTRY_H__
