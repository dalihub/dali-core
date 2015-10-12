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

// CLASS HEADER
#include <dali/internal/event/animation/constraint-base.h>

// INTERNAL INCLUDES
#include <dali/public-api/object/handle.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/event/common/property-helper.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner-messages.h>

using Dali::Internal::SceneGraph::AnimatableProperty;

namespace Dali
{

namespace Internal
{

ConstraintBase::ConstraintBase( Object& object, Property::Index targetPropertyIndex, SourceContainer& sources )
: mEventThreadServices( *Stage::GetCurrent() ),
  mTargetObject( &object ),
  mSceneGraphConstraint( NULL ),
  mSources( sources ),
  mObservedObjects(),
  mTargetPropertyIndex( targetPropertyIndex ),
  mRemoveAction( Dali::Constraint::DEFAULT_REMOVE_ACTION ),
  mTag( 0 ),
  mApplied( false ),
  mSourceDestroyed( false )
{
  ObserveObject( object );
}

ConstraintBase::~ConstraintBase()
{
  StopObservation();

  RemoveInternal();
}

void ConstraintBase::AddSource( Source source )
{
  mSources.push_back( source );

  // Observe the object providing this property
  if ( OBJECT_PROPERTY == source.sourceType )
  {
    if ( source.object != NULL )
    {
      ObserveObject( *source.object );
    }
    else
    {
      DALI_LOG_ERROR( "Constraint source object not found" );
    }
  }
}

void ConstraintBase::Apply()
{
  if ( mTargetObject && !mApplied && !mSourceDestroyed )
  {
    mApplied = true;
    ConnectConstraint();

    mTargetObject->ApplyConstraint( *this );
  }
}

void ConstraintBase::Remove()
{
  RemoveInternal();

  if( mTargetObject )
  {
    mTargetObject->RemoveConstraint( *this );
  }
}

void ConstraintBase::RemoveInternal()
{
  if ( mApplied )
  {
    mApplied = false;

    // Guard against constraint sending messages during core destruction
    if( Stage::IsInstalled() )
    {
      const SceneGraph::PropertyOwner* propertyOwner = mTargetObject ? mTargetObject->GetSceneObject() : NULL;

      if ( propertyOwner &&
           mSceneGraphConstraint )
      {
        // Remove from scene-graph
        RemoveConstraintMessage( GetEventThreadServices(), *propertyOwner, *(mSceneGraphConstraint) );

        // mSceneGraphConstraint will be deleted in update-thread, remove dangling pointer
        mSceneGraphConstraint = NULL;
      }
    }
  }
}

Object* ConstraintBase::GetParent()
{
  return mTargetObject;
}

Dali::Handle ConstraintBase::GetTargetObject()
{
  return Dali::Handle( mTargetObject );
}

Property::Index ConstraintBase::GetTargetProperty()
{
  return mTargetPropertyIndex;
}

void ConstraintBase::SetRemoveAction( ConstraintBase::RemoveAction action )
{
  mRemoveAction = action;
}

ConstraintBase::RemoveAction ConstraintBase::GetRemoveAction() const
{
  return mRemoveAction;
}

void ConstraintBase::SetTag(const unsigned int tag)
{
  mTag = tag;
}

unsigned int ConstraintBase::GetTag() const
{
  return mTag;
}

void ConstraintBase::SceneObjectAdded( Object& object )
{
  if ( mApplied &&
       ( NULL == mSceneGraphConstraint ) &&
       mTargetObject )
  {
    ConnectConstraint();
  }
}

void ConstraintBase::SceneObjectRemoved( Object& object )
{
  if ( mSceneGraphConstraint )
  {
    // An input property owning source has been deleted, need to tell the scene-graph-constraint owner to remove it
    if ( &object != mTargetObject )
    {
      const SceneGraph::PropertyOwner* propertyOwner = mTargetObject ? mTargetObject->GetSceneObject() : NULL;

      if( propertyOwner )
      {
        // Remove from scene-graph
        RemoveConstraintMessage( GetEventThreadServices(), *propertyOwner, *(mSceneGraphConstraint) );
      }
    }

    // mSceneGraphConstraint will be deleted in update-thread, remove dangling pointer
    mSceneGraphConstraint = NULL;
  }
}

void ConstraintBase::ObjectDestroyed( Object& object )
{
  // Remove object pointer from observation set
  ObjectIter iter = std::find( mObservedObjects.Begin(), mObservedObjects.End(), &object );
  DALI_ASSERT_DEBUG( mObservedObjects.End() != iter );
  mObservedObjects.Erase( iter );

  // Constraint is not useful anymore as an input-source has been destroyed
  mSourceDestroyed = true;

  // Stop observing the remaining objects
  StopObservation();

  // Clear our sources as well
  mSources.clear();

  // Discard all object & scene-graph pointers
  mSceneGraphConstraint = NULL;
  mTargetObject = NULL;
}

void ConstraintBase::ObserveObject( Object& object )
{
  ObjectIter iter = std::find( mObservedObjects.Begin(), mObservedObjects.End(), &object );
  if ( mObservedObjects.End() == iter )
  {
    object.AddObserver( *this );
    mObservedObjects.PushBack( &object );
  }
}

void ConstraintBase::StopObservation()
{
  const ObjectIter end = mObservedObjects.End();
  for( ObjectIter iter = mObservedObjects.Begin(); iter != end; ++iter )
  {
    (*iter)->RemoveObserver( *this );
  }

  mObservedObjects.Clear();
}

} // namespace Internal

} // namespace Dali
