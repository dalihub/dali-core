/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/property-array.h>


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
  indices.Reserve( DEFAULT_PROPERTY_COUNT );

  for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    indices.PushBack( i );
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
  if( index == Dali::LinearConstrainer::Property::VALUE )
  {
    Property::Value value( Property::ARRAY );
    Property::Array* array = value.GetArray();
    size_t count( mValue.Size() );

    if( array )
    {
      array->Reserve( count );
      for( size_t i( 0 ); i != count; ++i )
      {
        array->PushBack( mValue[i] );
      }
    }
    return value;
  }
  else if( index == Dali::LinearConstrainer::Property::PROGRESS )
  {
    Property::Value value( Property::ARRAY );
    Property::Array* array = value.GetArray();
    size_t count( mProgress.Size() );

    if( array )
    {
      array->Reserve( count );
      for( size_t i( 0 ); i != count; ++i )
      {
        array->PushBack( mProgress[i] );
      }
    }
    return value;
  }

  return Property::Value();
}

Property::Value LinearConstrainer::GetDefaultPropertyCurrentValue( Property::Index index ) const
{
  return GetDefaultProperty( index ); // Event-side only properties
}

void LinearConstrainer::SetDefaultProperty( Property::Index index, const Property::Value& propertyValue )
{
  const Property::Array* array = propertyValue.GetArray();
  if( array )
  {
    size_t propertyArrayCount = array->Count();
    if( index == Dali::LinearConstrainer::Property::VALUE  )
    {
      mValue.Clear(); // remove old values
      mValue.Resize( propertyArrayCount );
      for( size_t i(0); i != propertyArrayCount; ++i )
      {
        array->GetElementAt( i ).Get( mValue[ i ] );
      }
    }
    else if( index == Dali::LinearConstrainer::Property::PROGRESS  )
    {
      mProgress.Clear(); // remove old values
      mProgress.Resize( propertyArrayCount );
      for( size_t i(0); i != propertyArrayCount; ++i )
      {
        array->GetElementAt( i ).Get( mProgress[ i ] );
      }
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
