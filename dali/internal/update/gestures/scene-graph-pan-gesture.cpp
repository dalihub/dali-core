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
#include <dali/internal/update/gestures/pan-gesture-profiling.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{
namespace
{
const int MAX_GESTURE_AGE = 50; ///< maximum age of a gesture before disallowing its use in algorithm
} // unnamed namespace

const PanGesture::PredictionMode PanGesture::DEFAULT_PREDICTION_MODE = PanGesture::AVERAGE;
const int PanGesture::NUM_PREDICTION_MODES = PanGesture::PREDICTION_2 + 1;

PanGesture* PanGesture::New()
{
  return new PanGesture();
}

PanGesture::~PanGesture()
{
  delete mProfiling;
}

void PanGesture::AddGesture( const Dali::PanGesture& gesture )
{
  mGestures[ mWritePosition ] = gesture;

  // Update our write position.
  ++mWritePosition;
  mWritePosition %= PAN_GESTURE_HISTORY;
}

void PanGesture::RemoveOldHistory(PanInfoHistory& panHistory, uint currentTime, uint maxAge, uint minEvents)
{
  PanInfoHistoryConstIter endIter = panHistory.end();
  PanInfoHistoryIter iter = panHistory.begin();
  while( iter != endIter && panHistory.size() > minEvents)
  {
    PanInfo currentGesture = *iter;
    if( currentTime < currentGesture.time + maxAge )
    {
      break;
    }
    iter = panHistory.erase(iter);
    endIter = panHistory.end();
  }
}

void PanGesture::SimpleAverageAlgorithm(bool justStarted, PanInfo& gestureOut)
{
  if( mInGesture )
  {
    if( !justStarted )
    {
      gestureOut.screen.position += mLastEventGesture.screen.position;
      gestureOut.local.position += mLastEventGesture.local.position;
      gestureOut.screen.position *= 0.5f;
      gestureOut.local.position *= 0.5f;
      // make current displacement relative to previous update-frame now.
      gestureOut.screen.displacement = mLastEventGesture.screen.position - gestureOut.screen.position;
      gestureOut.local.displacement = mLastEventGesture.local.position - gestureOut.local.position;
    }
  }
}

void PanGesture::PredictiveAlgorithm1(int eventsThisFrame, PanInfo& gestureOut, PanInfoHistory& panHistory, unsigned int lastVSyncTime, unsigned int nextVSyncTime)
{
  RemoveOldHistory(panHistory, lastVSyncTime, MAX_GESTURE_AGE, 0);
  size_t panHistorySize = panHistory.size();
  if( panHistorySize == 0 )
  {
    // cant do any prediction without a history
    return;
  }

  PanInfoHistoryConstIter endIter = panHistory.end();
  PanInfoHistoryIter iter = panHistory.begin();
  Vector2 screenVelocity = gestureOut.screen.velocity;
  Vector2 localVelocity = gestureOut.local.velocity;

  bool havePreviousAcceleration = false;
  bool previousVelocity = false;
  float previousAccel = 0.0f;
  unsigned int lastTime;
  while( iter != endIter )
  {
    PanInfo currentGesture = *iter;
    if( !previousVelocity )
    {
      // not yet set a previous velocity
      screenVelocity = currentGesture.screen.velocity;
      previousVelocity = true;
      lastTime = currentGesture.time;
      ++iter;
      continue;
    }
    float velMag = currentGesture.screen.velocity.Length();
    float velDiff = velMag - screenVelocity.Length();
    float acceleration = 0.0f;
    float time = (float)(currentGesture.time - lastTime);
    if( time > Math::MACHINE_EPSILON_1 )
    {
      acceleration = velDiff / time;
    }
    float interpolationTime = (float)((int)lastVSyncTime - (int)currentGesture.time);
    float newVelMag = 0.0f;
    if( !havePreviousAcceleration )
    {
      newVelMag =  velMag + (acceleration * interpolationTime);
      havePreviousAcceleration = true;
    }
    else
    {
      newVelMag = velMag + (((acceleration + previousAccel) * 0.5f) * interpolationTime);
    }
    float velMod = 1.0f;
    if( velMag > Math::MACHINE_EPSILON_1 )
    {
      velMod = newVelMag / velMag;
    }
    gestureOut.screen.velocity = currentGesture.screen.velocity * velMod;
    gestureOut.local.velocity = currentGesture.local.velocity * velMod;
    screenVelocity = currentGesture.screen.velocity;
    localVelocity = currentGesture.local.velocity;
    previousAccel = acceleration;
    ++iter;
  }
  // gestureOut's position is currently equal to the last event's position and its displacement is equal to last frame's total displacement
  // add interpolated distance and position to current
  float interpolationTime = (float)((int)lastVSyncTime - (int)gestureOut.time);
  // work out interpolated velocity
  gestureOut.screen.displacement = (gestureOut.screen.velocity * interpolationTime);
  gestureOut.local.displacement = (gestureOut.local.velocity * interpolationTime);
  gestureOut.screen.position += gestureOut.screen.displacement;
  gestureOut.local.position += gestureOut.local.displacement;
  gestureOut.time += (lastVSyncTime - gestureOut.time);
}

