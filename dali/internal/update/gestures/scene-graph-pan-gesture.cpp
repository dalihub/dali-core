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

  // Update our write position (so now this new value can be read)
  // Note: we want to overwrite oldest gesture information even if it hasn't been read.
  ++mWritePosition;
  mWritePosition %= ARRAY_SIZE;
}

void PanGesture::UpdateProperties( unsigned int nextRenderTime )
{
  unsigned int time( 0u );
  bool justStarted ( false );
  bool justFinished ( false );

  // If our read position is not same as write position, then there
  // must be new values on circular buffer to read.
  while(mReadPosition != mWritePosition)
  {
    const PanInfo& currentGesture = mGestures[mReadPosition];

    if ( currentGesture.time > time )
    {
      justStarted |= (currentGesture.state == Gesture::Started);
      justFinished |= (currentGesture.state == Gesture::Finished || currentGesture.state == Gesture::Cancelled);

      // use position values direct from gesture.
      mLatestGesture.screen.position = currentGesture.screen.position;
      mLatestGesture.local.position = currentGesture.local.position;

      // change displacement to be relative to initial touch, instead of relative to last touch-frame.
      if(currentGesture.state == Gesture::Started)
      {
        mLatestGesture.screen.displacement = currentGesture.screen.displacement;
        mLatestGesture.local.displacement = currentGesture.local.displacement;
      }
      else
      {
        mLatestGesture.screen.displacement += currentGesture.screen.displacement;
        mLatestGesture.local.displacement += currentGesture.local.displacement;
      }

      time = currentGesture.time;
    }

    ++mReadPosition;
    mReadPosition %= ARRAY_SIZE;
  }

  mInGesture |= justStarted;

  if ( mInGesture )
  {
    PanInfo gesture(mLatestGesture);

    if( !justStarted ) // only use previous frame if this is the continuing.
    {
      // If previous gesture exists, then produce position as 50/50 interpolated blend of these two points.
      gesture.screen.position += mPreviousGesture.screen.position;
      gesture.local.position += mPreviousGesture.local.position;
      gesture.screen.position *= 0.5f;
      gesture.local.position *= 0.5f;
      // make current displacement relative to previous update-frame now.
      gesture.screen.displacement -= mPreviousGesture.screen.displacement;
      gesture.local.displacement -= mPreviousGesture.local.displacement;
    }

    mPreviousGesture = mLatestGesture;

    mScreenPosition.Set( gesture.screen.position );
    mScreenDisplacement.Set( gesture.screen.displacement );
    mLocalPosition.Set( gesture.local.position );
    mLocalDisplacement.Set( gesture.local.displacement );
  }

  mInGesture &= ~justFinished;
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
  mScreenPosition.Reset();
  mScreenDisplacement.Reset();
  mLocalPosition.Reset();
  mLocalDisplacement.Reset();
}

PanGesture::PanGesture()
: mGestures(),
  mWritePosition( 0 ),
  mReadPosition( 0 ),
  mInGesture( false )
{
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
