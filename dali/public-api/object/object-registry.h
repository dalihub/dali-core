#ifndef __DALI_OBJECT_REGISTRY_H__
#define __DALI_OBJECT_REGISTRY_H__

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

// EXTERNAL INCLUDES
#include <boost/function.hpp>

// INTERNAL INCLUDES
#include <dali/public-api/object/handle.h>
#include <dali/public-api/signals/dali-signal-v2.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class ObjectRegistry;
}

/**
 * @brief The ObjectRegistry notifies it's observers when an object is created.
 *
 * Handle to the created Object is passed in the call back function.
 * The Handle is passed as Dali::Object handle, which can be DownCast
 * to the appropriate type.
 *
 * Care should be taken to not store the handle in the Observer, as this will
 * have adverse effect on the life time of the Internal Object. The Handle
 * should only be used to connect to signals
 *
 * Usage:
 * ObjectRegistry registry = Stage::GetObjectRegistry();
 * registry.ObjectCreatedSignal().Connect( ObjectCreatedCallbackFunc );
 *
 */
class ObjectRegistry : public BaseHandle
{
public:

  //Signal Names
  static const char* const SIGNAL_OBJECT_CREATED;   ///< Created signal name
  static const char* const SIGNAL_OBJECT_DESTROYED; ///< Destroyed signal name

  // Typedefs

  /**
   * @brief Object created signal
   */
  typedef SignalV2<  void (BaseHandle) > ObjectCreatedSignalV2;

  /**
   * @brief Object destroyed signal
   */
  typedef SignalV2<  void (const Dali::RefObject*) > ObjectDestroyedSignalV2;

  /**
   * @brief Allows the creation of an empty objectRegistry handle.
   *
   * To retrieve the current objectRegistry,
   * this handle can be set using Stage::GetCurrent().GetObjectRegistry().
   */
  ObjectRegistry();

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ObjectRegistry();

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

public: // Signals

  /**
   * @brief This signal is emitted when an object is created.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(BaseHandle object);
   * @endcode
   * @pre The Object has been initialized.
   * @return The signal to connect to.
   */
  ObjectCreatedSignalV2& ObjectCreatedSignal();

  /**
   * @brief This signal is emitted when an object is destroyed.
   *
   * WARNING: Since this signal is emitted when the object is
   * in the process of being destroyed, the RefObject pointer
   * passed in the signal should not be modified in anyways.
   * And should NOT be used to create an handle. which will
   * affect the life time of this destroyed object and leads to
   * undefined behaviour.
   *
   * The only intended use is for Toolkit controls which want to
   * keep track of objects being created and destroyed for internal
   * bookkeeping.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(const Dali::RefObject* objectPointer);
   * @endcode
   * @pre The Object has been initialized.
   * @return The signal to connect to.
   */
  ObjectDestroyedSignalV2& ObjectDestroyedSignal();

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali Get() method.
   *
   * @param [in] objectRegistry A pointer to a Dali resource
   */
  explicit DALI_INTERNAL ObjectRegistry(Internal::ObjectRegistry* objectRegistry);
};

} // namespace Dali

#endif // __DALI_OBJECT_REGISTRY_H__
