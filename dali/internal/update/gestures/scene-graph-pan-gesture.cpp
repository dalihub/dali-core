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
const unsigned int UPDATES_BETWEEN_PRINT( 120u );
unsigned int UPDATE_COUNT( 0u );
const int MAX_GESTURE_AGE = 100; ///< maximum age of a gesture before disallowing its use in algorithm
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
    panHistory.erase(iter);
    endIter = panHistory.end();
  }
}

void PanGesture::SimpleAverageAlgorithm(bool justStarted, PanInfo& gestureOut)
{
  if( mInGesture )
  {
    if( !justStarted )
    {
      gestureOut.screen.position += mLatestGesture.screen.position;
      gestureOut.local.position += mLatestGesture.local.position;
      gestureOut.screen.position *= 0.5f;
      gestureOut.local.position *= 0.5f;
      // make current displacement relative to previous update-frame now.
      gestureOut.screen.displacement -= mLatestGesture.screen.displacement;
      gestureOut.local.displacement -= mLatestGesture.local.displacement;
    }
  }
}

void PanGesture::PredictiveAlgorithm1(int eventsThisFrame, PanInfo& gestureOut, PanInfoHistory& panHistory, unsigned int lastVSyncTime, unsigned int nextVSyncTime)
{
  RemoveOldHistory(panHistory, lastVSyncTime, MAX_GESTURE_AGE, 1);
  size_t panHistorySize = panHistory.size();

  PanInfoHistoryConstIter endIter = panHistory.end();
  PanInfoHistoryIter iter = panHistory.end() - (panHistorySize > 2 ? 2 : panHistorySize);
  if( panHistorySize >= 2 )
  {
    // create average velocity and acceleration
    // gestureOut is the combination of gesture events from this frame
    PanInfo lastGesture = *iter;
    ++iter;
    float previousAccel = 0.0f;
    while( iter != endIter )
    {
      PanInfo currentGesture = *iter;
      lastGesture = *(iter - 1);
      float velMag = currentGesture.screen.velocity.Length();
      float velDiff = velMag - lastGesture.screen.velocity.Length();
      float acceleration = 0.0f;
      float time = (float)(currentGesture.time - lastGesture.time);
      if( time >= Math::MACHINE_EPSILON_1 )
      {
        acceleration = velDiff / time;
      }
      float interpolationTime = (float)((int)lastVSyncTime - (int)currentGesture.time);
      float newVelMag = velMag + (((acceleration + previousAccel) * 0.5f) * interpolationTime);
      float velMod = 1.0f;
      if( velMag > Math::MACHINE_EPSILON_1 )
      {
        velMod = newVelMag / velMag;
      }
      gestureOut.screen.velocity *= velMod;
      gestureOut.local.velocity *= velMod;
      lastGesture = *iter;
      previousAccel = acceleration;
      ++iter;
    }
  }
  // gestureOut's position is currently equal to the last event's position and its displacement is equal to last frame's total displacement
  // add interpolated distance and position to current
  float interpolationTime = (float)((int)lastVSyncTime - (int)gestureOut.time);
  // work out interpolated velocity
  gestureOut.screen.displacement = (gestureOut.screen.velocity * interpolationTime);
  gestureOut.local.displacement = (gestureOut.local.velocity * interpolationTime);
  gestureOut.screen.position += gestureOut.screen.displacement;
  gestureOut.local.position += gestureOut.local.displacement;
  gestureOut.time += interpolationTime;
}

