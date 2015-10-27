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
#include <dali/internal/update/gestures/scene-graph-pan-gesture.h>

// EXTERNAL INCLUDES
#include <cmath>

// INTERNAL INCLUDES
#include <dali/internal/update/gestures/pan-gesture-profiling.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

namespace
{

// TODO: Experimental - for changing in code only:
const bool         TEST_TUNE_ENABLE_OVERSHOOT_PROTECTION   = false;

// Internal defaults:
const int          MAX_GESTURE_AGE                         = 200;    ///< maximum age of a gesture before disallowing its use in algorithm TODO: Possibly make this configurable.
const float        ACCELERATION_THRESHOLD                  = 0.1f;   ///< minimum pan velocity change to trigger dynamic change of prediction amount.
const float        OUTPUT_TIME_DIFFERENCE                  = ( 1000.0f / 60.0f ); ///< This is used to optionally override actual times if they make results worse.
const float        ACCELERATION_SMOOTHING                  = 0.44f;  ///< Smoothes acceleration changes from one frame to another.
const float        ACCELERATION_CAP                        = 0.0004f;///< Limits acceleration changes from one frame to another.

// Defaults for Environment Variables:

// Prediction Mode 1:
const unsigned int DEFAULT_MAX_PREDICTION_AMOUNT           = 32;     ///< the upper bound of the range to clamp the prediction interpolation.
const unsigned int DEFAULT_MIN_PREDICTION_AMOUNT           = 0;      ///< the lower bound of the range to clamp the prediction interpolation.
const unsigned int DEFAULT_PREDICTION_AMOUNT_ADJUSTMENT    = 2;      ///< the amount of prediction interpolation to adjust (in milliseconds) each time when pan velocity changes.

// Prediction Mode 2:
const bool         DEFAULT_USE_ACTUAL_TIMES                = false;  ///< Disable to optionally override actual times if they make results worse.
const int          DEFAULT_INTERPOLATION_TIME_RANGE        = 255;    ///< Time into past history (ms) to use points to interpolate the first point.
const bool         DEFAULT_SCALAR_ONLY_PREDICTION_ENABLED  = false;  ///< If enabled, prediction is done using velocity alone (no integration or acceleration).
const bool         DEFAULT_TWO_POINT_PREDICTION_ENABLED    = true;   ///< If enabled, a second interpolated point is predicted and combined with the first to get more stable values.
const int          DEFAULT_TWO_POINT_PAST_INTERPOLATE_TIME = 42;     ///< The target time in the past to generate the second interpolated point.
const float        DEFAULT_TWO_POINT_VELOCITY_BIAS         = 0.35f;  ///< The ratio of first and second interpolated points to use for velocity. 0.0f = 100% of first point. 1.0f = 100% of second point.
const float        DEFAULT_TWO_POINT_ACCELERATION_BIAS     = 0.10f;  ///< The ratio of first and second interpolated points to use for acceleration. 0.0f = 100% of first point. 1.0f = 100% of second point.
const int          DEFAULT_MULTITAP_SMOOTHING_RANGE        = 34;     ///< The range in time (ms) of points in the history to smooth the final output against.

// Prediction Modes 1 & 2.
const unsigned int DEFAULT_PREDICTION_AMOUNT[2]            = {     5,    57 }; ///< how much to interpolate pan position and displacement from last vsync time (in milliseconds)
const float        DEFAULT_SMOOTHING_AMOUNT[2]             = { 0.25f, 0.23f }; ///< how much to smooth final result from last vsync time

} // unnamed namespace

const PanGesture::PredictionMode PanGesture::DEFAULT_PREDICTION_MODE = PanGesture::PREDICTION_NONE;
const int PanGesture::NUM_PREDICTION_MODES = PanGesture::PREDICTION_2 + 1;

const PanGesture::SmoothingMode PanGesture::DEFAULT_SMOOTHING_MODE = PanGesture::SMOOTHING_LAST_VALUE;
const int PanGesture::NUM_SMOOTHING_MODES = PanGesture::SMOOTHING_MULTI_TAP + 1;

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
  Dali::Mutex::ScopedLock lock( mMutex );
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

void PanGesture::PredictionMode1(int eventsThisFrame, PanInfo& gestureOut, PanInfoHistory& panHistory, unsigned int lastVSyncTime, unsigned int nextVSyncTime)
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

  unsigned int interpolationTime = lastVSyncTime + mCurrentPredictionAmount;

  if( interpolationTime > gestureOut.time ) // Guard against the rare case when gestureOut.time > (lastVSyncTime + mCurrentPredictionAmount)
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

void PanGesture::BlendPoints( PanInfo& gesture, PanInfo& lastGesture, float blendValue )
{
  gesture.screen.position -= ( gesture.screen.position - lastGesture.screen.position ) * 0.5f * ( 1.0f - blendValue );
  gesture.local.position -= ( gesture.local.position - lastGesture.local.position ) * 0.5f * ( 1.0f - blendValue );
  // Make current displacement relative to previous update-frame now.
  gesture.screen.displacement = gesture.screen.position - lastGesture.screen.position;
  gesture.local.displacement = gesture.local.position - lastGesture.local.position;
  // Calculate velocity relative to previous update-frame
  float timeDifference( gesture.time - lastGesture.time );
  gesture.screen.velocity = gesture.screen.displacement / timeDifference;
  gesture.local.velocity = gesture.local.displacement / timeDifference;
}

