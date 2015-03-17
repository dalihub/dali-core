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
#include <dali/internal/event/object/custom-object-internal.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/animation/scene-graph-constraint-base.h>

using Dali::Internal::SceneGraph::PropertyOwner;
using Dali::Internal::SceneGraph::PropertyBase;
using Dali::Internal::SceneGraph::UpdateManager;
using Dali::Internal::SceneGraph::AnimatableProperty;

namespace Dali
{

namespace Internal
{

CustomObject* CustomObject::New()
{
  return new CustomObject();
}

const SceneGraph::PropertyOwner* CustomObject::GetSceneObject() const
{
  return mUpdateObject;
}

const PropertyBase* CustomObject::GetSceneObjectAnimatableProperty( Property::Index index ) const
{
  CustomProperty* custom = FindCustomProperty( index );
  DALI_ASSERT_ALWAYS( custom && "Property index is invalid" );
  return custom->GetSceneGraphProperty();
}

const PropertyInputImpl* CustomObject::GetSceneObjectInputProperty( Property::Index index ) const
{
  return GetSceneObjectAnimatableProperty( index );
}

unsigned int CustomObject::GetDefaultPropertyCount() const
{
  return 0u;
}

void CustomObject::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
}

const char* CustomObject::GetDefaultPropertyName( Property::Index index ) const
{
  return NULL;
}

Property::Index CustomObject::GetDefaultPropertyIndex(const std::string& name) const
{
  return Property::INVALID_INDEX;
}

bool CustomObject::IsDefaultPropertyWritable(Property::Index index) const
{
  return false;
}

bool CustomObject::IsDefaultPropertyAnimatable(Property::Index index) const
{
  return false;
}

bool CustomObject::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  return false;
}

Property::Type CustomObject::GetDefaultPropertyType(Property::Index index) const
{
  return Property::NONE;
}

void CustomObject::SetDefaultProperty( Property::Index index, const Property::Value& property )
{
  // do nothing
}

Property::Value CustomObject::GetDefaultProperty(Property::Index index) const
{
  return Property::Value();
}

CustomObject::~CustomObject()
{
  // Guard to allow handle destruction after Core has been destroyed
  if( Stage::IsInstalled() )
  {
    if( NULL != mUpdateObject )
    {
      RemoveObjectMessage( GetEventThreadServices().GetUpdateManager(), mUpdateObject );
      mUpdateObject = NULL; // object is about to be destroyed
    }
  }
}

CustomObject::CustomObject()
{
  PropertyOwner* updateObject = PropertyOwner::New();

  // Pass ownership to the update-thread
  AddObjectMessage( GetEventThreadServices().GetUpdateManager(), updateObject );

  // Keep as const since this should only be modified from update-thread
  mUpdateObject = updateObject;
}

} // namespace Internal

} // namespace Dali
