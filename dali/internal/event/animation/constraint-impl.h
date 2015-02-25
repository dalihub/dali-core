#ifndef __DALI_INTERNAL_CONSTRAINT_H__
#define __DALI_INTERNAL_CONSTRAINT_H__

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
#include <dali/public-api/animation/active-constraint.h>
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/time-period.h>
#include <dali/public-api/object/base-object.h>
#include <dali/internal/event/animation/constraint-source-impl.h>

namespace Dali
{

namespace Internal
{

class ActiveConstraintBase;
class Object;

/**
 * Constraints are used to constrain a property of an object, after animations have been applied.
 */
class Constraint : public BaseObject
{
public:

  typedef Any AnyFunction;

  /**
   * Construct a new constraint which targets a property.
   * @param [in] target The index of the property to constrain.
   * @param [in] targetType The type of the constrained property.
   * @param [in] sources The objects which provide parameters for func.
   * @param [in] func The constraint function.
   * @param [in] interpolator A function used to interpolate between the start value, and the value returned by func.
   */
  Constraint( Property::Index target,
              Property::Type targetType,
              SourceContainer& sources,
              AnyFunction& func,
              AnyFunction& interpolator );

  /**
   * @copydoc Dali::Constraint::SetApplyTime()
   */
  void SetApplyTime( TimePeriod timePeriod );

  /**
   * @copydoc Dali::Constraint::GetApplyTime()
   */
  TimePeriod GetApplyTime() const;

  /**
   * @copydoc Dali::Constraint::SetAlphaFunction( AlphaFunction func )
   */
  void SetAlphaFunction( AlphaFunction func );

  /**
   * @copydoc Dali::Constraint::GetAlphaFunction()
   */
  AlphaFunction GetAlphaFunction() const;

  /**
   * @copydoc Dali::Constraint::SetRemoveAction()
   */
  void SetRemoveAction(Dali::Constraint::RemoveAction action);

  /**
   * @copydoc Dali::Constraint::GetRemoveAction()
   */
  Dali::Constraint::RemoveAction GetRemoveAction() const;

  /**
   * @copydoc Dali::Constraint::SetTag()
   */
  void SetTag( const unsigned int tag);

  /**
   * @copydoc Dali::Constraint::GetTag()
   */
  unsigned int GetTag() const;




  /**
   * Create an active constraint.
   * An active constraint is created each time the constraint is applied to an object.
   * @return A newly allocated active-constraint.
   */
  ActiveConstraintBase* CreateActiveConstraint();

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Constraint();

private:

  // Undefined
  Constraint(const Constraint&);

  // Undefined
  Constraint& operator=(const Constraint& rhs);

protected:

  Dali::ActiveConstraint mActiveConstraintTemplate; ///< Used to create active constraints

  TimePeriod mApplyTime;
};

} // namespace Internal

// Helpers for public-api forwarding methods

inline Internal::Constraint& GetImplementation(Dali::Constraint& constraint)
{
  DALI_ASSERT_ALWAYS( constraint && "Constraint handle is empty" );

  BaseObject& handle = constraint.GetBaseObject();

  return static_cast<Internal::Constraint&>(handle);
}

inline const Internal::Constraint& GetImplementation(const Dali::Constraint& constraint)
{
  DALI_ASSERT_ALWAYS( constraint && "Constraint handle is empty" );

  const BaseObject& handle = constraint.GetBaseObject();

  return static_cast<const Internal::Constraint&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_CONSTRAINT_H__
