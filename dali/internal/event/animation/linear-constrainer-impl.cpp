/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/property-helper.h>


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
DALI_PROPERTY_TABLE_END( DEFAULT_OBJECT_PROPERTY_START_INDEX, LinearConstrainerDefaultProperties )

BaseHandle Create()
{
  return Dali::LinearConstrainer::New();
}

TypeRegistration mType( typeid( Dali::LinearConstrainer ), typeid( Dali::Handle ), Create, LinearConstrainerDefaultProperties );

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

Property::Value LinearConstrainer::GetDefaultProperty( Property::Index index ) const
{
  if( index == Dali::LinearConstrainer::Property::VALUE )
  {
    Property::Value value( Property::ARRAY );
    Property::Array* array = value.GetArray();
    uint32_t count = static_cast<uint32_t>( mValue.Size() );

    if( array )
    {
      array->Reserve( count );
      for( uint32_t i( 0 ); i != count; ++i )
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
    uint32_t count = static_cast<uint32_t>( mProgress.Size() );

    if( array )
    {
      array->Reserve( count );
      for( uint32_t i( 0 ); i != count; ++i )
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
    uint32_t propertyArrayCount = static_cast<uint32_t>( array->Count() );
    if( index == Dali::LinearConstrainer::Property::VALUE  )
    {
      mValue.Clear(); // remove old values
      mValue.Resize( propertyArrayCount );
      for( uint32_t i(0); i != propertyArrayCount; ++i )
      {
        array->GetElementAt( i ).Get( mValue[ i ] );
      }
    }
    else if( index == Dali::LinearConstrainer::Property::PROGRESS  )
    {
      mProgress.Clear(); // remove old values
      mProgress.Resize( propertyArrayCount );
      for( uint32_t i(0); i != propertyArrayCount; ++i )
      {
        array->GetElementAt( i ).Get( mProgress[ i ] );
      }
    }
  }
}

void LinearConstrainer::Apply( Property target, Property source, const Vector2& range, const Vector2& wrap)
{
  Dali::Constraint constraint = Dali::Constraint::New<float>( target.object, target.propertyIndex, LinearConstraintFunctor( mValue, mProgress, range, wrap ) );
  constraint.AddSource( Dali::Source(source.object, source.propertyIndex ) );

  constraint.SetTag( static_cast<uint32_t>( reinterpret_cast<uintptr_t>( this ) ) ); // taking 32bits of this as tag
  constraint.SetRemoveAction( Dali::Constraint::Discard );
  constraint.Apply();

  //Start observing the object
  Observe( target.object );
}

} // Internal

} // Dali
