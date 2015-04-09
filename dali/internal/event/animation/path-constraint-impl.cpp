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
#include <dali/internal/event/animation/path-constraint-impl.h>

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

//              Name         Type   writable animatable constraint-input  enum for index-checking
DALI_PROPERTY_TABLE_BEGIN
DALI_PROPERTY( "range",     VECTOR2,   true,    false,        false,        Dali::PathConstraint::Property::RANGE )
DALI_PROPERTY_TABLE_END( DEFAULT_OBJECT_PROPERTY_START_INDEX )


} //Unnamed namespace

PathConstraint* PathConstraint::New( Path& path, const Vector2& range)
{
  return new PathConstraint( path, range);
}

PathConstraint::PathConstraint( Path& path, const Vector2& range)
: Object(),
  mPath( Path::Clone(path)),
  mRange(range)
{
}

PathConstraint::~PathConstraint()
{
  //Remove constraints created by this PathConstraint
  size_t tag = reinterpret_cast<size_t>( this );
  const ObjectIter end = mObservedObjects.End();
  for( ObjectIter iter = mObservedObjects.Begin(); iter != end; ++iter )
  {
    //Remove PathConstraint from the observers list of the object
    (*iter)->RemoveObserver( *this );

    //Remove constraints created by this PathConstraint in the object
    (*iter)->RemoveConstraints( tag );
  }
}

void PathConstraint::ObjectDestroyed(Object& object)
{
  //Remove object from the list of observed
  const ObjectIter end = mObservedObjects.End();
  for( ObjectIter iter = mObservedObjects.Begin(); iter != end; ++iter )
  {
    if( *iter == &object )
    {
      mObservedObjects.Erase(iter);
      return;
    }
  }
}

unsigned int PathConstraint::GetDefaultPropertyCount() const
{
  return DEFAULT_PROPERTY_COUNT;
}

void PathConstraint::GetDefaultPropertyIndices( Property::IndexContainer& indices ) const
{
  indices.reserve( DEFAULT_PROPERTY_COUNT );

  for ( int i = 0; i < DEFAULT_PROPERTY_COUNT; ++i )
  {
    indices.push_back( i );
  }
}

const char* PathConstraint::GetDefaultPropertyName(Property::Index index) const
{
  if ( ( index >= 0 ) && ( index < DEFAULT_PROPERTY_COUNT ) )
  {
    return DEFAULT_PROPERTY_DETAILS[index].name;
  }

  // index out of range
  return NULL;
}

Property::Index PathConstraint::GetDefaultPropertyIndex(const std::string& name) const
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

Property::Type PathConstraint::GetDefaultPropertyType(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].type;
  }

  // index out of range
  return Property::NONE;
}

Property::Value PathConstraint::GetDefaultProperty( Property::Index index ) const
{
  if( index == Dali::PathConstraint::Property::RANGE )
  {
    return Property::Value( mRange );
  }

  return Property::Value();
}

void PathConstraint::SetDefaultProperty(Property::Index index, const Property::Value& propertyValue)
{
  if( index == Dali::PathConstraint::Property::RANGE )
  {
    propertyValue.Get(mRange);
  }
}

bool PathConstraint::IsDefaultPropertyWritable(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].writable;
  }

  return false;
}

bool PathConstraint::IsDefaultPropertyAnimatable(Property::Index index) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].animatable;
  }

  return false;
}

bool PathConstraint::IsDefaultPropertyAConstraintInput( Property::Index index ) const
{
  if( index < DEFAULT_PROPERTY_COUNT )
  {
    return DEFAULT_PROPERTY_DETAILS[index].constraintInput;
  }

  return false;
}

void PathConstraint::Apply( Property source, Property target, const Vector3& forward)
{

  Dali::Property::Type propertyType = target.object.GetPropertyType( target.propertyIndex);


  if( propertyType == Dali::Property::VECTOR3)
  {
    // If property is Vector3, constrain its value to the position of the path
    Dali::Constraint constraint = Dali::Constraint::New<Vector3>( target.object, target.propertyIndex, PathConstraintFunctor( mPath, mRange ) );
    constraint.AddSource( Source(source.object, source.propertyIndex ) );

    constraint.SetTag( reinterpret_cast<size_t>( this ) );
    constraint.SetRemoveAction( Dali::Constraint::Discard );
    constraint.Apply();
  }
  else if( propertyType == Dali::Property::ROTATION )
  {
    // If property is Rotation, constrain its value to align the forward vector to the tangent of the path
    Dali::Constraint constraint = Dali::Constraint::New<Quaternion>( target.object, target.propertyIndex, PathConstraintFunctor( mPath, mRange,forward) );
    constraint.AddSource( Source(source.object, source.propertyIndex ) );

    constraint.SetTag( reinterpret_cast<size_t>( this ) );
    constraint.SetRemoveAction( Dali::Constraint::Discard );
    constraint.Apply();
  }

  //Add the object to the list of observed objects if it is not there already
  Object& object = dynamic_cast<Object&>( GetImplementation(target.object) );
  const ObjectIter end = mObservedObjects.End();
  ObjectIter iter = mObservedObjects.Begin();
  for(; iter != end; ++iter )
  {
    if( *iter == &object )
    {
      break;
    }
  }

  if( iter == end )
  {
    //Start observing the object
    object.AddObserver( *this );

    //Add object in the observed objects vector
    mObservedObjects.PushBack( &object );
  }
}

void PathConstraint::Remove( Dali::Handle& target )
{
  Object& object = dynamic_cast<Object&>( GetImplementation(target) );
  const ObjectIter end = mObservedObjects.End();
  for( ObjectIter iter = mObservedObjects.Begin(); iter != end; ++iter )
  {
    if( *iter == &object )
    {
      //Stop observing the object
      (*iter)->RemoveObserver( *this );

      //Remove constraints created by PathConstraint in the object
      size_t tag = reinterpret_cast<size_t>( this );
      target.RemoveConstraints( tag );

      //Remove object from the vector of observed objects
      mObservedObjects.Erase(iter);
    }
  }
}

} // Internal
} // Dali
