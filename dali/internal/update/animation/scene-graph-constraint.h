#ifndef __DALI_INTERNAL_SCENE_GRAPH_CONSTRAINT_H__
#define __DALI_INTERNAL_SCENE_GRAPH_CONSTRAINT_H__

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
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/signals/callback.h>
#include <dali/internal/event/animation/property-constraint-ptr.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
#include <dali/internal/render/common/performance-monitor.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/**
 * Used to constrain a property of a scene-object.
 * The constraint function takes another scene-object property as an input.
 */
template < class PropertyType, typename PropertyAccessorType >
class Constraint : public ConstraintBase
{
public:

  typedef typename PropertyConstraintPtr< PropertyType >::Type ConstraintFunctionPtr;

  /**
   * Create a new scene-graph constraint.
   * @param[in] targetProperty The target property.
   * @param[in] ownerSet A set of property owners; func is connected to the properties provided by these objects.
   * @param[in] func The function to calculate the final constrained value.
   * @return A smart-pointer to a newly allocated constraint.
   */
  static ConstraintBase* New( const PropertyBase& targetProperty,
                              PropertyOwnerContainer& ownerContainer,
                              ConstraintFunctionPtr func )
  {
    // Scene-graph thread can edit these objects
    PropertyBase& property = const_cast< PropertyBase& >( targetProperty );

    return new Constraint< PropertyType, PropertyAccessorType >( property,
                                                                 ownerContainer,
                                                                 func );
  }

  /**
   * Virtual destructor.
   */
  virtual ~Constraint()
  {
  }

  /**
   * Query whether the constraint needs to be applied. If a constraint depends on a set of properties,
   * then it should be applied when any of those properties have changed.
   */
  bool ApplyNeeded()
  {
    if ( mFirstApply )
    {
      mFirstApply = false;
      return true;
    }

    if ( ! mTargetProperty.IsClean() ||
           mFunc->InputsChanged() )
    {
      return true;
    }

    // We don't need to reapply constraint if none of the properties changed
    return false;
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::ConstraintBase::Apply()
   */
  virtual void Apply( BufferIndex updateBufferIndex )
  {
    if ( mDisconnected )
    {
      return; // Early-out when property owners have been disconnected
    }

    if ( mFunc->InputsInitialized() &&
         ApplyNeeded() )
    {
      PropertyType current = mTargetProperty.Get( updateBufferIndex );
      mFunc->Apply( updateBufferIndex, current );

      // Optionally bake the final value
      if ( Dali::Constraint::Bake == mRemoveAction )
      {
        mTargetProperty.Bake( updateBufferIndex, current );
      }
      else
      {
        mTargetProperty.Set( updateBufferIndex, current );
      }

      INCREASE_COUNTER(PerformanceMonitor::CONSTRAINTS_APPLIED);
    }
    else
    {
      INCREASE_COUNTER(PerformanceMonitor::CONSTRAINTS_SKIPPED);
    }
  }

private:

  /**
   * @copydoc Dali::Internal::SceneGraph::Constraint::New()
   */
  Constraint( PropertyBase& targetProperty,
              PropertyOwnerContainer& ownerContainer,
              ConstraintFunctionPtr func )
  : ConstraintBase( ownerContainer ),
    mTargetProperty( &targetProperty ),
    mFunc( func )
  {
  }

  // Undefined
  Constraint( const Constraint& constraint );

  // Undefined
  Constraint& operator=( const Constraint& rhs );

  /**
   * @copydoc Dali::Internal::SceneGraph::ConstraintBase::OnDisconnect()
   */
  virtual void OnDisconnect()
  {
    // Discard target object/property pointers
    mTargetProperty.Reset();
    mFunc = NULL;
  }

protected:

  PropertyAccessorType mTargetProperty; ///< Raw-pointer to the target property. Not owned.

  ConstraintFunctionPtr mFunc;
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_CONSTRAINT_H__
