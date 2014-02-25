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

#include <dali/internal/event/common/proxy-object.h>
#include <dali/internal/event/animation/property-constraint.h>
#include <dali/internal/event/animation/property-input-accessor.h>
#include <dali/internal/event/animation/property-input-indexer.h>
#include <dali/internal/update/common/property-base.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/scene-graph-property-notification.h>
#include <dali/internal/update/common/property-condition-functions.h>

using namespace std;

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

PropertyNotification* PropertyNotification::New(ProxyObject& proxy,
                                                Property::Index propertyIndex,
                                                Property::Type propertyType,
                                                int componentIndex,
                                                ConditionType condition,
                                                const RawArgumentContainer& arguments,
                                                NotifyMode notifyMode)
{
  return new PropertyNotification( proxy, propertyIndex, propertyType, componentIndex, condition, arguments, notifyMode );
}


PropertyNotification::PropertyNotification(ProxyObject& proxy,
                                           Property::Index propertyIndex,
                                           Property::Type propertyType,
                                           int componentIndex,
                                           ConditionType condition,
                                           const RawArgumentContainer& arguments,
                                           NotifyMode notifyMode)
: mProxy(&proxy),
  mPropertyIndex(propertyIndex),
  mPropertyType(propertyType),
  mProperty(NULL),
  mComponentIndex(componentIndex),
  mConditionType(condition),
  mArguments(arguments),
  mValid(false)
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
    case PropertyCondition::False:
    {
      mConditionFunction = PropertyNotification::EvalFalse;
      break;
    }
    default:
    {
      DALI_ASSERT_ALWAYS(false && "Unrecognized ConditionType");
      break;
    }
  }

  mProperty = mProxy->GetSceneObjectInputProperty( mPropertyIndex );
  int internalComponentIndex = mProxy->GetPropertyComponentIndex(mPropertyIndex);
  if( internalComponentIndex != INVALID_PROPERTY_COMPONENT_INDEX )
  {
    // override the one passed in
    mComponentIndex = internalComponentIndex;
  }
}

PropertyNotification::~PropertyNotification()
{
}

bool PropertyNotification::EvalFalse( const Dali::PropertyInput& value, const RawArgumentContainer& arg )
{
  return false;
}

void PropertyNotification::SetNotifyMode( NotifyMode notifyMode )
{
  switch(notifyMode)
  {
    case Dali::PropertyNotification::Disabled:
    {
      mNotifyValidity[0] = false;
      mNotifyValidity[1] = false;
      break;
    }
    case Dali::PropertyNotification::NotifyOnTrue:
    {
      mNotifyValidity[0] = false;
      mNotifyValidity[1] = true;
      break;
    }
    case Dali::PropertyNotification::NotifyOnFalse:
    {
      mNotifyValidity[0] = true;
      mNotifyValidity[1] = false;
      break;
    }
    case Dali::PropertyNotification::NotifyOnChanged:
    {
      mNotifyValidity[0] = true;
      mNotifyValidity[1] = true;
      break;
    }
  }
}

bool PropertyNotification::Check( BufferIndex bufferIndex )
{
  bool validityChanged = false;
  bool currentValid = false;

  if ( INVALID_PROPERTY_COMPONENT_INDEX != mComponentIndex )
  {
    // Evaluate Condition
    const PropertyInputComponentAccessor component( mProperty, mComponentIndex );
    const PropertyInputIndexer< PropertyInputComponentAccessor > input( bufferIndex, &component );
    currentValid = mConditionFunction(input, mArguments);
  }
  else
  {
    // Evaluate Condition
    const PropertyInputIndexer< PropertyInputImpl > input( bufferIndex, mProperty );
    currentValid = mConditionFunction(input, mArguments);
  }

  if( mValid != currentValid )
  {
    mValid = currentValid;
    validityChanged = mNotifyValidity[currentValid];
  }

  return validityChanged;
}

bool PropertyNotification::GetValidity() const
{
  return mValid;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