bool PanGesture::ReadGestures( FrameGestureInfo& info, unsigned int currentTimestamp )
{
  unsigned int previousReadPosition = 0;
  bool eventFound = false;
  info.frameGesture = mLastUnmodifiedGesture;

  while( mReadPosition != mWritePosition )
  {
    // Copy the gesture first
    PanInfo currentGesture( mGestures[mReadPosition] );

    if( mProfiling )
    {
      mProfiling->mRawData.push_back( PanGestureProfiling::Position( currentGesture.time, currentGesture.screen.position, currentGesture.screen.displacement, currentGesture.screen.velocity, currentGesture.state ) );
    }
    info.frameGesture.local.position = currentGesture.local.position;
    info.frameGesture.local.velocity = currentGesture.local.velocity;
    info.frameGesture.screen.position = currentGesture.screen.position;
    info.frameGesture.screen.velocity = currentGesture.screen.velocity;

    if( info.eventsThisFrame > 0 )
    {
      info.acceleration = currentGesture.screen.velocity.Length() - mGestures[previousReadPosition].screen.velocity.Length();
    }

    if( !eventFound )
    {
      info.frameGesture.local.displacement = currentGesture.local.displacement;
      info.frameGesture.screen.displacement = currentGesture.screen.displacement;
      eventFound = true;
    }
    else
    {
      info.frameGesture.local.displacement += currentGesture.local.displacement;
      info.frameGesture.screen.displacement += currentGesture.screen.displacement;
    }
    info.frameGesture.time = currentGesture.time;

    // add event to history
    mPanHistory.push_back( currentGesture );
    if( currentGesture.state == Gesture::Started )
    {
      info.justStarted = true;
      // clear just finished as we have started new pan
      info.justFinished = false;
    }
    info.justFinished |= ( currentGesture.state == Gesture::Finished || currentGesture.state == Gesture::Cancelled );

    // Update our read position.
    previousReadPosition = mReadPosition;
    ++info.eventsThisFrame;
    ++mReadPosition;
    mReadPosition %= PAN_GESTURE_HISTORY;
  }
  // This code does not determine if the data will be used.
  return false;
}

bool PanGesture::ReadAndResampleGestures( FrameGestureInfo& info, unsigned int currentTimestamp )
{
  PanInfo lastReadGesture;
  Dali::Mutex::ScopedLock lock( mMutex );
  while( mReadPosition != mWritePosition )
  {
    // Copy the gesture first
    lastReadGesture = mGestures[mReadPosition];
    if( mProfiling )
    {
      mProfiling->mRawData.push_back( PanGestureProfiling::Position( lastReadGesture.time, lastReadGesture.screen.position,
          lastReadGesture.screen.displacement, lastReadGesture.screen.velocity, lastReadGesture.state ) );
    }

    info.frameGesture.screen.position += lastReadGesture.screen.position;
    info.frameGesture.local.position += lastReadGesture.local.position;
    info.frameGesture.screen.velocity += lastReadGesture.screen.velocity;
    info.frameGesture.local.velocity += lastReadGesture.local.velocity;

    if( lastReadGesture.state == Gesture::Started )
    {
      // Clear just finished as we have started new pan.
      info.justFinished = false;
      info.justStarted = true;
    }
    else
    {
      info.justFinished |= ( lastReadGesture.state == Gesture::Finished || lastReadGesture.state == Gesture::Cancelled );
    }

    // Add event to history
    mPanHistory.push_back( lastReadGesture );

    // Update our read position.
    ++info.eventsThisFrame;
    ++mReadPosition;
    mReadPosition %= PAN_GESTURE_HISTORY;
  }

  bool updateProperties = false;
  if( info.eventsThisFrame > 0 )
  {
    // Some events were read this frame.
    mTargetGesture = lastReadGesture;

    if( info.eventsThisFrame > 1 )
    {
      info.frameGesture.screen.position /= info.eventsThisFrame;
      info.frameGesture.local.position /= info.eventsThisFrame;
      info.frameGesture.screen.velocity /= info.eventsThisFrame;
      info.frameGesture.local.velocity /= info.eventsThisFrame;

      info.frameGesture.screen.displacement = info.frameGesture.screen.position - mLastGesture.screen.position;
      info.frameGesture.local.displacement = info.frameGesture.local.position - mLastGesture.local.position;

      mNotAtTarget = true;
    }
    else
    {
      info.frameGesture.screen.displacement = lastReadGesture.screen.displacement;
      info.frameGesture.local.displacement = lastReadGesture.local.displacement;
    }

    info.frameGesture.time = currentTimestamp;

    updateProperties = true;
  }
  else
  {
    // 0 Events this frame.
    if( mNotAtTarget )
    {
      mNotAtTarget = false;
      info.frameGesture = mTargetGesture;
      updateProperties = true;
    }
    else
    {
      info.frameGesture = mLastGesture;
    }
  }

  return updateProperties;
}

