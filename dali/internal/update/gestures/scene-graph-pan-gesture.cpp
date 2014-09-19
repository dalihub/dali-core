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
const int MAX_GESTURE_AGE = 50; ///< maximum age of a gesture before disallowing its use in algorithm
const unsigned int DEFAULT_PREDICTION_INTERPOLATION = 0; ///< how much to interpolate pan position and displacement from last vsync time
const float DEFAULT_SMOOTHING_AMOUNT = 1.0f; ///< how much to interpolate pan position and displacement from last vsync time
} // unnamed namespace

const PanGesture::PredictionMode PanGesture::DEFAULT_PREDICTION_MODE = PanGesture::PREDICTION_NONE;
const int PanGesture::NUM_PREDICTION_MODES = PanGesture::PREDICTION_1 + 1;

const PanGesture::SmoothingMode PanGesture::DEFAULT_SMOOTHING_MODE = PanGesture::SMOOTHING_LAST_VALUE;
const int PanGesture::NUM_SMOOTHING_MODES = PanGesture::SMOOTHING_LAST_VALUE + 1;

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

void PanGesture::RemoveOldHistory(PanInfoHistory& panHistory, unsigned int currentTime, unsigned int maxAge, unsigned int minEvents)
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

  // dont want more than 5 previous predictions for smoothing
  iter = mPredictionHistory.begin();
  while( mPredictionHistory.size() > 1 && iter != mPredictionHistory.end() )
  {
    iter = mPredictionHistory.erase(iter);
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
  Vector2 screenDisplacement = gestureOut.screen.displacement;
  Vector2 localDisplacement = gestureOut.local.displacement;

  bool havePreviousAcceleration = false;
  bool previousVelocity = false;
  float previousAccel = 0.0f;
  unsigned int lastTime(0);

  unsigned int interpolationTime = lastVSyncTime + mPredictionAmount;
  if( interpolationTime > gestureOut.time ) // Guard against the rare case when gestureOut.time > (lastVSyncTime + mPredictionAmount)
  {
    interpolationTime -= gestureOut.time;
  }
  else
  {
    interpolationTime = 0u;
  }

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

    float time(0.f);
    if (currentGesture.time > lastTime) // Guard against invalid timestamps
    {
      time = static_cast<float>( currentGesture.time - lastTime );
    }
    if( time > Math::MACHINE_EPSILON_1 )
    {
      acceleration = velDiff / time;
    }

    float newVelMag = 0.0f;
    int currentInterpolation = interpolationTime;
    if( !havePreviousAcceleration )
    {
      newVelMag =  velMag;
      havePreviousAcceleration = true;
    }
    else
    {
      newVelMag = velMag + (((acceleration * (1.0f - previousValueWeight)) + (previousAccel * previousValueWeight)) * currentInterpolation);
    }
    float velMod = 1.0f;
    if( velMag > Math::MACHINE_EPSILON_1 )
    {
      velMod = newVelMag / velMag;
    }
    gestureOut.screen.velocity = currentGesture.screen.velocity * velMod;
    gestureOut.local.velocity = currentGesture.local.velocity * velMod;
    screenDisplacement = gestureOut.screen.displacement + (gestureOut.screen.velocity * interpolationTime);
    localDisplacement = gestureOut.local.displacement + (gestureOut.local.velocity * interpolationTime);
    screenVelocity = currentGesture.screen.velocity;
    localVelocity = currentGesture.local.velocity;
    previousAccel = acceleration;
    ++iter;
  }
  // gestureOut's position is currently equal to the last event's position and its displacement is equal to last frame's total displacement
  // add interpolated distance and position to current
  // work out interpolated velocity
  gestureOut.screen.position = (gestureOut.screen.position - gestureOut.screen.displacement) + screenDisplacement;
  gestureOut.local.position = (gestureOut.local.position - gestureOut.local.displacement) + localDisplacement;
  gestureOut.screen.displacement = screenDisplacement;
  gestureOut.local.displacement = localDisplacement;
  gestureOut.time += interpolationTime;
}

void PanGesture::SmoothingAlgorithm1(bool justStarted, PanInfo& gestureOut, unsigned int lastVSyncTime)
{
  if( !justStarted )
  {
    gestureOut.screen.position -= (gestureOut.screen.position - mLastGesture.screen.position) * 0.5f * (1.0f - mSmoothingAmount);
    gestureOut.local.position -= (gestureOut.local.position - mLastGesture.local.position) * 0.5f * (1.0f - mSmoothingAmount);
    // make current displacement relative to previous update-frame now.
    gestureOut.screen.displacement = gestureOut.screen.position - mLastGesture.screen.position;
    gestureOut.local.displacement = gestureOut.local.position - mLastGesture.local.position;
    // calculate velocity relative to previous update-frame
    unsigned int timeDiff( gestureOut.time - mLastGesture.time );
    gestureOut.screen.velocity = gestureOut.screen.displacement / timeDiff;
    gestureOut.local.velocity = gestureOut.local.displacement / timeDiff;
  }
}

