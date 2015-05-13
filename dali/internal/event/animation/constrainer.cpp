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
#include <dali/internal/event/animation/constrainer.h>

// INTERNAL INCLUDES
#include <dali/internal/event/animation/constraint-source-impl.h>
#include <dali/public-api/animation/constraint.h>

namespace Dali
{

namespace Internal
{

Constrainer::Constrainer()
:Object()
{
}

Constrainer::~Constrainer()
{
  //Remove all the constraints created by the object
  size_t tag = reinterpret_cast<size_t>( this );
  const ObjectIter end = mObservedObjects.End();
  for( ObjectIter iter = mObservedObjects.Begin(); iter != end; ++iter )
  {
    //Remove Constrainer from the observers list of the object
    (*iter)->RemoveObserver( *this );

    //Remove constraints
    (*iter)->RemoveConstraints( tag );
  }
}

void Constrainer::ObjectDestroyed( Object& object )
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

void Constrainer::Remove( Dali::Handle& target )
{
  size_t tag = reinterpret_cast<size_t>( this );
  Object& object = GetImplementation(target);
  const ObjectIter end = mObservedObjects.End();
  for( ObjectIter iter = mObservedObjects.Begin(); iter != end; ++iter )
  {
    if( *iter == &object )
    {
      //Stop observing the object
      (*iter)->RemoveObserver( *this );

      //Remove constraints created in the object
      target.RemoveConstraints( tag );

      //Remove object from the vector of observed objects
      mObservedObjects.Erase(iter);
    }
  }
}

void Constrainer::Observe( Dali::Handle& handle )
{
  Object& object = GetImplementation(handle);

  //Add the object to the list of observed objects if it is not in it already
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

} // namespace Internal

} // namespace Dali