bool PanGesture::UpdateProperties( unsigned int lastVSyncTime, unsigned int nextVSyncTime )
{
  if( mPredictionMode == PREDICTION_2 )
  {
    // TODO: Have the two prediction modes share more behavior so some parts of mode 2 can
    // be used with mode 1 etc. Needs code moving and more importantly testing.
    return NewAlgorithm( lastVSyncTime, nextVSyncTime );
  }

  if( !mInGesture )
  {
    // clear current pan history
    mPanHistory.clear();
    mPredictionHistory.clear();
  }

  FrameGestureInfo frameInfo;
  bool updateProperties = false;

  // Read input data.
  // If we are using a form of prediction, read all the input as-is.
  if( mPredictionMode != PREDICTION_NONE )
  {
    // Read input required for prediction algorithms.
    updateProperties = ReadGestures( frameInfo, lastVSyncTime );
  }
  else
  {
    // Read and resample input.
    updateProperties = ReadAndResampleGestures( frameInfo, lastVSyncTime );
  }

  PanInfo frameGesture = frameInfo.frameGesture;
  PanInfo unmodifiedGesture = frameGesture;

  // Process input data.
  mInGesture |= frameInfo.justStarted;
  if( mInGesture )
  {
    // Profiling.
    if( mProfiling )
    {
      mProfiling->mLatestData.push_back( PanGestureProfiling::Position( lastVSyncTime, frameGesture.screen.position,
          frameGesture.screen.displacement, frameGesture.screen.velocity, frameGesture.state ) );
    }

    // Perform prediction.
    if( mPredictionMode == PREDICTION_1 )
    {
      // Dynamically change the prediction amount according to the pan velocity acceleration.
      if( !frameInfo.justStarted )
      {
        if( frameInfo.eventsThisFrame <= 1 )
        {
          frameInfo.acceleration = frameGesture.screen.velocity.Length() - mLastUnmodifiedGesture.screen.velocity.Length();
        }

        // Ignore tiny velocity fluctuation to avoid unnecessary prediction amount change
        if( fabsf( frameInfo.acceleration ) > ACCELERATION_THRESHOLD )
        {
          mCurrentPredictionAmount += mPredictionAmountAdjustment * ( frameInfo.acceleration > Math::MACHINE_EPSILON_0 ? 1.0f : -1.0f );
          if( mCurrentPredictionAmount > mMaxPredictionAmount + mPredictionAmountAdjustment ) // Guard against unsigned int overflow
          {
            mCurrentPredictionAmount = 0;
          }
        }
      }
      else
      {
        if( !mPredictionAmountOverridden )
        {
          // If the prediction amount has not been modified, default to the correct amount for this algorithm.
          mPredictionAmount = DEFAULT_PREDICTION_AMOUNT[0];
        }
        mCurrentPredictionAmount = mPredictionAmount; // Reset the prediction amount for each new gesture
      }

      mCurrentPredictionAmount = std::max( mMinPredictionAmount, std::min( mCurrentPredictionAmount, mMaxPredictionAmount ) );

      // Calculate the delta of positions before the prediction
      Vector2 deltaPosition = frameGesture.screen.position - mLastUnmodifiedGesture.screen.position;

      // Make latest gesture equal to current gesture before interpolation
      PredictionMode1( frameInfo.eventsThisFrame, frameGesture, mPanHistory, lastVSyncTime, nextVSyncTime );

      // Calculate the delta of positions after the prediction.
      Vector2 deltaPredictedPosition = frameGesture.screen.position - mLastGesture.screen.position;

      // If the change in the prediction has a different sign than the change in the actual position,
      // there is overshot (i.e. the current prediction is too large). Return the previous prediction
      // to give the user's finger a chance to catch up with where we have panned to.
      bool overshotXAxis = false;
      bool overshotYAxis = false;
      if( (deltaPosition.x > Math::MACHINE_EPSILON_0 && deltaPredictedPosition.x < Math::MACHINE_EPSILON_0 )
       || (deltaPosition.x < Math::MACHINE_EPSILON_0 && deltaPredictedPosition.x > Math::MACHINE_EPSILON_0 ) )
      {
        overshotXAxis = true;
        frameGesture.screen.position.x = mLastGesture.screen.position.x;
      }

      if( (deltaPosition.y > Math::MACHINE_EPSILON_0 && deltaPredictedPosition.y < Math::MACHINE_EPSILON_0 )
       || (deltaPosition.y < Math::MACHINE_EPSILON_0 && deltaPredictedPosition.y > Math::MACHINE_EPSILON_0 ) )
      {
        overshotYAxis = true;
        frameGesture.screen.position.y = mLastGesture.screen.position.y;
      }

      // If there is overshot in one axis, reduce the possible overshot in the other axis,
      // and reduce the prediction amount so that it doesn't overshoot as easily next time.
      if(overshotXAxis || overshotYAxis)
      {
        mCurrentPredictionAmount -= mPredictionAmountAdjustment;
        if( mCurrentPredictionAmount > mMaxPredictionAmount + mPredictionAmountAdjustment ) // Guard against unsigned int overflow
        {
          mCurrentPredictionAmount = 0;
        }
        mCurrentPredictionAmount = std::max( mMinPredictionAmount, std::min( mCurrentPredictionAmount, mMaxPredictionAmount ) );

        if( overshotXAxis && !overshotYAxis )
        {
          frameGesture.screen.position.y = ( mLastGesture.screen.position.y + frameGesture.screen.position.y ) * 0.5f;
        }

        if( overshotYAxis && !overshotXAxis )
        {
          frameGesture.screen.position.x = ( mLastGesture.screen.position.x + frameGesture.screen.position.x ) * 0.5f;
        }
      }

      updateProperties = true;
    }

    // Perform smoothing.
    switch( mSmoothingMode )
    {
      case SMOOTHING_NONE:
      case SMOOTHING_MULTI_TAP:
      {
        // No smoothing
        // TODO: Old algorithm to be able to use multitap smoothing.
        break;
      }
      case SMOOTHING_LAST_VALUE:
      {
        if( !frameInfo.justStarted )
        {
          if( !mSmoothingAmountOverridden )
          {
            // If the smoothing amount has not been modified, default to the correct amount for this algorithm.
            mSmoothingAmount = DEFAULT_SMOOTHING_AMOUNT[0];
          }
          BlendPoints( frameGesture, mLastGesture, mSmoothingAmount );
        }
        break;
      }
    }

    if( updateProperties )
    {
      // only update properties if event received
      // set latest gesture to raw pan info with unchanged time
      mPanning.Set( mInGesture & !frameInfo.justFinished );
      mScreenPosition.Set( frameGesture.screen.position );
      mScreenDisplacement.Set( frameGesture.screen.displacement );
      mScreenVelocity.Set( frameGesture.screen.velocity );
      mLocalPosition.Set( frameGesture.local.position );
      mLocalDisplacement.Set( frameGesture.local.displacement );
      mLocalVelocity.Set( frameGesture.local.velocity );
    }

    if( mProfiling )
    {
      mProfiling->mAveragedData.push_back( PanGestureProfiling::Position( frameGesture.time, frameGesture.screen.position,
          frameGesture.screen.displacement, frameGesture.screen.velocity, frameGesture.state ) );
    }
  }

  mLastGesture = frameGesture;
  mLastUnmodifiedGesture = unmodifiedGesture;

  mInGesture &= ~frameInfo.justFinished;
  if( mProfiling && frameInfo.justFinished )
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
  mPredictionAmountOverridden = true;
}

