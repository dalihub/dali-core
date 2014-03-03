#ifndef __DALI_CONSTRAINABLE_H__
#define __DALI_CONSTRAINABLE_H__

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
#include <dali/public-api/object/handle.h>
#include <dali/public-api/animation/active-constraint.h>
#include <dali/public-api/animation/constraint.h>

namespace Dali DALI_IMPORT_API
{

namespace Internal DALI_INTERNAL
{
class Object;
}

class Constraint;

/**
 * Dali::Constrainable is a handle to an internal property owning Dali object that
 * can have constraints applied to it.
 */
class Constrainable : public Handle
{
public:

  /**
   * This constructor provides an uninitialized Dali::Constrainable.
   * This should be initialized with a Dali New() method before use.
   * Methods called on an uninitialized Dali::Constrainable will assert.
   * @code
   * Constrainable handle; // uninitialized
   * handle.SomeMethod(); // unsafe! This will assert
   *
   * handle = SomeClass::New(); // now initialized
   * handle.SomeMethod(); // safe
   * @endcode
   */
  Constrainable();

  /**
   * Dali::Handle is intended as a base class
   */
  virtual ~Constrainable();

  /**
   * This copy constructor is required for (smart) pointer semantics
   * @param [in] handle A reference to the copied handle
   */
  Constrainable(const Constrainable& handle);

  /**
   * This assignment operator is required for (smart) pointer semantics
   * @param [in] rhs  A reference to the copied handle
   */
  Constrainable& operator=(const Handle& rhs);

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * Constrain one of the properties of an Actor.
   * @note The constraint will be copied by the Actor. This means that modifying the apply-time etc.
   * of the constraint, will not affect actors which are already being constrained.
   * @pre The Actor has been initialized.
   * @param[in] constraint The constraint to add.
   * @return The active-constraint being applied to the actor.
   */
  ActiveConstraint ApplyConstraint( Constraint constraint );

  /**
   * Remove one constraint from an Object.
   * @pre The Object has been intialized.
   * @param[in] activeConstraint The active-constraint to remove.
   */
  void RemoveConstraint(ActiveConstraint activeConstraint);

  /**
   * Remove all constraints from an Object.
   * @pre The object has been initialized.
   */
  void RemoveConstraints();

public:


  /**
   * This constructor is used by Dali New() methods.
   * @param [in] handle A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Constrainable(Dali::Internal::Object* handle);
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_CONSTRAINABLE_H__
