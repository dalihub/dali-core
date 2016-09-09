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
#include <dali/devel-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/object-impl.h>

namespace Dali
{

struct WeakHandleBase::Impl : public Internal::Object::Observer
{
  // Construction
  Impl()
  : mObject( NULL )
  {
  }

  // Construction
  Impl( Handle& handle )
  : mObject( NULL )
  {
    if(handle)
    {
      mObject = static_cast<Internal::Object*>( handle.GetObjectPtr() );
      if(mObject)
      {
        mObject->AddObserver( *this );
      }
    }
  }

  // Destruction
  ~Impl()
  {
    Reset();
  }

  void Reset()
  {
    if( mObject )
    {
      mObject->RemoveObserver( *this );
      mObject = NULL;
    }
  }

  /**
   * From Object::Observer
   */
  virtual void SceneObjectAdded( Internal::Object& object )
  {
  }

  /**
   * From Object::Observer
   */
  virtual void SceneObjectRemoved( Internal::Object& object )
  {
  }

  /**
   * From Object::Observer
   */
  virtual void ObjectDestroyed( Internal::Object& object )
  {
    mObject = NULL;
  }

  // Data
  Internal::Object* mObject;
};

WeakHandleBase::WeakHandleBase()
: mImpl( new Impl() )
{
}

WeakHandleBase::WeakHandleBase( Handle& handle )
: mImpl( new Impl( handle ) )
{
}

WeakHandleBase::~WeakHandleBase()
{
  delete mImpl;
  mImpl = NULL;
}

WeakHandleBase::WeakHandleBase(const WeakHandleBase& handle)
: mImpl( NULL )
{
  Handle object = handle.GetBaseHandle();
  mImpl = new Impl(object);
}

WeakHandleBase& WeakHandleBase::operator=( const WeakHandleBase& rhs )
{
  if( this != &rhs )
  {
    delete mImpl;

    Handle handle = rhs.GetBaseHandle();
    mImpl = new Impl(handle);
  }

  return *this;
}

bool WeakHandleBase::operator==( const WeakHandleBase& rhs ) const
{
  return this->mImpl->mObject == rhs.mImpl->mObject;
}

bool WeakHandleBase::operator!=( const WeakHandleBase& rhs ) const
{
  return !( *this == rhs );
}

Handle WeakHandleBase::GetBaseHandle() const
{
  return Handle( mImpl->mObject );
}

void WeakHandleBase::Reset()
{
  mImpl->Reset();
}


} // Dali