void PanGesture::SetMaximumPredictionAmount(unsigned int amount)
{
  mMaxPredictionAmount = amount;
}

void PanGesture::SetMinimumPredictionAmount(unsigned int amount)
{
  mMinPredictionAmount = amount;
}

void PanGesture::SetPredictionAmountAdjustment(unsigned int amount)
{
  mPredictionAmountAdjustment = amount;
}

void PanGesture::SetSmoothingMode(SmoothingMode mode)
{
  mSmoothingMode = mode;
}

void PanGesture::SetSmoothingAmount(float amount)
{
  mSmoothingAmount = amount;
  mSmoothingAmountOverridden = true;
}

void PanGesture::SetUseActualTimes( bool value )
{
  mUseActualTimes = value;
}

void PanGesture::SetInterpolationTimeRange( int value )
{
  mInterpolationTimeRange = value;
}

void PanGesture::SetScalarOnlyPredictionEnabled( bool value )
{
  mScalarOnlyPredictionEnabled = value;
}

void PanGesture::SetTwoPointPredictionEnabled( bool value )
{
  mTwoPointPredictionEnabled = value;
}

void PanGesture::SetTwoPointInterpolatePastTime( int value )
{
  mTwoPointPastInterpolateTime = value;
}

void PanGesture::SetTwoPointVelocityBias( float value )
{
  mTwoPointVelocityBias = value;
}

void PanGesture::SetTwoPointAccelerationBias( float value )
{
  mTwoPointAccelerationBias = value;
}

