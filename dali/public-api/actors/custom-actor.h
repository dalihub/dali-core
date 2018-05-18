#ifndef __DALI_CUSTOM_ACTOR_H__
#define __DALI_CUSTOM_ACTOR_H__

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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/custom-actor-impl.h>

namespace Dali
{
/**
 * @addtogroup dali_core_actors
 * @{
 */

namespace Internal DALI_INTERNAL
{
class CustomActor;
}

/**
 * @brief CustomActor is a base class for custom UI controls.
 *
 * The implementation of the control must be supplied; see CustomActorImpl for more details.
 * @SINCE_1_0.0
 */
class DALI_CORE_API CustomActor : public Actor
{
public:

  /**
   * @brief Creates an uninitialized CustomActor handle.
   *
   * Only derived versions can be instantiated.
   * Calling member functions with an uninitialized CustomActor handle is not allowed.
   * @SINCE_1_0.0
   */
  CustomActor();

  /**
   * @brief Downcasts a handle to CustomActor handle.
   *
   * If the handle points to a CustomActor, the downcast produces valid handle.
   * If not, the returned handle is left uninitialized.
   *
   * @SINCE_1_0.0
   * @param[in] handle Handle to an object
   * @return Handle to a CustomActor or an uninitialized handle
   */
  static CustomActor DownCast( BaseHandle handle );

  /**
   * @brief Destructor.
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~CustomActor();

  /**
   * @brief Retrieves the custom actor implementation.
   *
   * @SINCE_1_0.0
   * @return The implementation
   */
  CustomActorImpl& GetImplementation();

  /**
   * @brief Retrieves the custom actor implementation.
   *
   * @SINCE_1_0.0
   * @return The implementation
   */
  const CustomActorImpl& GetImplementation() const;

  /**
   * @brief Creates an initialized CustomActor.
   *
   * @SINCE_1_0.0
   * @param[in] implementation The implementation for this custom actor
   * @return A handle to a newly allocated Dali resource
   */
  CustomActor(CustomActorImpl& implementation);

  /**
   * @brief Copy constructor.
   *
   * @SINCE_1_0.0
   * @param[in] copy The actor to copy
   */
  CustomActor(const CustomActor& copy);

  /**
   * @brief Assignment operator.
   *
   * @SINCE_1_0.0
   * @param[in] rhs The actor to copy
   * @return A reference to this
   */
  CustomActor& operator=(const CustomActor& rhs);

public: // Not intended for application developers

  /**
   * @brief This constructor is used internally to create additional CustomActor handles.
   *
   * @SINCE_1_0.0
   * @param[in] actor A pointer to a newly allocated Dali resource
   */
  CustomActor(Internal::CustomActor* actor);
};

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_CUSTOM_ACTOR_H__
