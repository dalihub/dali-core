/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/animation/linear-constrainer-impl.h>

//EXTRENAL INCLUDES
#include <cstring> // for strcmp

// INTERNAL INCLUDES
#include <dali/internal/event/common/property-helper.h>
#include <dali/public-api/animation/constraint.h>

namespace Dali
{

namespace Internal
{

namespace
{

// Properties
//              Name            Type   writable animatable constraint-input  enum for index-checking
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "value",        ARRAY,     true,    false,       false,        Dali::LinearConstrainer::Property::VALUE )
DALI_PROPERTY( "progress",     ARRAY,     true,    false,       false,        Dali::LinearConstrainer::Property::PROGRESS )
DALI_PROPERTY_TABLE_END( DEFAULT_OBJECT_PROPERTY_START_INDEX )

} //Unnamed namespace

LinearConstrainer* LinearConstrainer::New()
{
  return new LinearConstrainer();
}

LinearConstrainer::LinearConstrainer()
: Constrainer()
{
}

LinearConstrainer::~LinearConstrainer()
{
}

unsigned int LinearConstrainer::GetDefaultPropertyCount() const
{
  return DEFAULT_PROPERTY_COUNT;
}

void LinearConstrainer::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.reserve( DEFAULT_PROPERTY_COUNT );

  for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    indices.push_back( i );
  }
}

const char* LinearConstrainer::GetDefaultPropertyName(Property::Index index) const
{
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[index].name;
  }

  // index out of range
  return NULL;
}

Property::Index LinearConstrainer::GetDefaultPropertyIndex(const std::string& name) const
{
  Property::Index index = Property::INVALID_INDEX;

  // Look for name in default properties
  for( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    const Internal::PropertyDetails* property = &DEFAULT_PROPERTY_DETAILS[ i ];
    if( 0 == strcmp( name.c_str(), property->name ) )
    {
      index = i;
      break;
    }
  }
  return index;
}

Property::Type LinearConstrainer::GetDefaultPropertyType(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].type;
  }

  // index out of range
  return Property::NONE;
}

Property::Value LinearConstrainer::GetDefaultProperty( Property::Index index ) const
{
  Property::Value value;
  if( index == Dali::LinearConstrainer::Property::VALUE )
  {
    value = Property::Value(Property::ARRAY);
    size_t count( mValue.Size() );
    for( size_t i( 0 ); i != count; ++i )
    {
      value.AppendItem( mValue[i] );
    }
  }
  else if( index == Dali::LinearConstrainer::Property::PROGRESS )
  {
    value = Property::Value(Property::ARRAY);
    size_t count( mValue.Size() );
    for( size_t i( 0 ); i != count; ++i )
    {
      value.AppendItem( mProgress[i] );
    }
  }

  return value;
}

void LinearConstrainer::SetDefaultProperty(Property::Index index, const Property::Value& propertyValue)
{
  if( index == Dali::LinearConstrainer::Property::VALUE  )
  {
    size_t propertyArrayCount = propertyValue.GetSize();
    mValue.Resize( propertyArrayCount );
    for( size_t i(0); i != propertyArrayCount; ++i )
    {
      propertyValue.GetItem(i).Get( mValue[i] );
    }
  }
  else if( index == Dali::LinearConstrainer::Property::PROGRESS  )
  {
    size_t propertyArrayCount = propertyValue.GetSize();
    mProgress.Resize( propertyArrayCount );
    for( size_t i(0); i != propertyArrayCount; ++i )
    {
      propertyValue.GetItem(i).Get( mProgress[i] );
    }
  }
}

bool LinearConstrainer::IsDefaultPropertyWritable(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].writable;
  }

  return false;
}

bool LinearConstrainer::IsDefaultPropertyAnimatable(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].animatable;
  }

  return false;
}

bool LinearConstrainer::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].constraintInput;
  }

  return false;
}

void LinearConstrainer::Apply( Property target, Property source, const Vector2& range, const Vector2& wrap)
{
  Dali::Constraint constraint = Dali::Constraint::New<float>( target.object, target.propertyIndex, LinearConstraintFunctor( mValue, mProgress, range, wrap ) );
  constraint.AddSource( Dali::Source(source.object, source.propertyIndex ) );

  constraint.SetTag( reinterpret_cast<size_t>( this ) );
  constraint.SetRemoveAction( Dali::Constraint::Discard );
  constraint.Apply();


  //Start observing the object
  Observe( target.object );
}

} // Internal

} // Dali
