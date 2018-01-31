#ifndef DALI_GRAPHICS_SYNCHRONIZED_H
#define DALI_GRAPHICS_SYNCHRONIZED_H

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

// EXTERNAL INCLUDES
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

namespace Dali
{
namespace Graphics
{
namespace Utility
{
/**
 * @brief Class that encapsulates object for thread-safe access
 */
template < typename T >
class Synchronized final
{
public:
  template < typename... Args >
  Synchronized( Args... args )
  : mMutex(std::make_unique<std::mutex>()),
    mObject( std::forward< Args >( args )... )
  {
  }

  Synchronized( const Synchronized& ) = delete;
  Synchronized& operator=( const Synchronized& ) = delete;
  Synchronized( Synchronized&& ) = default;
  Synchronized& operator=( Synchronized&& ) = default;
  virtual ~Synchronized() = default;

  /**
   * @brief Handle to access the data in the Synchronized object
   */
  class Handle final
  {
  public:
    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;
    Handle(Handle&&) = default;
    Handle& operator=(Handle&&) = default;
    ~Handle()
    {
      mMutex.unlock();
    }

    T& operator*()
    {
      return mObjectRef;
    }

    const T& operator*() const
    {
      return mObjectRef;
    }

    T& operator->()
    {
      return mObjectRef;
    }

    const T& operator->() const
    {
      return mObjectRef;
    }

    T* operator&()
    {
      return &mObjectRef;
    }

    const T* operator&() const
    {
      return &mObjectRef;
    }

  private:
    friend Synchronized; //< Allow Synchronized to create a handle
    Handle( std::mutex& mutex, T& object )
    : mMutex( mutex ),
      mObjectRef( object )
    {
      mMutex.lock();
    }

    std::mutex& mMutex;
    T& mObjectRef;
  };

  Handle Lock() {
    return Handle(*mMutex, mObject);
  }

private:
  std::unique_ptr< std::mutex > mMutex; //< Mutex to guard access to the object
  T mObject;                            //< Memeber object that stores the data
};

template < typename T, typename... Args >
Synchronized< T > MakeSynchronized( Args... args )
{
  return Synchronized< T >( std::forward< Args >( args )... );
}

} // namespace Utility
} // namespace Graphics
} // namespace Dali

#endif //DALI_GRAPHICS_SYNCHRONIZED_H