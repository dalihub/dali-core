#ifndef __DALI_CONSTRAINABLE_H__
#define __DALI_CONSTRAINABLE_H__

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

/**
 * @addtogroup CAPI_DALI_OBJECT_MODULE
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
 * @brief Dali::Constrainable is a handle to an internal property owning Dali object that
 * can have constraints applied to it.
 */
class Constrainable : public Handle
{
public:

  /**
   * @brief Create a constrainable object.
   *
   * @return A handle to a newly allocated object.
   */
  static Constrainable New();

  /**
   * @brief This constructor provides an uninitialized Dali::Constrainable.
   *
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
   * @brief Downcast a handle to a custom object.
   *
   * @param[in] handle The handle to cast.
   * @return A handle to a custom object or an empty handle.
   */
  static Constrainable DownCast( BaseHandle handle );

  /**
   * @brief Dali::Constrainable is intended as a base class
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~Constrainable();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  Constrainable(const Constrainable& handle);

  /**
   * @copydoc Dali::BaseHandle::operator=
   */
  using BaseHandle::operator=;

  /**
   * @brief Constrain one of the properties of an Actor.
   *
   * @note The constraint will be copied by the Actor. This means that modifying the apply-time etc.
   * of the constraint, will not affect actors which are already being constrained.
   * @pre The Actor has been initialized.
   * @param[in] constraint The constraint to apply.
   * @return The active-constraint being applied to the actor.
   */
  ActiveConstraint ApplyConstraint( Constraint constraint );

  /**
   * @brief Constrain one of the properties of an Actor, using a custom weight property.
   *
   * This overload is intended to allow a single weight property to be shared by many constraints
   * e.g. call WeightObject::New() once, and pass the return value into every call to ApplyConstraint().
   * @pre The Actor has been initialized.
   * @param[in] constraint The constraint to apply.
   * @param[in] weightObject An object which is expected to have a float property named "weight".
   * @return The active-constraint being applied to the actor.
   */
  ActiveConstraint ApplyConstraint( Constraint constraint, Constrainable weightObject );

  /**
   * @brief Remove one constraint from an Object.
   *
   * @pre The Object has been intialized.
   * @param[in] activeConstraint The active-constraint to remove.
   */
  void RemoveConstraint(ActiveConstraint activeConstraint);

  /**
   * @brief Remove all constraints from an Object.
   *
   * @pre The object has been initialized.
   */
  void RemoveConstraints();

  /**
   * @brief Remove all the constraint from the Object with a matching tag.
   *
   * @pre The Object has been intialized.
   * @param[in] tag The tag of the constraints which will be removed
   */
  void RemoveConstraints( unsigned int tag );

public:

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] handle A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL Constrainable(Dali::Internal::Object* handle);
};

namespace WeightObject
{

extern const Property::Index WEIGHT; ///< name "weight", type FLOAT

/**
 * @brief Convenience function to create an object with a custom "weight" property.
 *
 * @return A handle to a newly allocated object.
 */
Constrainable New();

} // namespace WeightObject

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_CONSTRAINABLE_H__
