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

#include <dali/internal/event/common/object-impl.h>
#include <dali/internal/event/animation/property-constraint.h>
#include <dali/internal/event/animation/property-input-accessor.h>
#include <dali/internal/event/animation/property-input-indexer.h>
#include <dali/internal/update/common/property-base.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/scene-graph-property-notification.h>
#include <dali/internal/update/common/property-condition-functions.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

PropertyNotification* PropertyNotification::New(Object& object,
                                                Property::Index propertyIndex,
                                                Property::Type propertyType,
                                                int componentIndex,
                                                ConditionType condition,
                                                RawArgumentContainer& arguments,
                                                NotifyMode notifyMode)
{
  return new PropertyNotification( object, propertyIndex, propertyType, componentIndex, condition, arguments, notifyMode );
}


PropertyNotification::PropertyNotification(Object& object,
                                           Property::Index propertyIndex,
                                           Property::Type propertyType,
                                           int componentIndex,
                                           ConditionType condition,
                                           RawArgumentContainer& arguments,
                                           NotifyMode notifyMode)
: mObject(&object),
  mPropertyIndex(propertyIndex),
  mPropertyType(propertyType),
  mProperty(NULL),
  mComponentIndex(componentIndex),
  mConditionType(condition),
  mArguments(arguments),
  mValid(false),
  mNotifyMode( Dali::PropertyNotification::Disabled )
{
  SetNotifyMode(notifyMode);

  switch(mConditionType)
  {
    case PropertyCondition::LessThan:
    {
      mConditionFunction = LessThan::GetFunction(mPropertyType);
      break;
    }
    case PropertyCondition::GreaterThan:
    {
      mConditionFunction = GreaterThan::GetFunction(mPropertyType);
      break;
    }
    case PropertyCondition::Inside:
    {
      mConditionFunction = Inside::GetFunction(mPropertyType);
      break;
    }
    case PropertyCondition::Outside:
    {
      mConditionFunction = Outside::GetFunction(mPropertyType);
      break;
    }
    case PropertyCondition::Step:
    {
      mConditionFunction = Step::GetFunction(mPropertyType);
      break;
    }
    case PropertyCondition::VariableStep:
    {
      mConditionFunction = VariableStep::GetFunction(mPropertyType);
      break;
    }
    case PropertyCondition::False:
    {
      mConditionFunction = PropertyNotification::EvalFalse;
      break;
    }
  }

  mProperty = mObject->GetSceneObjectInputProperty( mPropertyIndex );
  int internalComponentIndex = mObject->GetPropertyComponentIndex(mPropertyIndex);
  if( internalComponentIndex != Property::INVALID_COMPONENT_INDEX )
  {
    // override the one passed in
    mComponentIndex = internalComponentIndex;
  }
}

PropertyNotification::~PropertyNotification()
{
}

bool PropertyNotification::EvalFalse( const Dali::PropertyInput& value, RawArgumentContainer& arg )
{
  return false;
}

void PropertyNotification::SetNotifyMode( NotifyMode notifyMode )
{
  mNotifyMode = notifyMode;
}

bool PropertyNotification::Check( BufferIndex bufferIndex )
{
  bool notifyRequired = false;
  bool currentValid = false;

  if ( Property::INVALID_COMPONENT_INDEX != mComponentIndex )
  {
    // Evaluate Condition
    const PropertyInputAccessor component( mProperty, mComponentIndex );
    const PropertyInputIndexer< PropertyInputAccessor > input( bufferIndex, &component );
    currentValid = mConditionFunction(input, mArguments);
  }
  else
  {
    // Evaluate Condition
    const PropertyInputIndexer< PropertyInputImpl > input( bufferIndex, mProperty );
    currentValid = mConditionFunction(input, mArguments);
  }

  if( mValid != currentValid
      || (currentValid && ((mConditionType == PropertyCondition::Step)
                        || (mConditionType == PropertyCondition::VariableStep))) )
  {
    mValid = currentValid;
    //  means don't notify so notifyRequired stays false
    switch( mNotifyMode )
    {
      case Dali::PropertyNotification::Disabled:
      {
        // notify never, already initialized to false
        break;
      }
      case Dali::PropertyNotification::NotifyOnTrue:
      {
        notifyRequired = mValid; // notify if value is true
        break;
      }
      case Dali::PropertyNotification::NotifyOnFalse:
      {
        notifyRequired = !mValid; // notify when its false
        break;
      }
      case Dali::PropertyNotification::NotifyOnChanged:
      {
        notifyRequired = true; // notify whenever changed
        break;
      }
    }
  }

  return notifyRequired;
}

bool PropertyNotification::GetValidity() const
{
  return mValid;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
