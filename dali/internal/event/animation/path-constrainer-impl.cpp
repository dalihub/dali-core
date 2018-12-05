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
#include <dali/internal/event/animation/path-constrainer-impl.h>

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
//              Name             Type   writable animatable constraint-input  enum for index-checking
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "forward",       VECTOR3,   true,    false,       false,        Dali::PathConstrainer::Property::FORWARD )
DALI_PROPERTY( "points",         ARRAY,    true,    false,       false,        Dali::PathConstrainer::Property::POINTS )
DALI_PROPERTY( "controlPoints",  ARRAY,    true,    false,       false,        Dali::PathConstrainer::Property::CONTROL_POINTS )
DALI_PROPERTY_TABLE_END( DEFAULT_OBJECT_PROPERTY_START_INDEX, PathConstrainerDefaultProperties )

BaseHandle Create()
{
  return Dali::PathConstrainer::New();
}

TypeRegistration mType( typeid( Dali::PathConstrainer ), typeid( Dali::Handle ), Create, PathConstrainerDefaultProperties );

} //Unnamed namespace

PathConstrainer* PathConstrainer::New()
{
  return new PathConstrainer();
}

PathConstrainer::PathConstrainer()
: Constrainer(),
  mPath( Path::New() )
{
}

PathConstrainer::~PathConstrainer()
{
}

Property::Value PathConstrainer::GetDefaultProperty( Property::Index index ) const
{
  if( index == Dali::PathConstrainer::Property::FORWARD )
  {
    return Property::Value( mForward );
  }
  else
  {
    if( index == Dali::PathConstrainer::Property::POINTS )
    {
      Property::Value value( Property::ARRAY );
      Property::Array* array = value.GetArray();
      const Dali::Vector<Vector3>& point = mPath->GetPoints();
      Property::Array::SizeType pointCount = static_cast<Property::Array::SizeType>( point.Size() );

      if( array )
      {
        array->Reserve( pointCount );
        for( Property::Array::SizeType i = 0; i < pointCount; ++i )
        {
          array->PushBack( point[i] );
        }
      }
      return value;
    }
    else if( index == Dali::PathConstrainer::Property::CONTROL_POINTS )
    {
      Property::Value value( Property::ARRAY );
      Property::Array* array = value.GetArray();
      const Dali::Vector<Vector3>& point = mPath->GetControlPoints();
      Property::Array::SizeType pointCount = static_cast<Property::Array::SizeType>( point.Size() );

      if( array )
      {
        array->Reserve( pointCount );
        for( Property::Array::SizeType i = 0; i < pointCount; ++i )
        {
          array->PushBack( point[i] );
        }
      }
      return value;
    }
  }

  return Property::Value();
}

Property::Value PathConstrainer::GetDefaultPropertyCurrentValue( Property::Index index ) const
{
  return GetDefaultProperty( index ); // Event-side only properties
}

void PathConstrainer::SetDefaultProperty( Property::Index index, const Property::Value& propertyValue )
{
  if( index == Dali::PathConstrainer::Property::FORWARD )
  {
    propertyValue.Get(mForward);
  }
  else if( index == Dali::PathConstrainer::Property::POINTS  )
  {
    const Property::Array* array = propertyValue.GetArray();
    mPath->ClearPoints();
    if( array )
    {
      for( Property::Array::SizeType i = 0, count = array->Count(); i < count; ++i )
      {
        Vector3 point;
        array->GetElementAt( i ).Get( point );
        mPath->AddPoint( point );
      }
    }
  }
  else if( index == Dali::PathConstrainer::Property::CONTROL_POINTS )
  {
    const Property::Array* array = propertyValue.GetArray();
    mPath->ClearControlPoints();
    if( array )
    {
      for( Property::Array::SizeType i = 0, count = array->Count(); i < count; ++i )
      {
        Vector3 point;
        array->GetElementAt( i ).Get( point );
        mPath->AddControlPoint( point );
      }
    }
  }
}

void PathConstrainer::Apply( Property target, Property source, const Vector2& range, const Vector2& wrap)
{
  Dali::Property::Type propertyType = target.object.GetPropertyType( target.propertyIndex);
  if( propertyType == Dali::Property::VECTOR3)
  {
    // If property type is Vector3, constrain its value to the position of the path
    Dali::Constraint constraint = Dali::Constraint::New<Vector3>( target.object, target.propertyIndex, PathConstraintFunctor( mPath, range, wrap ) );
    constraint.AddSource( Dali::Source(source.object, source.propertyIndex ) );

    constraint.SetTag( static_cast<uint32_t>( reinterpret_cast<uintptr_t>( this ) ) ); // taking 32bits of this as tag
    constraint.SetRemoveAction( Dali::Constraint::Discard );
    constraint.Apply();
  }
  else if( propertyType == Dali::Property::ROTATION )
  {
    // If property type is Rotation, constrain its value to align the forward vector to the tangent of the path
    Dali::Constraint constraint = Dali::Constraint::New<Quaternion>( target.object, target.propertyIndex, PathConstraintFunctor( mPath, range, mForward, wrap) );
    constraint.AddSource( Dali::Source(source.object, source.propertyIndex ) );

    constraint.SetTag( static_cast<uint32_t>( reinterpret_cast<uintptr_t>( this ) ) ); // taking 32bits of this as tag
    constraint.SetRemoveAction( Dali::Constraint::Discard );
    constraint.Apply();
  }

  //Start observing the object
  Observe( target.object );
}

} // Internal

} // Dali
