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
#include <dali/internal/event/animation/path-constrainer-impl.h>

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
//              Name             Type   writable animatable constraint-input  enum for index-checking
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "forward",       VECTOR3,   true,    false,       false,        Dali::PathConstrainer::Property::FORWARD )
DALI_PROPERTY( "points",         ARRAY,    true,    false,       false,        Dali::PathConstrainer::Property::POINTS )
DALI_PROPERTY( "control-points", ARRAY,    true,    false,       false,        Dali::PathConstrainer::Property::CONTROL_POINTS )
DALI_PROPERTY_TABLE_END( DEFAULT_OBJECT_PROPERTY_START_INDEX )

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

unsigned int PathConstrainer::GetDefaultPropertyCount() const
{
  return DEFAULT_PROPERTY_COUNT;
}

void PathConstrainer::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.reserve( DEFAULT_PROPERTY_COUNT );

  for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    indices.push_back( i );
  }
}

const char* PathConstrainer::GetDefaultPropertyName(Property::Index index) const
{
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[index].name;
  }

  // index out of range
  return NULL;
}

Property::Index PathConstrainer::GetDefaultPropertyIndex(const std::string& name) const
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

Property::Type PathConstrainer::GetDefaultPropertyType(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].type;
  }

  // index out of range
  return Property::NONE;
}

Property::Value PathConstrainer::GetDefaultProperty( Property::Index index ) const
{
  Property::Value value;
  if( index == Dali::PathConstrainer::Property::FORWARD )
  {
    value = Property::Value( mForward );
  }
  else if( index == Dali::PathConstrainer::Property::POINTS )
  {
    Property::Array propertyArray;
    value = Property::Value(propertyArray);
    const Dali::Vector<Vector3>& point = mPath->GetPoints();
    size_t pointCount( point.Size() );
    for( size_t i( 0 ); i != pointCount; ++i )
    {
      value.AppendItem( point[i] );
    }
  }
  else if( index == Dali::PathConstrainer::Property::CONTROL_POINTS )
  {
    Property::Array propertyArray;
    value = Property::Value(propertyArray);
    const Dali::Vector<Vector3>& point = mPath->GetControlPoints();
    size_t pointCount( point.Size() );
    for( size_t i( 0 ); i != pointCount; ++i )
    {
      value.AppendItem( point[i] );
    }
  }

  return value;
}

void PathConstrainer::SetDefaultProperty(Property::Index index, const Property::Value& propertyValue)
{
  if( index == Dali::PathConstrainer::Property::FORWARD )
  {
    propertyValue.Get(mForward);
  }
  else if( index == Dali::PathConstrainer::Property::POINTS  )
  {
    Property::Array propertyArray;
    propertyValue.Get(propertyArray);

    size_t propertyArrayCount = propertyArray.size();
    Dali::Vector<Vector3> point;
    point.Resize( propertyArrayCount );
    for( size_t i(0); i!=propertyArrayCount; ++i )
    {
      propertyArray[i].Get( point[i]);
    }
    mPath->SetPoints( point );
  }
  else if( index == Dali::PathConstrainer::Property::CONTROL_POINTS )
  {
    Property::Array propertyArray;
    propertyValue.Get(propertyArray);

    size_t propertyArrayCount = propertyArray.size();
    Dali::Vector<Vector3> point;
    point.Resize( propertyArrayCount );
    for( size_t i(0); i!=propertyArrayCount; ++i )
    {
      propertyArray[i].Get( point[i]);
    }
    mPath->SetControlPoints( point );
  }
}

bool PathConstrainer::IsDefaultPropertyWritable(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].writable;
  }

  return false;
}

bool PathConstrainer::IsDefaultPropertyAnimatable(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].animatable;
  }

  return false;
}

bool PathConstrainer::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].constraintInput;
  }

  return false;
}

void PathConstrainer::Apply( Property target, Property source, const Vector2& range, const Vector2& wrap)
{
  Dali::Property::Type propertyType = target.object.GetPropertyType( target.propertyIndex);
  if( propertyType == Dali::Property::VECTOR3)
  {
    // If property type is Vector3, constrain its value to the position of the path
    Dali::Constraint constraint = Dali::Constraint::New<Vector3>( target.object, target.propertyIndex, PathConstraintFunctor( mPath, range, wrap ) );
    constraint.AddSource( Dali::Source(source.object, source.propertyIndex ) );

    constraint.SetTag( reinterpret_cast<size_t>( this ) );
    constraint.SetRemoveAction( Dali::Constraint::Discard );
    constraint.Apply();
  }
  else if( propertyType == Dali::Property::ROTATION )
  {
    // If property type is Rotation, constrain its value to align the forward vector to the tangent of the path
    Dali::Constraint constraint = Dali::Constraint::New<Quaternion>( target.object, target.propertyIndex, PathConstraintFunctor( mPath, range, mForward, wrap) );
    constraint.AddSource( Dali::Source(source.object, source.propertyIndex ) );

    constraint.SetTag( reinterpret_cast<size_t>( this ) );
    constraint.SetRemoveAction( Dali::Constraint::Discard );
    constraint.Apply();
  }

  //Start observing the object
  Observe( target.object );
}

} // Internal

} // Dali
