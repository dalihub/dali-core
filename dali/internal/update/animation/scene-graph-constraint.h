#ifndef DALI_INTERNAL_SCENE_GRAPH_CONSTRAINT_H
#define DALI_INTERNAL_SCENE_GRAPH_CONSTRAINT_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

  using ConstraintFunctionPtr = typename PropertyConstraintPtr< PropertyType >::Type;

  /**
   * Create a new scene-graph constraint.
   * @param[in] targetProperty The target property.
   * @param[in] ownerSet A set of property owners; func is connected to the properties provided by these objects.
   * @param[in] func The function to calculate the final constrained value.
   * @param[in] removeAction Remove action to perform when constraint is removed
   * @return A smart-pointer to a newly allocated constraint.
   */
  static ConstraintBase* New( const PropertyBase& targetProperty,
                              PropertyOwnerContainer& ownerContainer,
                              ConstraintFunctionPtr func,
                              RemoveAction removeAction )
  {
    // Scene-graph thread can edit these objects
    PropertyBase& property = const_cast< PropertyBase& >( targetProperty );

    return new Constraint< PropertyType, PropertyAccessorType >( property,
                                                                 ownerContainer,
                                                                 func,
                                                                 removeAction );
  }

  /**
   * Virtual destructor.
   */
  ~Constraint() override = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::ConstraintBase::Apply()
   */
  void Apply( BufferIndex updateBufferIndex ) override
  {
    if ( !mDisconnected )
    {
      if ( mFunc->InputsInitialized() )
      {
        PropertyType current = mTargetProperty.Get( updateBufferIndex );
        mFunc->Apply( updateBufferIndex, current );

        // Optionally bake the final value
        if ( Dali::Constraint::BAKE == mRemoveAction )
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
  }

private:

  /**
   * @copydoc Dali::Internal::SceneGraph::Constraint::New()
   */
  Constraint( PropertyBase& targetProperty,
              PropertyOwnerContainer& ownerContainer,
              ConstraintFunctionPtr func,
              RemoveAction removeAction )
  : ConstraintBase( ownerContainer, removeAction ),
    mTargetProperty( &targetProperty ),
    mFunc( func )
  {
  }

  // Undefined
  Constraint() = delete;
  Constraint( const Constraint& constraint ) = delete;
  Constraint& operator=( const Constraint& rhs ) = delete;

  /**
   * @copydoc Dali::Internal::SceneGraph::ConstraintBase::OnDisconnect()
   */
  void OnDisconnect() override
  {
    // Discard target object/property pointers
    mTargetProperty.Reset();
    mFunc = nullptr;
  }

protected:

  PropertyAccessorType mTargetProperty; ///< Raw-pointer to the target property. Not owned.

  ConstraintFunctionPtr mFunc;

};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_CONSTRAINT_H