void PanGesture::PredictiveAlgorithm2(int eventsThisFrame, PanInfo& gestureOut, PanInfoHistory& panHistory, unsigned int lastVSyncTime, unsigned int nextVSyncTime)
{
  // TODO - adapt PredictiveAlgorithm1 with better smoothing, still under development
  RemoveOldHistory(panHistory, lastVSyncTime, MAX_GESTURE_AGE, 0);
  size_t panHistorySize = panHistory.size();
  if( panHistorySize == 0 )
  {
    // cant do any prediction without a history
    return;
  }

  PanInfoHistoryConstIter endIter = panHistory.end();
  PanInfoHistoryIter iter = panHistory.begin();
  Vector2 screenVelocity = gestureOut.screen.velocity;
  Vector2 localVelocity = gestureOut.local.velocity;

  bool havePreviousAcceleration = false;
  bool previousVelocity = false;
  float previousAccel = 0.0f;
  unsigned int lastTime;
  while( iter != endIter )
  {
    PanInfo currentGesture = *iter;
    if( !previousVelocity )
    {
      // not yet set a previous velocity
      screenVelocity = currentGesture.screen.velocity;
      previousVelocity = true;
      lastTime = currentGesture.time;
      ++iter;
      continue;
    }
    float previousValueWeight = (float)(MAX_GESTURE_AGE - (lastVSyncTime - lastTime)) / (float)MAX_GESTURE_AGE;
    float velMag = currentGesture.screen.velocity.Length();
    float velDiff = velMag - screenVelocity.Length();
    float acceleration = 0.0f;
    float time = (float)(currentGesture.time - lastTime);
    if( time > Math::MACHINE_EPSILON_1 )
    {
      acceleration = velDiff / time;
    }
    float interpolationTime = (float)((int)lastVSyncTime - (int)currentGesture.time);
    float newVelMag = 0.0f;
    if( !havePreviousAcceleration )
    {
      newVelMag =  velMag + (acceleration * interpolationTime);
      havePreviousAcceleration = true;
    }
    else
    {
      newVelMag = velMag + (((acceleration * (1.0f - previousValueWeight)) + (previousAccel * previousValueWeight)) * interpolationTime);
    }
    float velMod = 1.0f;
    if( velMag > Math::MACHINE_EPSILON_1 )
    {
      velMod = newVelMag / velMag;
    }
    gestureOut.screen.velocity = currentGesture.screen.velocity * velMod;
    gestureOut.local.velocity = currentGesture.local.velocity * velMod;
    screenVelocity = currentGesture.screen.velocity;
    localVelocity = currentGesture.local.velocity;
    previousAccel = acceleration;
    ++iter;
  }
  // gestureOut's position is currently equal to the last event's position and its displacement is equal to last frame's total displacement
  // add interpolated distance and position to current
  float interpolationTime = (float)((int)lastVSyncTime - (int)gestureOut.time);
  // work out interpolated velocity
  gestureOut.screen.displacement = (gestureOut.screen.velocity * interpolationTime);
  gestureOut.local.displacement = (gestureOut.local.velocity * interpolationTime);
  gestureOut.screen.position += gestureOut.screen.displacement;
  gestureOut.local.position += gestureOut.local.displacement;
  gestureOut.time += (lastVSyncTime - gestureOut.time);
}

