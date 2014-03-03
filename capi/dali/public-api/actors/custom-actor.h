#ifndef __DALI_CUSTOM_ACTOR_H__
#define __DALI_CUSTOM_ACTOR_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_FRAMEWORK
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/actors/custom-actor-impl.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class CustomActor;
}

/**
 * CustomActor is a base class for custom UI controls.
 * The implementation of the control must be supplied; see CustomActorImpl for more details.
 */
class DALI_IMPORT_API CustomActor : public Actor
{
public:

  /**
   * Create an uninitialized CustomActor handle. Only derived versions can be instantiated.
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  CustomActor();

  /**
   * Downcast an Object handle to CustomActor. If handle points to a CustomActor the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a CustomActor or an uninitialized handle
   */
  static CustomActor DownCast( BaseHandle handle );

  /**
   * Virtual destructor.
   * Dali::Object derived classes do not contain member data.
   */
  virtual ~CustomActor();

  /**
   * Retrieve the custom actor implementation.
   * @return The implementation.
   */
  CustomActorImpl& GetImplementation();

  /**
   * Retrieve the custom actor implementation.
   * @return The implementation.
   */
  const CustomActorImpl& GetImplementation() const;

  /**
   * Create an initialised CustomActor.
   * @param[in] implementation The implementation for this custom actor.
   * @return A handle to a newly allocated Dali resource.
   */
  CustomActor(CustomActorImpl& implementation);

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

public: // Not intended for application developers

  /**
   * This constructor is used internally to create additional CustomActor handles.
   * @param [in] actor A pointer to a newly allocated Dali resource
   */
  CustomActor(Internal::CustomActor* actor);
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_CUSTOM_ACTOR_H__
