#ifndef __DALI_OBJECT_REGISTRY_H__
#define __DALI_OBJECT_REGISTRY_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/handle.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{
/**
 * @addtogroup dali_core_object
 * @{
 */

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
 * should only be used to connect to signals.
 *
 * Usage:
 * ObjectRegistry registry = Stage::GetObjectRegistry();
 * registry.ObjectCreatedSignal().Connect( ObjectCreatedCallbackFunc );
 *
 * Signals
 * | %Signal Name     | Method                       |
 * |------------------|------------------------------|
 * | objectCreated    | @ref ObjectCreatedSignal()   |
 * | objectDestroyed  | @ref ObjectDestroyedSignal() |
 * @SINCE_1_0.0
 */
class DALI_CORE_API ObjectRegistry : public BaseHandle
{
public:

  // Typedefs

  /**
   * @brief Object created signal.
   * @SINCE_1_0.0
   */
  typedef Signal< void ( BaseHandle ) > ObjectCreatedSignalType;

  /**
   * @brief Object destroyed signal.
   * @SINCE_1_0.0
   */
  typedef Signal< void ( const Dali::RefObject* ) > ObjectDestroyedSignalType;

  /**
   * @brief Allows the creation of an empty objectRegistry handle.
   *
   * To retrieve the current objectRegistry,
   * this handle can be set using Stage::GetCurrent().GetObjectRegistry().
   * @SINCE_1_0.0
   */
  ObjectRegistry();

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~ObjectRegistry();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] handle A reference to the copied handle
   */
  ObjectRegistry(const ObjectRegistry& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param[in] rhs A reference to the copied handle
   * @return A reference to this
   */
  ObjectRegistry& operator=(const ObjectRegistry& rhs);

public: // Signals

  /**
   * @brief This signal is emitted when an object is created.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName(BaseHandle object);
   * @endcode
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The Object has been initialized.
   */
  ObjectCreatedSignalType& ObjectCreatedSignal();

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
   * @SINCE_1_0.0
   * @return The signal to connect to
   * @pre The Object has been initialized.
   */
  ObjectDestroyedSignalType& ObjectDestroyedSignal();

public: // Not intended for application developers

  /// @cond internal
  /**
   * @brief This constructor is used by Dali Get() method.
   *
   * @SINCE_1_0.0
   * @param[in] objectRegistry A pointer to a Dali resource
   */
  explicit DALI_INTERNAL ObjectRegistry(Internal::ObjectRegistry* objectRegistry);
  /// @endcond
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_OBJECT_REGISTRY_H__
