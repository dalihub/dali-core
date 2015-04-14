#ifndef __DALI_INTERNAL_SCENE_GRAPH_DOUBLE_BUFFERED_H__
#define __DALI_INTERNAL_SCENE_GRAPH_DOUBLE_BUFFERED_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/internal/common/owner-pointer.h>

namespace Dali
{

namespace Internal
{

// The number of buffers per scene-graph property
static const unsigned int NUM_SCENE_GRAPH_BUFFERS = 2;

// Buffer index used when reading off-stage values
static const unsigned int ARBITRARY_OFF_STAGE_BUFFER = 0;

namespace SceneGraph
{

/**
 * Templated class for a double-buffered value.
 */
template <typename T>
class DoubleBuffered
{
public:

  DoubleBuffered()
  : mValue1(),
    mValue2()
  {
  }

  DoubleBuffered(const T& val)
  : mValue1(val),
    mValue2(val)
  {
  }

  inline T& operator[](const size_t i)
  {
    DALI_ASSERT_DEBUG(i < NUM_SCENE_GRAPH_BUFFERS);

    return *(&mValue1+i);
  }

  inline const T& operator[](const size_t i) const
  {
    DALI_ASSERT_DEBUG(i < NUM_SCENE_GRAPH_BUFFERS);

    return *(&mValue1+i);
  }

private:

  // Undefined
  DoubleBuffered<T>(const DoubleBuffered<T>&);

  // Undefined
  DoubleBuffered<T>& operator=(const DoubleBuffered<T>& rhs);

private:

  T mValue1;
  T mValue2;
};

/**
 * @brief Specialization for owner-pointer
 *
 * This class takes ownership of the pointers and releases the memory when the pointer
 * is no longer used by either buffer
 */
template <typename T>
class DoubleBuffered< OwnerPointer< T > >
{
public:

  /**
   * Class that deals with setting a value
   */
  class Setter
  {
  public:
    /**
     * @brief Assignment operator to that a value that will later
     * be set in the correct buffer index of the object referenced by the setter.
     */
    Setter& operator=( T* value )
    {
      mValue = value;
      return *this;
    }

    ~Setter()
    {
      mObject.Set( mIndex, mValue );
    }

  private:
    Setter( DoubleBuffered& object,
            size_t i,
            T* value )
    : mObject( object ),
      mIndex( i ),
      mValue( value )
    {
    }

    Setter( const Setter& rhs )
    : mObject( rhs.mObject ),
      mIndex( rhs.mIndex ),
      mValue( rhs.mValue )
    {
    }

    DoubleBuffered& mObject; ///< Double-buffered object that will be changed
    const size_t mIndex;                          ///< Buffer index that will be changed
    T* mValue;                                    ///< Value of the pointer

    friend class DoubleBuffered;
  };

  DoubleBuffered()
  : mValue1( NULL ),
    mValue2( NULL )
  {
  }

  DoubleBuffered(T* val)
  : mValue1( val ),
    mValue2( val )
  {
  }

  ~DoubleBuffered()
  {
    if( mValue2 != mValue1 )
    {
      delete mValue2;
    }
    delete mValue1;
  }

  void Set( size_t i, T* value )
  {
    T*& current = *(&mValue1 + i);
    T*& previous = *(&mValue1 + 1u-i);

    if( current != value && current != previous )
    {
      delete current;
    }
    current = value;
  }

  Setter operator[](size_t i)
  {
    return Setter( *this, i, *(&mValue1+i) );
  }

  const T* operator[](size_t i) const
  {
    DALI_ASSERT_DEBUG(i < NUM_SCENE_GRAPH_BUFFERS);

    return *(&mValue1+i);
  }

  /**
   * Auto-age the property: if it was set the previous frame,
   * then copy the value into the current frame's buffer.
   */
  void CopyPrevious( size_t i )
  {
    DALI_ASSERT_DEBUG(i < NUM_SCENE_GRAPH_BUFFERS);

    T*& current = *(&mValue1 + i);
    T*& previous = *(&mValue1 + 1u-i);

    if( current != previous )
    {
      delete previous;
    }

    current = previous;
  }

private:

  // Undefined
  DoubleBuffered(const DoubleBuffered&);

  // Undefined
  DoubleBuffered& operator=(const DoubleBuffered& rhs);

private:

  T* mValue1;
  T* mValue2;

};


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_DOUBLE_BUFFERED_H__