void PanGesture::SmoothingAlgorithm2(bool justStarted, PanInfo& gestureOut, unsigned int lastVSyncTime)
{
  // push back result
  mPredictionHistory.push_back(gestureOut);

  // now smooth current pan event
  PanInfoHistoryConstIter endIter = mPredictionHistory.end() - 1;
  PanInfoHistoryIter iter = mPredictionHistory.begin();

  float distanceMod = 1.0f;
  float weight = 0.8f;
  while( iter != endIter )
  {
    PanInfo currentGesture = *iter;
    float newDistanceMod = currentGesture.screen.displacement.Length() / gestureOut.screen.displacement.Length();
    distanceMod = ((distanceMod * weight) + (newDistanceMod * (1.0f - weight)));
    weight -= 0.15f;
    ++iter;
  }
  gestureOut.screen.position -= gestureOut.screen.displacement;
  gestureOut.local.position -= gestureOut.local.displacement;
  gestureOut.screen.displacement *= distanceMod;
  gestureOut.local.displacement *= distanceMod;
  gestureOut.screen.position += gestureOut.screen.displacement;
  gestureOut.local.position += gestureOut.local.displacement;
}

bool PanGesture::UpdateProperties( unsigned int lastVSyncTime, unsigned int nextVSyncTime )
{
  if( !mInGesture )
  {
    // clear current pan history
    mPanHistory.clear();
    mPredictionHistory.clear();
  }

  // create an event for this frame
  bool justStarted ( false );
  bool justFinished ( false );
  bool eventFound( false );

  // Not going through array from the beginning, using it as a circular buffer and only using unread
  // values.
  int eventsThisFrame = 0;

  // create PanInfo to pass into prediction method
  mLastEventGesture = mEventGesture;
  mLastGesture = mLatestGesture;
  // add new gestures and work out one full gesture for the frame
  while(mReadPosition != mWritePosition)
  {
    // Copy the gesture first
    PanInfo currentGesture(mGestures[mReadPosition]);

    if( mProfiling )
    {
      mProfiling->mRawData.push_back( PanGestureProfiling::Position( currentGesture.time, currentGesture.screen.position ) );
    }
    mEventGesture.local.position = currentGesture.local.position;
    mEventGesture.local.velocity = currentGesture.local.velocity;
    mEventGesture.screen.position = currentGesture.screen.position;
    mEventGesture.screen.velocity = currentGesture.screen.velocity;
    if( !eventFound )
    {
      mEventGesture.local.displacement = currentGesture.local.displacement;
      mEventGesture.screen.displacement = currentGesture.screen.displacement;
    }
    else
    {
      mEventGesture.local.displacement += currentGesture.local.displacement;
      mEventGesture.screen.displacement += currentGesture.screen.displacement;
    }
    eventFound = true;
    mEventGesture.time = currentGesture.time;

    // add event to history
    mPanHistory.push_back(currentGesture);
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
  mLatestGesture = mEventGesture;

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
      case PREDICTION_NONE:
      {
        updateProperties = eventFound;
        // dont want event time
        unsigned int time = mLastGesture.time;
        mLastGesture = mLastEventGesture;
        mLastGesture.time = time;
        mLatestGesture.time = lastVSyncTime;
        break;
      }
      case PREDICTION_1:
      {
        // make latest gesture equal to current gesture before interpolation
        PredictiveAlgorithm1(eventsThisFrame, mLatestGesture, mPanHistory, lastVSyncTime, nextVSyncTime);
        updateProperties = true;
        break;
      }
    }

    switch( mSmoothingMode )
    {
      case SMOOTHING_NONE:
      {
        // no smoothing
        break;
      }
      case SMOOTHING_LAST_VALUE:
      {
        SmoothingAlgorithm1(justStarted, mLatestGesture, lastVSyncTime);
        break;
      }
    }

    if( updateProperties )
    {
      // only update properties if event received
      // set latest gesture to raw pan info with unchanged time
      mPanning.Set( mInGesture & !justFinished );
      mScreenPosition.Set( mLatestGesture.screen.position );
      mScreenDisplacement.Set( mLatestGesture.screen.displacement );
      mScreenVelocity.Set( mLatestGesture.screen.velocity );
      mLocalPosition.Set( mLatestGesture.local.position );
      mLocalDisplacement.Set( mLatestGesture.local.displacement );
      mLocalVelocity.Set( mLatestGesture.local.velocity );
    }

    if( mProfiling )
    {
      mProfiling->mAveragedData.push_back( PanGestureProfiling::Position( mLatestGesture.time, mLatestGesture.screen.position ) );
    }
  }

  mInGesture &= ~justFinished;

  if( mProfiling && justFinished )
  {
    mProfiling->PrintData();
    mProfiling->ClearData();
  }

  return updateProperties;
}

const GesturePropertyBool& PanGesture::GetPanningProperty() const
{
  return mPanning;
}

const GesturePropertyVector2& PanGesture::GetScreenPositionProperty() const
{
  return mScreenPosition;
}

const GesturePropertyVector2& PanGesture::GetScreenVelocityProperty() const
{
  return mScreenVelocity;
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

const GesturePropertyVector2& PanGesture::GetLocalVelocityProperty() const
{
  return mLocalVelocity;
}

void PanGesture::SetPredictionMode(PredictionMode mode)
{
  mPredictionMode = mode;
}

void PanGesture::SetPredictionAmount(unsigned int amount)
{
  mPredictionAmount = amount;
}

void PanGesture::SetSmoothingMode(SmoothingMode mode)
{
  mSmoothingMode = mode;
}

void PanGesture::SetSmoothingAmount(float amount)
{
  mSmoothingAmount = amount;
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
  mPredictionAmount(DEFAULT_PREDICTION_INTERPOLATION),
  mSmoothingMode(DEFAULT_SMOOTHING_MODE),
  mSmoothingAmount(DEFAULT_SMOOTHING_AMOUNT),
  mProfiling( NULL )
{
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