bool PanGesture::UpdateProperties( unsigned int lastVSyncTime, unsigned int nextVSyncTime )
{
  bool propertiesUpdated( false );

  if( !mInGesture )
  {
    // clear current pan history
    mPanHistory.clear();
  }

  // create an event for this frame
  bool justStarted ( false );
  bool justFinished ( false );
  bool eventFound( false );

  // Not going through array from the beginning, using it as a circular buffer and only using unread
  // values.
  int eventsThisFrame = 0;

  // create PanInfo to pass into prediction method
  PanInfo nextGesture = mEventGesture;
  // add new gestures and work out one full gesture for the frame
  while(mReadPosition != mWritePosition)
  {
    // Copy the gesture first
    PanInfo currentGesture(mGestures[mReadPosition]);

    if( mProfiling )
    {
      mProfiling->mRawData.push_back( PanGestureProfiling::Position( currentGesture.time, currentGesture.screen.position ) );
    }
    nextGesture.local.position = currentGesture.local.position;
    nextGesture.local.velocity = currentGesture.local.velocity;
    nextGesture.screen.position = currentGesture.screen.position;
    nextGesture.screen.velocity = currentGesture.screen.velocity;
    if( !eventFound )
    {
      nextGesture.local.displacement = currentGesture.local.displacement;
      nextGesture.screen.displacement = currentGesture.screen.displacement;
    }
    else
    {
      nextGesture.local.displacement += currentGesture.local.displacement;
      nextGesture.screen.displacement += currentGesture.screen.displacement;
    }
    eventFound = true;
    nextGesture.time = currentGesture.time;

    // add event to history
    mPanHistory.push_back(currentGesture);
    justStarted |= (currentGesture.state == Gesture::Started);
    if( currentGesture.state == Gesture::Started )
    {
      justStarted = true;
      // clear just finished as we have started new pan
      justFinished = false;
    }
    justFinished |= (currentGesture.state == Gesture::Finished || currentGesture.state == Gesture::Cancelled);

    // Update our read position.
    ++eventsThisFrame;
    ++mReadPosition;
    mReadPosition %= PAN_GESTURE_HISTORY;
  }
  // set nextGesture to last gesture so it's position is correct and velocity is same as last frame
  mEventGesture = nextGesture;

  mInGesture |= justStarted;

  bool updateProperties = false;

  if ( mInGesture )
  {
    if( mProfiling )
    {
      mProfiling->mLatestData.push_back( PanGestureProfiling::Position( lastVSyncTime, mEventGesture.screen.position ) );
    }

    switch( mPredictionMode )
    {
      case NONE:
      {
        updateProperties = eventFound;
        break;
      }
      case AVERAGE:
      {
        SimpleAverageAlgorithm(justStarted, nextGesture);
        // make latest gesture equal to current gesture after averaging
        updateProperties = true;
        break;
      }
      case PREDICTION_1:
      {
        // make latest gesture equal to current gesture before interpolation
        PredictiveAlgorithm1(eventsThisFrame, nextGesture, mPanHistory, lastVSyncTime, nextVSyncTime);
        updateProperties = true;
        break;
      }
      case PREDICTION_2:
      {
        // make latest gesture equal to current gesture before interpolation
        PredictiveAlgorithm2(eventsThisFrame, nextGesture, mPanHistory, lastVSyncTime, nextVSyncTime);
        updateProperties = true;
        break;
      }
    }

    // always keep latest gesture up to date with event gesture
    mLatestGesture = nextGesture;

    if( updateProperties )
    {
      // only update properties if event received
      // set latest gesture to raw pan info with unchanged time
      mPanning.Set( mInGesture & !justFinished );
      mScreenPosition.Set( nextGesture.screen.position );
      mScreenDisplacement.Set( nextGesture.screen.displacement );
      mLocalPosition.Set( nextGesture.local.position );
      mLocalDisplacement.Set( nextGesture.local.displacement );

      propertiesUpdated = true;
    }

    if( mProfiling )
    {
      mProfiling->mAveragedData.push_back( PanGestureProfiling::Position( nextGesture.time, nextGesture.screen.position ) );
    }
  }
  mLastEventGesture = mEventGesture;

  mInGesture &= ~justFinished;

  if( mProfiling && justFinished )
  {
    mProfiling->PrintData();
    mProfiling->ClearData();
  }

  return propertiesUpdated;
}

const GesturePropertyBool& PanGesture::GetPanningProperty() const
{
  return mPanning;
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

void PanGesture::EnableProfiling()
{
  if( !mProfiling )
  {
    mProfiling = new PanGestureProfiling();
  }
}

void PanGesture::ResetDefaultProperties( BufferIndex updateBufferIndex )
{
  mScreenPosition.Reset();
  mScreenDisplacement.Reset();
  mLocalPosition.Reset();
  mLocalDisplacement.Reset();
  mPanning.Reset();
}

PanGesture::PanGesture()
: mGestures(),
  mWritePosition( 0 ),
  mReadPosition( 0 ),
  mInGesture( false ),
  mPredictionMode(DEFAULT_PREDICTION_MODE),
  mProfiling( NULL )
{
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
