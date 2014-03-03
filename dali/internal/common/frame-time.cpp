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
#include <dali/internal/common/frame-time.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/platform-abstraction.h>

namespace Dali
{

using Integration::PlatformAbstraction;

namespace Internal
{

namespace
{
#if defined(DEBUG_ENABLED)
Integration::Log::Filter* gLogFilter = Integration::Log::Filter::New(Debug::NoLogging, false, "LOG_FRAME_TIME");
#endif

const unsigned int DEFAULT_MINIMUM_FRAME_TIME_INTERVAL( 16667u );

const unsigned int MICROSECONDS_PER_SECOND( 1000000u );
const unsigned int MICROSECONDS_PER_MILLISECOND( 1000u );

const float        MICROSECONDS_TO_SECONDS( 0.000001f );

const unsigned int HISTORY_SIZE(3);
} // unnamed namespace

FrameTime::FrameTime( PlatformAbstraction& platform )
: mPlatform( platform ),
  mMinimumFrameTimeInterval( DEFAULT_MINIMUM_FRAME_TIME_INTERVAL ),
  mLastVSyncTime( 0u ),
  mLastVSyncTimeAtUpdate( 0u ),
  mLastVSyncFrameNumber( 0u ),
  mLastUpdateFrameNumber( 0u ),
  mRunning( true ),
  mFirstFrame( true ),
  writePos( 0u ),
  mExtraUpdatesSinceVSync( 0u )
{
  // Clear buffer
  for ( unsigned int i = 0; i < HISTORY_SIZE; ++i )
  {
    mPreviousUpdateFrames[i] = 0;
  }

  SetLastVSyncTime();
  mLastVSyncTimeAtUpdate = mLastVSyncTime;

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "FrameTime Initialized\n" );
}

FrameTime::~FrameTime()
{
}

void FrameTime::SetMinimumFrameTimeInterval( unsigned int interval )
{
  mMinimumFrameTimeInterval = interval;
}

void FrameTime::SetVSyncTime( unsigned int frameNumber )
{
  // Only set the render time if we are running
  if ( mRunning )
  {
    SetLastVSyncTime();

    mLastVSyncFrameNumber = frameNumber;

    DALI_LOG_INFO( gLogFilter, Debug::General, "FrameTime: Frame: %u: Time: %u\n", mLastVSyncFrameNumber, (unsigned int) ( mLastVSyncTime / MICROSECONDS_PER_MILLISECOND ) );
  }
}

void FrameTime::Suspend()
{
  mRunning = false;

  // Reset members
  mLastVSyncFrameNumber = 0;
  mLastUpdateFrameNumber = 0;
  writePos = 0;
  mExtraUpdatesSinceVSync = 0;

  // Clear buffer
  for ( unsigned int i = 0; i < HISTORY_SIZE; ++i )
  {
    mPreviousUpdateFrames[i] = 0;
  }

  DALI_LOG_INFO( gLogFilter, Debug::Concise, "FrameTime: Suspended\n" );
}

void FrameTime::Resume()
{
  DALI_LOG_INFO( gLogFilter, Debug::Concise, "FrameTime: Resuming\n" );

  SetLastVSyncTime();   // Should only update the last VSync time so the elapsed time during suspension is taken into consideration when we next update.
  mFirstFrame = true;

  mRunning = true;
}

void FrameTime::Sleep()
{
  DALI_LOG_INFO( gLogFilter, Debug::Concise, "FrameTime: Sleeping\n" );

  // Mimic Suspend behaviour
  Suspend();
}

void FrameTime::WakeUp()
{
  DALI_LOG_INFO( gLogFilter, Debug::Concise, "FrameTime: Waking Up\n" );

  SetLastVSyncTime();
  mLastVSyncTimeAtUpdate = mLastVSyncTime; // We do not want any animations to progress as we have just been woken up.
  mFirstFrame = true;

  mRunning = true;
}

void FrameTime::PredictNextRenderTime( float& lastFrameDeltaSeconds, unsigned int& lastRenderTimeMilliseconds, unsigned int& nextRenderTimeMilliseconds )
{
  if ( mRunning )
  {
    const unsigned int minimumFrameTimeInterval( mMinimumFrameTimeInterval );
    const uint64_t lastVSyncTime( mLastVSyncTime );
    const unsigned int lastVSyncFrameNumber( mLastVSyncFrameNumber );

    float lastFrameDelta( 0.0f ); // Assume the last update frame delta is 0.
    unsigned int framesTillNextVSync( 1 ); // Assume next render will be in one VSync frame time.

    unsigned int framesInLastUpdate( lastVSyncFrameNumber - mLastUpdateFrameNumber );
    lastFrameDelta = lastVSyncTime - mLastVSyncTimeAtUpdate;

    // We should only evaluate the previous frame values if this is not the first frame.
    if ( !mFirstFrame )
    {
      // Check whether we have had any VSyncs since we last did an Update.
      if ( framesInLastUpdate == 0 )
      {
        // We have had another update before a VSync, increment counter.
        ++mExtraUpdatesSinceVSync;

        // This update frame will be rendered mUpdatesSinceVSync later.
        framesTillNextVSync += mExtraUpdatesSinceVSync;
      }
      else
      {
        mExtraUpdatesSinceVSync = 0;
      }

      // If more than one frame elapsed since last Update, then check if this is a recurring theme so we can accurately predict when this Update is rendered.
      if ( framesInLastUpdate > 1 )
      {
        unsigned int average(0);
        for ( unsigned int i = 0; i < HISTORY_SIZE; ++i )
        {
          average += mPreviousUpdateFrames[i];
        }
        average /= HISTORY_SIZE;

        if ( average > 1 )
        {
          // Our average shows a recurring theme, we are missing frames when rendering so calculate number of frames this will take.
          framesTillNextVSync = average;
        }
      }

      // Write the number of frames the last update took to the array.
      mPreviousUpdateFrames[writePos] = framesInLastUpdate;
      writePos = ( writePos + 1 ) % HISTORY_SIZE;
    }

    mLastUpdateFrameNumber = lastVSyncFrameNumber;
    mLastVSyncTimeAtUpdate = lastVSyncTime;
    mFirstFrame = false;

    // Calculate the time till the next render
    unsigned int timeTillNextRender( minimumFrameTimeInterval * framesTillNextVSync );

    // Set the input variables
    lastFrameDeltaSeconds = lastFrameDelta * MICROSECONDS_TO_SECONDS;
    lastRenderTimeMilliseconds = lastVSyncTime / MICROSECONDS_PER_MILLISECOND;
    nextRenderTimeMilliseconds = ( lastVSyncTime + timeTillNextRender ) / MICROSECONDS_PER_MILLISECOND;

    DALI_LOG_INFO( gLogFilter, Debug::General, "FrameTime: Frame: %u, Time: %u, NextTime: %u, LastDelta: %f\n", mLastUpdateFrameNumber, lastRenderTimeMilliseconds, nextRenderTimeMilliseconds, lastFrameDeltaSeconds );
    DALI_LOG_INFO( gLogFilter, Debug::Verbose, "                      FramesInLastUpdate: %u, FramesTillNextVSync: %u\n", framesInLastUpdate, framesTillNextVSync );
  }
}

inline void FrameTime::SetLastVSyncTime()
{
  unsigned int seconds( 0u );
  unsigned int microseconds( 0u );

  mPlatform.GetTimeMicroseconds( seconds, microseconds );

  mLastVSyncTime = seconds;
  mLastVSyncTime = ( mLastVSyncTime * MICROSECONDS_PER_SECOND ) + microseconds;
}

} // namespace Internal

} // namespace Dali
