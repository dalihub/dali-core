//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/internal/update/gestures/scene-graph-pan-gesture.h>

// EXTERNAL INCLUDES
#include <cstring>

// INTERNAL INCLUDES

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

namespace
{
const unsigned int ARRAY_SIZE( 4u );
} // unnamed namespace

PanGesture* PanGesture::New()
{
  return new PanGesture();
}

PanGesture::~PanGesture()
{
}

void PanGesture::AddGesture( const Dali::PanGesture& gesture )
{
  mGestures[ mWritePosition ] = gesture;

  // Set the read flag to false for the gesture we have just written.
  // Ignore return value, we want to overwrite oldest gesture information even if it hasn't been read.
  (void) __sync_val_compare_and_swap( &mGestures[ mWritePosition ].read, true, false );

  // Update our write position.
  ++mWritePosition;
  mWritePosition %= ARRAY_SIZE;
}

void PanGesture::UpdateProperties( unsigned int nextRenderTime )
{
  unsigned int time( 0u );
  PanInfo latest;
  bool set( false );

  // Not going through array from the beginning, using it as a circular buffer and only using unread
  // values.
  for ( unsigned int i = 0; ( i < ARRAY_SIZE ); ++i )
  {
    // Copy the gesture first
    PanInfo currentGesture;
    currentGesture = mGestures[mReadPosition];

    // If we have already read this member, then exit out of loop
    if ( ! __sync_bool_compare_and_swap( &mGestures[mReadPosition].read, false, true ) )
    {
      break;
    }

    if ( currentGesture.time < time )
    {
      break;
    }

    latest = currentGesture;
    time = currentGesture.time;
    set = true;

    // Update our read position.
    ++mReadPosition;
    mReadPosition %= ARRAY_SIZE;
  }

  if ( set )
  {
    mScreenPosition.Set( latest.screen.position );
    mScreenDisplacement.Set( latest.screen.displacement );
    mLocalPosition.Set( latest.local.position );
    mLocalDisplacement.Set( latest.local.displacement );
  }
}

const GesturePropertyVector2& PanGesture::GetScreenPositionProperty() const
{
  return mScreenPosition;
}

const GesturePropertyVector2& PanGesture::GetScreenDisplacementProperty() const
{
  return mScreenDisplacement;
}

const GesturePropertyVector2& PanGesture::GetLocalPositionProperty() const
{
  return mLocalPosition;
}

const GesturePropertyVector2& PanGesture::GetLocalDisplacementProperty() const
{
  return mLocalDisplacement;
}

void PanGesture::ResetDefaultProperties( BufferIndex updateBufferIndex )
{
  // Nothing to do
}

PanGesture::PanGesture()
: mGestures(),
  mWritePosition( 0 ),
  mReadPosition( 0 )
{
  mGestures.resize( ARRAY_SIZE );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