void PanGesture::PredictiveAlgorithm2(int eventsThisFrame, PanInfo& gestureOut, PanInfoHistory& panHistory, unsigned int lastVSyncTime, unsigned int nextVSyncTime)
{
  // TODO - adapt PredictiveAlgorithm1 with better smoothing, still under development
  RemoveOldHistory(panHistory, lastVSyncTime, MAX_GESTURE_AGE, 1);
  size_t panHistorySize = panHistory.size();

  PanInfoHistoryConstIter endIter = panHistory.end();
  PanInfoHistoryIter iter = panHistory.end() - (panHistorySize > 2 ? 2 : panHistorySize);
  if( panHistorySize >= 2 )
  {
    // create average velocity and acceleration
    // gestureOut is the combination of gesture events from this frame
    PanInfo lastGesture = *iter;
    ++iter;
    float previousAccel = 0.0f;
    while( iter != endIter )
    {
      PanInfo currentGesture = *iter;
      lastGesture = *(iter - 1);
      float velMag = currentGesture.screen.velocity.Length();
      float velDiff = velMag - lastGesture.screen.velocity.Length();
      float acceleration = 0.0f;
      float time = (float)(currentGesture.time - lastGesture.time);
      if( time >= Math::MACHINE_EPSILON_1 )
      {
        acceleration = velDiff / time;
      }
      float interpolationTime = (float)((int)lastVSyncTime - (int)currentGesture.time);
      float newVelMag = velMag + (((acceleration + previousAccel) * 0.5f) * interpolationTime);
      float velMod = 1.0f;
      if( velMag > Math::MACHINE_EPSILON_1 )
      {
        velMod = newVelMag / velMag;
      }
      gestureOut.screen.velocity *= velMod;
      gestureOut.local.velocity *= velMod;
      lastGesture = *iter;
      previousAccel = acceleration;
      ++iter;
    }
  }
  // gestureOut's position is currently equal to the last event's position and its displacement is equal to last frame's total displacement
  // add interpolated distance and position to current
  float interpolationTime = (float)((int)lastVSyncTime - (int)gestureOut.time);
  // work out interpolated velocity
  gestureOut.screen.displacement = (gestureOut.screen.velocity * interpolationTime);
  gestureOut.local.displacement = (gestureOut.local.velocity * interpolationTime);
  gestureOut.screen.position += gestureOut.screen.displacement;
  gestureOut.local.position += gestureOut.local.displacement;
  gestureOut.time += interpolationTime;
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
  // set nextGesture to last gesture so it's position is correct and velocity is same as last frame
  mEventGesture = mLatestGesture;
  // add new gestures and work out one full gesture for the frame
  while(mReadPosition != mWritePosition)
  {
    // Copy the gesture first
    PanInfo currentGesture(mGestures[mReadPosition]);

    if( mProfiling )
    {
      mProfiling->mRawData.push_back( PanGestureProfiling::Position( currentGesture.time, currentGesture.screen.position ) );
    }

    if ( currentGesture.time < mEventGesture.time )
    {
      break;
    }

    mEventGesture.local.position = currentGesture.local.position;
    mEventGesture.local.velocity = currentGesture.local.velocity;
    mEventGesture.screen.position = currentGesture.screen.position;
    mEventGesture.screen.velocity = currentGesture.screen.velocity;
    if( !eventFound )
    {
      mEventGesture.local.displacement = currentGesture.local.displacement;
      mEventGesture.screen.displacement = currentGesture.screen.displacement;
      eventFound = true;
    }
    else
    {
      mEventGesture.local.displacement += currentGesture.local.displacement;
      mEventGesture.screen.displacement += currentGesture.screen.displacement;
    }
    mEventGesture.time = currentGesture.time;

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
    ++mReadPosition;
    ++eventsThisFrame;
    mReadPosition %= PAN_GESTURE_HISTORY;
  }

  // create PanInfo to pass into prediction method
  PanInfo nextGesture(mEventGesture);

  mInGesture |= justStarted;

  bool updateProperties = false;

  if ( mInGesture )
  {
    if( mProfiling )
    {
      mProfiling->mLatestData.push_back( PanGestureProfiling::Position( lastVSyncTime, mLatestGesture.screen.position ) );
    }

    switch( mPredictionMode )
    {
      case NONE:
      {
        updateProperties = eventFound;
        // no prediction, just using latest event info
        mLatestGesture = mEventGesture;
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
        mLatestGesture = nextGesture;
        PredictiveAlgorithm1(eventsThisFrame, nextGesture, mPanHistory, lastVSyncTime, nextVSyncTime);
        updateProperties = true;
        break;
      }
      case PREDICTION_2:
      {
        // make latest gesture equal to current gesture before interpolation
        mLatestGesture = nextGesture;
        PredictiveAlgorithm2(eventsThisFrame, nextGesture, mPanHistory, lastVSyncTime, nextVSyncTime);
        updateProperties = true;
        break;
      }
    }

    // always keep latest gesture up to date with event gesture
    mLatestGesture = mEventGesture;

    if( updateProperties )
    {
      // only update properties if event received
      // set latest gesture to raw pan info with unchanged time
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

  mInGesture &= ~justFinished;

  if( mProfiling &&
      UPDATE_COUNT++ >= UPDATES_BETWEEN_PRINT )
  {
    mProfiling->PrintData();
    mProfiling->ClearData();
    UPDATE_COUNT = 0u;
  }

  return propertiesUpdated;
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
