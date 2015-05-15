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
#include <dali/internal/event/common/property-notification-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/radian.h>
#include <dali/internal/event/actors/actor-impl.h>
#include <dali/internal/event/common/property-notification-manager.h>
#include <dali/internal/event/common/object-impl.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/common/scene-graph-property-notification.h>
#include <dali/internal/event/common/thread-local-storage.h>

using Dali::Internal::SceneGraph::UpdateManager;

namespace Dali
{

namespace Internal
{

PropertyNotificationPtr PropertyNotification::New(Property& target,
                                                  int componentIndex,
                                                  const Dali::PropertyCondition& condition)
{
  ThreadLocalStorage& tls = ThreadLocalStorage::Get();

  UpdateManager& updateManager = tls.GetUpdateManager();

  PropertyNotificationManager& propertyNotificationManager = Stage::GetCurrent()->GetPropertyNotificationManager();

  PropertyNotificationPtr propertyNotification = new PropertyNotification(updateManager,
                                                                          propertyNotificationManager,
                                                                          target,
                                                                          componentIndex,
                                                                          condition);

  return propertyNotification;
}

PropertyNotification::PropertyNotification(UpdateManager& updateManager,
                                           PropertyNotificationManager& propertyNotificationManager,
                                           Property& target,
                                           int componentIndex,
                                           const Dali::PropertyCondition& condition)
: mUpdateManager(updateManager),
  mPropertyNotification(NULL),
  mPropertyNotificationManager(propertyNotificationManager),
  mObjectPropertyIndex(target.propertyIndex),
  mPropertyType(Property::NONE),
  mComponentIndex(componentIndex),
  mCondition(condition),
  mNotifyMode(Dali::PropertyNotification::NotifyOnTrue),
  mNotifyResult(false)
{
  // Set condition arguments (as simple vector of floats)
  PropertyCondition::ArgumentContainer arguments = GetImplementation(condition).arguments;
  PropertyCondition::ArgumentConstIter iter = arguments.begin();

  while( iter != arguments.end() )
  {
    const Property::Value& value = *iter;
    float floatValue;
    value.Get(floatValue);

    mRawConditionArgs.PushBack( floatValue );
    ++iter;
  }

  // Observe target object and create/destroy notification scene object accordingly.
  mObject = dynamic_cast<Object*>( &GetImplementation(target.object) );
  if ( mObject )
  {
    mPropertyType = mObject->GetPropertyType(mObjectPropertyIndex);

    int internalComponentIndex = mObject->GetPropertyComponentIndex(mObjectPropertyIndex);
    if( internalComponentIndex != Property::INVALID_COMPONENT_INDEX )
    {
      // override the one passed in
      mComponentIndex = internalComponentIndex;
    }
    if(mComponentIndex != Property::INVALID_COMPONENT_INDEX)
    {
      Property::Type type = mObject->GetPropertyType(mObjectPropertyIndex);
      if( type == Property::VECTOR2
          || type == Property::VECTOR3
          || type == Property::VECTOR4 )
      {
        mPropertyType = Property::FLOAT;
      }
    }

    // Check if target scene-object already present, and if so create our notification
    // scene-object
    const SceneGraph::PropertyOwner* object = mObject->GetSceneObject();
    if (object)
    {
      CreateSceneObject();
    }
  }

  // Connect to the property notification manager
  mPropertyNotificationManager.PropertyNotificationCreated( *this );
}

PropertyNotification::~PropertyNotification()
{
  Disable();

  // Guard to disallow use of PropertyNotificationManager after Core has been destroyed
  if ( Stage::IsInstalled() )
  {
    // Disconnect from the property notification manager
    mPropertyNotificationManager.PropertyNotificationDestroyed( *this );
  }
}

Dali::PropertyNotifySignalType& PropertyNotification::NotifySignal()
{
  return mNotifySignal;
}

void PropertyNotification::EmitSignalNotify()
{
  Dali::PropertyNotification source(this);

  mNotifySignal.Emit( source );
}

void PropertyNotification::Enable()
{
  CreateSceneObject();
}

void PropertyNotification::Disable()
{
  // Guard to allow handle destruction after Core has been destroyed
  if ( Stage::IsInstalled() )
  {
    // Stop scene-graph from monitoring the target's properties.
    DestroySceneObject();
  }
}

void PropertyNotification::SetNotifyResult( bool result )
{
  mNotifyResult = result;
}

const Dali::PropertyCondition& PropertyNotification::GetCondition() const
{
  return mCondition;
}

Dali::Handle PropertyNotification::GetTarget() const
{
  Dali::Handle handle(mObject);

  return handle;
}

Property::Index PropertyNotification::GetTargetProperty() const
{
  return mObjectPropertyIndex;
}

void PropertyNotification::SetNotifyMode( NotifyMode mode )
{
  mNotifyMode = mode;
  if( mPropertyNotification )
  {
    PropertyNotificationSetNotifyModeMessage( mUpdateManager, mPropertyNotification, mode );
  }
}

PropertyNotification::NotifyMode PropertyNotification::GetNotifyMode()
{
  return mNotifyMode;
}

bool PropertyNotification::GetNotifyResult() const
{
  return mNotifyResult;
}

bool PropertyNotification::CompareSceneObject( const SceneGraph::PropertyNotification* sceneObject )
{
  return sceneObject && sceneObject == mPropertyNotification;
}

void PropertyNotification::CreateSceneObject()
{
  // this method can be called from constructor and on stage connection
  if( !mPropertyNotification )
  {
    // Create a new PropertyNotification, temporarily owned
    SceneGraph::PropertyNotification* propertyNotification = SceneGraph::PropertyNotification::New( *mObject,
                                                                                                    mObjectPropertyIndex,
                                                                                                    mPropertyType,
                                                                                                    mComponentIndex,
                                                                                                    GetImplementation(mCondition).type,
                                                                                                    mRawConditionArgs,
                                                                                                    mNotifyMode );
    // Keep a const pointer to the PropertyNotification.
    mPropertyNotification = propertyNotification;

    // Transfer scene object ownership to the update manager through a message
    AddPropertyNotificationMessage( mUpdateManager, propertyNotification );
  }
}

void PropertyNotification::DestroySceneObject()
{
  if ( mPropertyNotification != NULL )
  {
    // Remove PropertyNotification using a message to the update manager
    RemovePropertyNotificationMessage( mUpdateManager, *mPropertyNotification );
    mPropertyNotification = NULL;
  }
}

} // namespace Internal

} // namespace Dali
