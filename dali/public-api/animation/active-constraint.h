#ifndef __DALI_ACTIVE_CONSTRAINT_H__
#define __DALI_ACTIVE_CONSTRAINT_H__

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
#include <dali/public-api/animation/active-constraint-declarations.h>
#include <dali/public-api/object/any.h>
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/property.h>

namespace Dali
{

namespace Internal DALI_INTERNAL
{
class ActiveConstraintBase;
}

/**
 * @brief A constraint which is being applied to an object.
 */
class DALI_IMPORT_API ActiveConstraint : public Handle
{
public:

  // Default Properties
  static const Property::Index WEIGHT; ///< Property 0, name "weight", type FLOAT

  static const float FINAL_WEIGHT;   ///< 1.0f means the constraint is fully-applied, unless weight is still being animated
  static const float DEFAULT_WEIGHT; ///< 1.0f

  //Signal Names
  static const char* const SIGNAL_APPLIED; ///< name "applied"

  /**
   * @brief Create an uninitialized Constraint; this can be initialized with Constraint::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  ActiveConstraint();

  /**
   * @brief Dali::ActiveConstraint is intended as a base class
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~ActiveConstraint();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  ActiveConstraint(const ActiveConstraint& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  ActiveConstraint& operator=(const ActiveConstraint& rhs);

  /**
   * @brief This method is defined to allow assignment of the NULL value,
   * and will throw an exception if passed any other value.
   *
   * Assigning to NULL is an alias for Reset().
   * @param [in] rhs  A NULL pointer
   * @return A reference to this handle
   */
  ActiveConstraint& operator=(BaseHandle::NullType* rhs);

  /**
   * @brief Retrieve the object which this constraint is targeting.
   *
   * @return The target object.
   */
  Handle GetTargetObject();

  /**
   * @brief Retrieve the property which this constraint is targeting.
   *
   * @return The target property.
   */
  Property::Index GetTargetProperty();

  /**
   * @brief Set the weight of the constraint; this is a value clamped between 0.
   * 0f and 1.0f.
   * The default is 1.0f.
   * 0.0f means the constraint has no effect, and 1.0f means the constraint is fully-applied.
   * @note This is an asynchronous method; the value written may not match a value subsequently read with GetCurrentWeight().
   * @param [in] weight The weight.
   */
  void SetWeight( float weight );

  /**
   * @brief Retrieve the current weight of the constraint.
   *
   * @return The current weight.
   */
  float GetCurrentWeight() const;

  /**
   * @brief This signal is emitted after the apply-time of the constraint has expired.
   *
   * @note A non-zero apply-time must have been set using Constraint::SetApplyTime().
   * @return A signal object to Connect() with.
   */
  ActiveConstraintSignalV2& AppliedSignal();

public: // Not intended for use by Application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in] constraint A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL ActiveConstraint( Internal::ActiveConstraintBase* constraint );
};

} // namespace Dali

#endif // __DALI_ACTIVE_CONSTRAINT_H__
