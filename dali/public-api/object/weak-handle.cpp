/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/object/weak-handle.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/base-object-impl.h>

namespace Dali
{

struct WeakHandleBase::Impl : public BaseObject::Impl::Observer
{
  // Construction
  Impl()
  : mObject( nullptr )
  {
  }

  // Construction
  Impl( BaseHandle& handle )
  : mObject( nullptr )
  {
    if( handle )
    {
      mObject = static_cast<Dali::BaseObject*>( handle.GetObjectPtr() );
      if( mObject )
      {
        BaseObject::Impl::Get( *mObject ).AddObserver( *this );
      }
    }
  }

  // Destruction
  ~Impl() override
  {
    Reset();
  }

  void Reset()
  {
    if( mObject )
    {
      BaseObject::Impl::Get( *mObject ).RemoveObserver( *this );
      mObject = nullptr;
    }
  }

  /**
   * From BaseObject::Impl::Observer
   */
  void ObjectDestroyed( BaseObject& object ) override
  {
    mObject = nullptr;
  }

  // Data
  Dali::BaseObject* mObject;
};

WeakHandleBase::WeakHandleBase()
: mImpl( new Impl() )
{
}

WeakHandleBase::WeakHandleBase( BaseHandle& handle )
: mImpl( new Impl( handle ) )
{
}

WeakHandleBase::~WeakHandleBase()
{
  delete mImpl;
  mImpl = nullptr;
}

WeakHandleBase::WeakHandleBase(const WeakHandleBase& handle)
: mImpl( nullptr )
{
  BaseHandle object = handle.GetBaseHandle();
  mImpl = new Impl(object);
}

WeakHandleBase& WeakHandleBase::operator=( const WeakHandleBase& rhs )
{
  if( this != &rhs )
  {
    delete mImpl;

    BaseHandle handle = rhs.GetBaseHandle();
    mImpl = new Impl(handle);
  }

  return *this;
}

WeakHandleBase::WeakHandleBase( WeakHandleBase&& rhs )
: mImpl( rhs.mImpl )
{
  rhs.mImpl = nullptr;
}

WeakHandleBase& WeakHandleBase::operator=( WeakHandleBase&& rhs )
{
  if (this != &rhs)
  {
    delete mImpl;

    mImpl = rhs.mImpl;
    rhs.mImpl = nullptr;
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

BaseHandle WeakHandleBase::GetBaseHandle() const
{
  return mImpl ? BaseHandle( mImpl->mObject ) : BaseHandle();
}

void WeakHandleBase::Reset()
{
  mImpl->Reset();
}


} // Dali