void PanGesture::SetMultitapSmoothingRange( int value )
{
  mMultiTapSmoothingRange = value;
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
  mNotAtTarget( false ),
  mInGesture( false ),
  mPredictionAmountOverridden( false ),
  mSmoothingAmountOverridden( false ),
  mProfiling( NULL ),

  // Set environment variable defaults:
  mPredictionMode( DEFAULT_PREDICTION_MODE ),
  mPredictionAmount( DEFAULT_PREDICTION_AMOUNT[0] ),
  mCurrentPredictionAmount( DEFAULT_PREDICTION_AMOUNT[0] ),
  mMaxPredictionAmount( DEFAULT_MAX_PREDICTION_AMOUNT ),
  mMinPredictionAmount( DEFAULT_MIN_PREDICTION_AMOUNT ),
  mPredictionAmountAdjustment( DEFAULT_PREDICTION_AMOUNT_ADJUSTMENT ),
  mSmoothingMode( DEFAULT_SMOOTHING_MODE ),
  mSmoothingAmount( DEFAULT_SMOOTHING_AMOUNT[0] ),
  mUseActualTimes( DEFAULT_USE_ACTUAL_TIMES ),
  mInterpolationTimeRange( DEFAULT_INTERPOLATION_TIME_RANGE ),
  mScalarOnlyPredictionEnabled( DEFAULT_SCALAR_ONLY_PREDICTION_ENABLED ),
  mTwoPointPredictionEnabled( DEFAULT_TWO_POINT_PREDICTION_ENABLED ),
  mTwoPointPastInterpolateTime( DEFAULT_TWO_POINT_PAST_INTERPOLATE_TIME ),
  mTwoPointVelocityBias( DEFAULT_TWO_POINT_VELOCITY_BIAS ),
  mTwoPointAccelerationBias( DEFAULT_TWO_POINT_ACCELERATION_BIAS ),
  mMultiTapSmoothingRange( DEFAULT_MULTITAP_SMOOTHING_RANGE )
{

}

// Prediction mode 2 related code and functions follow:

unsigned int PanGesture::ReadFrameEvents()
{
  unsigned int eventsThisFrame;
  // Copy the events into a linear buffer while holding the mutex.
  // This is so the lock is not held while any processing is done.
  Dali::Mutex::ScopedLock lock( mMutex );
  for( eventsThisFrame = 0; mReadPosition != mWritePosition; ++eventsThisFrame )
  {
    mReadGestures[ eventsThisFrame ] = mGestures[ mReadPosition ];
    ++mReadPosition;
    mReadPosition %= PAN_GESTURE_HISTORY;
  }
  return eventsThisFrame;
}

// TODO: eventsThisFrame parameter can be removed if we use a smarter container.
bool PanGesture::InputRateConversion( PanInfo& rateConvertedGesture, unsigned int eventsThisFrame,
    unsigned int currentFrameTime, unsigned int lastFrameTime, bool& justStarted, bool& justFinished )
{
  // TODO: Lots of variables on the stack. Needs optimizing.
  PanInfo readGesture;
  PanInfo firstReadGesture;
  unsigned int eventsKeptThisFrame = 0;

  for( unsigned int readPosition = 0; readPosition < eventsThisFrame; ++readPosition )
  {
    // Copy the gesture first
    readGesture = mReadGestures[ readPosition ];

    if( mProfiling )
    {
      mProfiling->mRawData.push_back( PanGestureProfiling::Position( readGesture.time, readGesture.screen.position,
          readGesture.screen.displacement, readGesture.screen.velocity, readGesture.state ) );
    }

    if( readGesture.state == Gesture::Started )
    {
      // Clear pan data.
      mPanHistory.clear();
      mPredictionHistory.clear();
      mLastAcceleration.local = Vector2::ZERO;
      mLastAcceleration.screen = Vector2::ZERO;
      mLastInterpolatedAcceleration.local = Vector2::ZERO;
      mLastInterpolatedAcceleration.screen = Vector2::ZERO;
      mLastInitialAcceleration.local = Vector2::ZERO;
      mLastInitialAcceleration.screen = Vector2::ZERO;
      PanInfo startInfo;
      mLastGesture = startInfo;
      mLastSecondInterpolatedPoint = startInfo;
      mLastPredictedPoint = startInfo;
      mLastFrameReadGesture = startInfo;
      rateConvertedGesture = startInfo;
      firstReadGesture = readGesture;
      eventsKeptThisFrame = 0;
      mNotAtTarget = false;
      justFinished = false;
      justStarted = true;
      mInGesture = true;

      if( !mPredictionAmountOverridden )
      {
        // If the prediction amount has not been modified, default to the correct amount for this algorithm.
        mPredictionAmount = DEFAULT_PREDICTION_AMOUNT[1];
      }
      mCurrentPredictionAmount = mPredictionAmount;
    }
    else
    {
      justFinished |= ( readGesture.state == Gesture::Finished || readGesture.state == Gesture::Cancelled );
    }

    rateConvertedGesture.screen.position += readGesture.screen.position;
    rateConvertedGesture.local.position += readGesture.local.position;
    rateConvertedGesture.screen.velocity += readGesture.screen.velocity;
    rateConvertedGesture.local.velocity += readGesture.local.velocity;
    rateConvertedGesture.screen.displacement += readGesture.screen.displacement;
    rateConvertedGesture.local.displacement += readGesture.local.displacement;

    ++eventsKeptThisFrame;
  }

  bool storeGesture = false;
  if( eventsKeptThisFrame > 0 )
  {
    // Some events were read this frame.
    if( eventsKeptThisFrame > 1 )
    {
      float eventDivisor( eventsKeptThisFrame );
      rateConvertedGesture.screen.position /= eventDivisor;
      rateConvertedGesture.local.position /= eventDivisor;
      rateConvertedGesture.screen.velocity /= eventDivisor;
      rateConvertedGesture.local.velocity /= eventDivisor;
      rateConvertedGesture.screen.displacement /= eventDivisor;
      rateConvertedGesture.local.displacement /= eventDivisor;

      mTargetGesture = readGesture;
      mNotAtTarget = true;
    }
    else
    {
      mNotAtTarget = false;
    }

    rateConvertedGesture.time = currentFrameTime;
    storeGesture = true;
  }
  else
  {
    // We did not get any event this frame.
    // If we just started (or aren't in a gesture), exit.
    if( !mInGesture || justStarted )
    {
      // We cannot guess what the event could be as we have no other events to base the guess from.
      return false;
    }

    // As we are currently in a gesture, we can estimate an event.
    readGesture = mLastFrameReadGesture;
    readGesture.time = currentFrameTime;

    // Take the last event, halve the acceleration, and use that.
    const float accelerationDegrade = 2.0f;
    Vector2 degradedAccelerationLocal( mLastAcceleration.local /= accelerationDegrade );
    Vector2 degradedAccelerationScreen( mLastAcceleration.screen /= accelerationDegrade );

    float outputTimeGranularity( GetDivisibleTimeDifference( currentFrameTime, lastFrameTime, 1.0f, OUTPUT_TIME_DIFFERENCE ) );

    readGesture.local.velocity = degradedAccelerationLocal * outputTimeGranularity;
    readGesture.local.displacement = readGesture.local.velocity * outputTimeGranularity;
    readGesture.local.position = mLastFrameReadGesture.local.position + readGesture.local.displacement;
    readGesture.screen.velocity = degradedAccelerationScreen * outputTimeGranularity;
    readGesture.screen.displacement = readGesture.screen.velocity * outputTimeGranularity;
    readGesture.screen.position = mLastFrameReadGesture.screen.position + readGesture.screen.displacement;

    rateConvertedGesture = readGesture;
    eventsKeptThisFrame = 1;
    storeGesture = true;
  }

  if( eventsKeptThisFrame > 0 )
  {
    // Store last read gesture.
    readGesture.time = currentFrameTime;
    mLastFrameReadGesture = readGesture;

    if( eventsKeptThisFrame > 2 )
    {
      DALI_LOG_WARNING( "Got events this frame:%d (more than 2 will compromise result)\n", eventsKeptThisFrame );
    }
  }

  if( storeGesture )
  {
    // Store final converted result.
    mPanHistory.push_back( rateConvertedGesture );
  }
  return true;
}

bool PanGesture::InterpolatePoint( PanInfoHistory& history, unsigned int currentTime, unsigned int targetTime, unsigned int range,
      PanInfo& outPoint, RelativeVectors& acceleration, int outputTimeGranularity, bool eraseUnused )
{
  unsigned int discardInputBufferTime = targetTime + range;
  unsigned int maxHistoryTime = targetTime - range;
  unsigned int tapsUsed = 0;
  outPoint.time = targetTime;
  float divisor = 0.0f;
  float accelerationDivisor = 0.0f;
  PanInfoHistoryIter historyBegin = history.begin();
  PanInfoHistoryIter lastIt = history.end();
  bool pointGenerated = false;
  bool havePreviousPoint = false;
  RelativeVectors newAcceleration;

  // Iterate through point history to perform interpolation.
  for( PanInfoHistoryIter it = historyBegin; it != history.end(); )
  {
    unsigned int gestureTime = it->time;

    if( gestureTime < maxHistoryTime )
    {
      // Too far in the past, discard.
      // Clean history as we go (if requested).
      if( eraseUnused )
      {
        it = history.erase( it );
      }
      else
      {
        ++it;
        continue;
      }
    }
    else
    {
      if( gestureTime > discardInputBufferTime )
      {
        // Too far in the future, discard (and finish).
        break;
      }

      float timeDelta( static_cast<float>( abs( int( targetTime - gestureTime ) ) ) );
      // Handle low time deltas.
      if( timeDelta < 1.0f )
      {
        timeDelta = 1.0f;
      }

      outPoint.local.position += it->local.position / timeDelta;
      outPoint.screen.position += it->screen.position / timeDelta;
      outPoint.local.velocity += it->local.velocity / timeDelta;
      outPoint.screen.velocity += it->screen.velocity / timeDelta;
      outPoint.local.displacement += it->local.displacement / timeDelta;
      outPoint.screen.displacement += it->screen.displacement / timeDelta;

      divisor += 1.0f / timeDelta;

      // Acceleration requires a previous point.
      if( havePreviousPoint )
      {
        // Time delta of input.
        float timeDifference( GetDivisibleTimeDifference( it->time, lastIt->time, 1.0f, OUTPUT_TIME_DIFFERENCE ) );

        newAcceleration.local += ( ( it->local.velocity - lastIt->local.velocity ) / timeDifference ) / timeDelta;
        newAcceleration.screen += ( ( it->screen.velocity - lastIt->screen.velocity ) / timeDifference ) / timeDelta;

        accelerationDivisor += 1.0f / timeDelta;
      }
      else
      {
        havePreviousPoint = true;
      }

      tapsUsed++;
      lastIt = it;
      ++it;
    }
  }

  // Divide results by their respective divisors.
  if( tapsUsed > 0 )
  {
    if( divisor > 0.0f )
    {
      outPoint.local.position /= divisor;
      outPoint.screen.position /= divisor;
      outPoint.local.velocity /= divisor;
      outPoint.screen.velocity /= divisor;
      outPoint.local.displacement /= divisor;
      outPoint.screen.displacement /= divisor;
    }

    if( tapsUsed > 1 )
    {
      if( accelerationDivisor > 0.0f )
      {
        newAcceleration.local /= accelerationDivisor;
        newAcceleration.screen /= accelerationDivisor;
      }

      float accelerationSmoothing( ACCELERATION_SMOOTHING );
      newAcceleration.local = ( acceleration.local * accelerationSmoothing ) + ( newAcceleration.local * ( 1.0f - accelerationSmoothing ) );
      newAcceleration.screen = ( acceleration.screen * accelerationSmoothing ) + ( newAcceleration.screen * ( 1.0f - accelerationSmoothing ) );
    }
    else
    {
      // If we just started, last velocity was 0. So difference of zero to current velocity over time gives acceleration of the first point.
      newAcceleration.local = outPoint.local.velocity / outputTimeGranularity;
      newAcceleration.screen = outPoint.screen.velocity / outputTimeGranularity;
    }
    pointGenerated = true;
  }

  acceleration.local = newAcceleration.local;
  acceleration.screen = newAcceleration.screen;
  return pointGenerated;
}

float PanGesture::GetDivisibleTimeDifference( int timeA, int timeB, float minimumDelta, float overrideDifference )
{
  float timeDifference( overrideDifference );
  if( mUseActualTimes )
  {
    timeDifference = static_cast<float>( abs( timeA - timeB ) );
    if( timeDifference < minimumDelta )
    {
      timeDifference = minimumDelta;
    }
  }
  return timeDifference;
}

void PanGesture::LimitAccelerationChange( RelativeVectors& currentAcceleration, RelativeVectors& lastAcceleration, float changeLimit )
{
  // We don't use the float parameter version of clamp here, as that will create the capping vectors twice in total.
  Vector2 capMinimum( -changeLimit, -changeLimit );
  Vector2 capMaximum( changeLimit, changeLimit );
  Vector2 accelerationDeltaLocal( currentAcceleration.local - lastAcceleration.local );
  Vector2 accelerationDeltaScreen( currentAcceleration.screen - lastAcceleration.screen );
  accelerationDeltaLocal.Clamp( capMinimum, capMaximum );
  accelerationDeltaScreen.Clamp( capMinimum, capMaximum );
  currentAcceleration.local = lastAcceleration.local + accelerationDeltaLocal;
  currentAcceleration.screen = lastAcceleration.screen + accelerationDeltaScreen;
}

void PanGesture::PredictionMode2( PanInfo& startPoint, RelativeVectors& accelerationToUse,
    PanInfo& predictedPoint, unsigned int currentFrameTime, unsigned int previousFrameTime, bool noPreviousData )
{
  // Do the prediction (based on mode).
  if( mScalarOnlyPredictionEnabled )
  {
    // We are doing scalar based prediction.
    // This divisor is to help tuning by giving the scalar only result
    // a similar prediction amount to the integrated result.
    float scalarVelocityMultiplier = static_cast<float>( mCurrentPredictionAmount ) / 1.364f;
    predictedPoint.local.position = startPoint.local.position + ( startPoint.local.velocity * scalarVelocityMultiplier );
    predictedPoint.screen.position = startPoint.screen.position + ( startPoint.screen.velocity * scalarVelocityMultiplier );
  }
  else
  {
    // We are doing integration based prediction.
    float predictionDelta( mCurrentPredictionAmount );

    predictedPoint.local.position = startPoint.local.position + ( startPoint.local.velocity * predictionDelta ) +
        ( accelerationToUse.local * ( predictionDelta * predictionDelta * 0.5f ) );
    predictedPoint.screen.position = startPoint.screen.position + ( startPoint.screen.velocity * predictionDelta ) +
        ( accelerationToUse.screen * ( predictionDelta * predictionDelta * 0.5f ) );
  }

  // Calculate remaining gesture data from the result.
  float timeDifference( GetDivisibleTimeDifference( currentFrameTime, previousFrameTime, 1.0f, OUTPUT_TIME_DIFFERENCE ) );
  if( noPreviousData )
  {
    predictedPoint.local.displacement = predictedPoint.local.position - startPoint.local.position;
    predictedPoint.screen.displacement = predictedPoint.screen.position - startPoint.screen.position;
  }
  else
  {
    predictedPoint.local.displacement = predictedPoint.local.position - mLastPredictedPoint.local.position;
    predictedPoint.screen.displacement = predictedPoint.screen.position - mLastPredictedPoint.screen.position;
  }
  predictedPoint.local.velocity = predictedPoint.local.displacement / timeDifference;
  predictedPoint.screen.velocity = predictedPoint.screen.displacement / timeDifference;

  // TODO: Experimental - not used at run time. Left in code for reference only.
  if( TEST_TUNE_ENABLE_OVERSHOOT_PROTECTION )
  {
    // Overshoot protection
    if( !noPreviousData )
    {
      if( ( mLastPredictedPoint.local.velocity.x > Math::MACHINE_EPSILON_0 && predictedPoint.local.velocity.x < Math::MACHINE_EPSILON_0 )
        || ( mLastPredictedPoint.local.velocity.x < Math::MACHINE_EPSILON_0 && predictedPoint.local.velocity.x > Math::MACHINE_EPSILON_0 ) )
      {
        predictedPoint.local.position.x = mLastPredictedPoint.local.position.x;
        predictedPoint.screen.position.x = mLastPredictedPoint.screen.position.x;
        mPredictionHistory.clear();
      }
      if( ( mLastPredictedPoint.local.velocity.y > Math::MACHINE_EPSILON_0 && predictedPoint.local.velocity.y < Math::MACHINE_EPSILON_0 )
        || ( mLastPredictedPoint.local.velocity.y < Math::MACHINE_EPSILON_0 && predictedPoint.local.velocity.y > Math::MACHINE_EPSILON_0 ) )
      {
        predictedPoint.local.position.y = mLastPredictedPoint.local.position.y;
        predictedPoint.screen.position.y = mLastPredictedPoint.screen.position.y;
        mPredictionHistory.clear();
      }
    }
  }

  predictedPoint.time = currentFrameTime;
  mLastPredictedPoint = predictedPoint;
}

// TODO: This needs a better name! It is called this instead of prediction mode 2 because:
// 1) It is the entire workflow, not just prediction.
// 2) To make it less confusing as there is a function that does prediction alone called PerformPredictionMode2.
// Ultimately we need to combine the old and new code modularly so there is one code path that can optionally run different functions based on configuration.
// At the moment, the differences between the inputs & outputs of these different functions prevent that, but this can be resolved.
bool PanGesture::NewAlgorithm( unsigned int lastVSyncTime, unsigned int nextVSyncTime )
{
  if( !mInGesture )
  {
    // clear current pan history
    mPanHistory.clear();
    mPredictionHistory.clear();
  }

  /*#########################################################################################
    #### Read in all gestures received this frame first (holding a lock for a short time)
    #########################################################################################*/

  unsigned int eventsThisFrame = ReadFrameEvents();

  /*#########################################################################################
    #### Perform input rate-conversion on all gestures received this frame.
    #### This also populates the pan history.
    #########################################################################################*/

  bool justStarted = false;
  bool justFinished = false;
  PanInfo rateConvertedGesture;
  if( !InputRateConversion( rateConvertedGesture, eventsThisFrame, nextVSyncTime, lastVSyncTime, justStarted, justFinished ) )
  {
    // There's nothing we can do with the input, exit.
    return false;
  }

  /*#########################################################################################
    #### If we are in gesture, Get first interpolated point with: target time = current time
    #########################################################################################*/

  bool performUpdate = false;
  RelativeVectors currentAcceleration;
  currentAcceleration.local = mLastInitialAcceleration.local;
  currentAcceleration.screen = mLastInitialAcceleration.screen;

  if( mInGesture || justStarted )
  {
    // Get first interpolated point.
    // TODO: Erase time should be maximum of both interpolated point ranges in past.
    PanInfo targetPoint;
    float outputTimeGranularity( GetDivisibleTimeDifference( nextVSyncTime, lastVSyncTime, 1.0f, OUTPUT_TIME_DIFFERENCE ) );
    bool pointGenerated = InterpolatePoint( mPanHistory, nextVSyncTime, nextVSyncTime, mInterpolationTimeRange,
        targetPoint, currentAcceleration, outputTimeGranularity, true );
    if( pointGenerated )
    {
      mLastInitialAcceleration.local = currentAcceleration.local;
      mLastInitialAcceleration.screen = currentAcceleration.screen;
      performUpdate = true;
    }
    else
    {
      targetPoint = rateConvertedGesture;
      currentAcceleration.local = mLastInitialAcceleration.local;
      currentAcceleration.screen = mLastInitialAcceleration.screen;
      // TODO: Potentially do something to substitute lack of generated point (and perform update).
    }

    /*#########################################################################################
      #### Limit the change of acceleration of the first interpolated point since last time
      #########################################################################################*/

    if( !justStarted )
    {
      LimitAccelerationChange( currentAcceleration, mLastAcceleration, ACCELERATION_CAP );
    }
    mLastAcceleration.local = currentAcceleration.local;
    mLastAcceleration.screen = currentAcceleration.screen;

    /*#########################################################################################
      #### Get second interpolated point, and blend the resultant velocity and acceleration (optional)
      #########################################################################################*/

    PanInfo outPoint;
    RelativeVectors interpolatedAcceleration;
    if( mTwoPointPredictionEnabled )
    {
      // Get second interpolated point with target time = current time - past interpolate time.
      unsigned int pastInterpolateTime = nextVSyncTime - mTwoPointPastInterpolateTime;
      PanInfo outPoint;
      RelativeVectors interpolatedAcceleration;
      interpolatedAcceleration.local = mLastInterpolatedAcceleration.local;
      interpolatedAcceleration.screen = mLastInterpolatedAcceleration.screen;
      if( !InterpolatePoint( mPanHistory, nextVSyncTime, pastInterpolateTime, mTwoPointPastInterpolateTime,
          outPoint, interpolatedAcceleration, outputTimeGranularity, false ) )
      {
        if( justStarted )
        {
          outPoint = targetPoint;
        }
        else
        {
          outPoint = mLastSecondInterpolatedPoint;
        }
      }
      mLastInterpolatedAcceleration.local = interpolatedAcceleration.local;
      mLastInterpolatedAcceleration.screen = interpolatedAcceleration.screen;
      mLastSecondInterpolatedPoint = outPoint;

      // Combine the first interpolated point and the second interpolated point.
      // by mixing them with the configured amount. This is done for acceleration and velocity.
      // It could be optionally done for position too, but this typically is worse as it means we have to predict further ahead.
      float currentVelocityMultiplier( 1.0f - mTwoPointVelocityBias );
      float lastVelocityMultiplier( mTwoPointVelocityBias );
      targetPoint.local.velocity = ( outPoint.local.velocity * lastVelocityMultiplier ) + ( targetPoint.local.velocity * currentVelocityMultiplier );
      targetPoint.screen.velocity = ( outPoint.screen.velocity * lastVelocityMultiplier ) + ( targetPoint.screen.velocity * currentVelocityMultiplier );
      float currentAccelerationMultiplier( 1.0f - mTwoPointAccelerationBias );
      float lastAccelerationMultiplier( mTwoPointAccelerationBias );
      currentAcceleration.local = ( interpolatedAcceleration.local * lastAccelerationMultiplier ) + ( currentAcceleration.local * currentAccelerationMultiplier );
      currentAcceleration.screen = ( interpolatedAcceleration.screen * lastAccelerationMultiplier ) + ( currentAcceleration.screen * currentAccelerationMultiplier );
    }

    /*#########################################################################################
      #### Perform prediction
      #########################################################################################*/

    PanInfo predictedPoint;
    PredictionMode2( targetPoint, currentAcceleration, predictedPoint, nextVSyncTime, lastVSyncTime, justStarted );
    targetPoint = predictedPoint;

    /*#########################################################################################
      #### Smoothing
      #########################################################################################*/

    // If we are using multi-tap smoothing, keep a history of predicted results.
    if( mSmoothingMode == SMOOTHING_MULTI_TAP )
    {
      mPredictionHistory.push_back( targetPoint );
    }

    if( !justStarted )
    {
      float outputTimeGranularity( GetDivisibleTimeDifference( nextVSyncTime, lastVSyncTime, 1.0f, OUTPUT_TIME_DIFFERENCE ) );
      if( mSmoothingMode == SMOOTHING_MULTI_TAP )
      {
        // Perform Multi-tap Smoothing.
        RelativeVectors blank;
        InterpolatePoint( mPredictionHistory, nextVSyncTime, nextVSyncTime, mMultiTapSmoothingRange,
            targetPoint, blank, outputTimeGranularity, true );
      }
      else
      {
        // Perform Single-tap Smoothing.
        if( !mSmoothingAmountOverridden )
        {
          // If the smoothing amount has not been modified, default to the correct amount for this algorithm.
          mSmoothingAmount = DEFAULT_SMOOTHING_AMOUNT[1];
        }
        BlendPoints( targetPoint, mLastGesture, mSmoothingAmount );
      }

      /*#########################################################################################
        #### Finalize other point data (from position)
        #########################################################################################*/

      targetPoint.local.displacement = targetPoint.local.position - mLastGesture.local.position;
      targetPoint.local.velocity = targetPoint.local.displacement / outputTimeGranularity;
      targetPoint.screen.displacement = targetPoint.screen.position - mLastGesture.screen.position;
      targetPoint.screen.velocity = targetPoint.screen.displacement / outputTimeGranularity;
    }

    /*#########################################################################################
      #### Send out the new point, by setting the properties
      #### (Constraints will automatically react to this)
      #########################################################################################*/

    if( performUpdate )
    {
      mPanning.Set( mInGesture & !justFinished );
      mScreenPosition.Set( targetPoint.screen.position );
      mScreenDisplacement.Set( targetPoint.screen.displacement );
      mScreenVelocity.Set( targetPoint.screen.velocity );
      mLocalPosition.Set( targetPoint.local.position );
      mLocalDisplacement.Set( targetPoint.local.displacement );
      mLocalVelocity.Set( targetPoint.local.velocity );

      mLastGesture = targetPoint;

      if( mProfiling )
      {
        mProfiling->mAveragedData.push_back( PanGestureProfiling::Position( targetPoint.time, targetPoint.screen.position,
            targetPoint.screen.displacement, targetPoint.screen.velocity, targetPoint.state ) );
      }
    }
  }

  mInGesture &= ~justFinished;

  return performUpdate;
}


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
