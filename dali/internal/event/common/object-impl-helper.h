#ifndef DALI_INTERNAL_OBJECT_IMPL_HELPER_H
#define DALI_INTERNAL_OBJECT_IMPL_HELPER_H

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
#include <dali/public-api/object/property.h> // Dali::Property
#include <dali/public-api/object/property-index-ranges.h> // DEFAULT_DERIVED_HANDLE_PROPERTY_START_INDEX
#include <dali/internal/event/common/property-helper.h> // Dali::Internal::PropertyDetails

namespace Dali
{
namespace Internal
{

class CustomProperty;
class PropertyInputImpl;

namespace SceneGraph
{

class PropertyBase;
class PropertyOwner;


} // namespace SceneGraph

/**
 * Helper template class to be used by class that implement Object
 *
 * Example:
 *<pre>
 * typename ObjectImplHelper<DEFAULT_PROPERTY_COUNT, DEFAULT_PROPERTY_DETAILS> MyObjectImpl;
 *
 * MyObjectImpl::GetDefaultPropertyCount();
 * </pre>
 */

template<size_t DEFAULT_PROPERTY_COUNT>
struct ObjectImplHelper
{
  const PropertyDetails* DEFAULT_PROPERTY_DETAILS;

  unsigned int GetDefaultPropertyCount() const
  {
    return DEFAULT_PROPERTY_COUNT;
  }

  void GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
  {
    indices.reserve( DEFAULT_PROPERTY_COUNT );

    for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
    {
      indices.push_back( DEFAULT_DERIVED_HANDLE_PROPERTY_START_INDEX + i );
    }
  }

  const char* GetDefaultPropertyName( Property::Index index ) const
  {
    const char* name = NULL;

    if( index >= DEFAULT_DERIVED_HANDLE_PROPERTY_START_INDEX && index < DEFAULT_PROPERTY_COUNT )
    {
      name = DEFAULT_PROPERTY_DETAILS[index].name;
    }

    return name;
  }

  Property::Index GetDefaultPropertyIndex( const std::string& name ) const
  {
    Property::Index index = Property::INVALID_INDEX;

    // Look for name in default properties
    for( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
    {
      const Internal::PropertyDetails* property = &DEFAULT_PROPERTY_DETAILS[ i ];
      if( 0 == strcmp( name.c_str(), property->name ) ) // dont want to convert rhs to string
      {
        index = i;
        break;
      }
    }

    return index;
  }

  bool IsDefaultPropertyWritable( Property::Index index ) const
  {
    bool isWritable = false;

    if( index >= DEFAULT_DERIVED_HANDLE_PROPERTY_START_INDEX && index < DEFAULT_PROPERTY_COUNT )
    {
      isWritable = DEFAULT_PROPERTY_DETAILS[index].writable;
    }

    return isWritable;
  }

  bool IsDefaultPropertyAnimatable( Property::Index index ) const
  {
    bool isAnimatable = false;

    if( index >= DEFAULT_DERIVED_HANDLE_PROPERTY_START_INDEX && index < DEFAULT_PROPERTY_COUNT )
    {
      isAnimatable =  DEFAULT_PROPERTY_DETAILS[index].animatable;
    }

    return isAnimatable;
  }

  bool IsDefaultPropertyAConstraintInput( Property::Index index ) const
  {
    bool isConstraintInput = false;

    if( index >= DEFAULT_DERIVED_HANDLE_PROPERTY_START_INDEX && index < DEFAULT_PROPERTY_COUNT )
    {
      isConstraintInput = DEFAULT_PROPERTY_DETAILS[index].constraintInput;
    }

    return isConstraintInput;
  }

  Property::Type GetDefaultPropertyType( Property::Index index ) const
  {
    Property::Type type = Property::NONE;

    if( index >= DEFAULT_DERIVED_HANDLE_PROPERTY_START_INDEX && index < DEFAULT_PROPERTY_COUNT )
    {
      type =  DEFAULT_PROPERTY_DETAILS[index].type;
    }

    return type;
  }

  void SetDefaultProperty( Property::Index index,
                           const Property::Value& property ) const
  {
    // TODO: MESH_REWORK
    DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
  }

  void SetSceneGraphProperty( Property::Index index,
                              const CustomProperty& entry,
                              const Property::Value& value ) const
  {
    // TODO: MESH_REWORK
    DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
  }

  Property::Value GetDefaultProperty( Property::Index index ) const
  {
    Property::Value value;

    // TODO: MESH_REWORK
    DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );

    return value;
  }

  const SceneGraph::PropertyOwner* GetPropertyOwner() const
  {
    // TODO: MESH_REWORK
    DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
    return 0;
  }

  const SceneGraph::PropertyOwner* GetSceneObject() const
  {
    // TODO: MESH_REWORK
    DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
    return 0;
  }

  const SceneGraph::PropertyBase* GetSceneObjectAnimatableProperty( Property::Index index ) const
  {
    // TODO: MESH_REWORK
    DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
    return 0;
  }

  const PropertyInputImpl* GetSceneObjectInputProperty( Property::Index index ) const
  {
    // TODO: MESH_REWORK
    DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
    return 0;
  }

  int GetPropertyComponentIndex( Property::Index index ) const
  {
    // TODO: MESH_REWORK
    DALI_ASSERT_ALWAYS( false && "TODO: MESH_REWORK" );
    return 0;
  }

};



} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_OBJECT_IMPL_HELPER_H

