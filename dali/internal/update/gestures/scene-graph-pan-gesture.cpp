/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

namespace
{
const int MAX_GESTURE_AGE = 50;                                     ///< maximum age of a gesture before disallowing its use in algorithm TODO: Possibly make this configurable.
const float ACCELERATION_THRESHOLD = 0.1f;                          ///< minimum pan velocity change to trigger dynamic change of prediction amount
const unsigned int DEFAULT_PREDICTION_INTERPOLATION = 5;            ///< how much to interpolate pan position and displacement from last vsync time (in milliseconds)
const unsigned int DEFAULT_MAX_PREDICTION_INTERPOLATION = 32;       ///< the upper bound of the range to clamp the prediction interpolation
const unsigned int DEFAULT_MIN_PREDICTION_INTERPOLATION = 0;        ///< the lower bound of the range to clamp the prediction interpolation
const unsigned int DEFAULT_PREDICTION_INTERPOLATION_ADJUSTMENT = 2; ///< the amount of prediction interpolation to adjust (in milliseconds) each time when pan velocity changes
const float DEFAULT_SMOOTHING_AMOUNT = 0.25f;                       ///< how much to interpolate pan position and displacement from last vsync time
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
    float previousValueWeight = ( static_cast< float >( MAX_GESTURE_AGE ) - (lastVSyncTime - lastTime) ) / static_cast< float >( MAX_GESTURE_AGE );
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
  if( justStarted )
  {
    gestureOut.screen.displacement = Vector2::ZERO;
    gestureOut.screen.velocity = Vector2::ZERO;
    gestureOut.local.displacement = Vector2::ZERO;
    gestureOut.local.velocity = Vector2::ZERO;
  }
  else
  {
    gestureOut.screen.position -= (gestureOut.screen.position - mLastGesture.screen.position) * 0.5f * (1.0f - mSmoothingAmount);
    gestureOut.local.position -= (gestureOut.local.position - mLastGesture.local.position) * 0.5f * (1.0f - mSmoothingAmount);
    // make current displacement relative to previous update-frame now.
    gestureOut.screen.displacement = gestureOut.screen.position - mLastGesture.screen.position;
    gestureOut.local.displacement = gestureOut.local.position - mLastGesture.local.position;
    // calculate velocity relative to previous update-frame
    float timeDiff( gestureOut.time - mLastGesture.time );
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
  mLastUnmodifiedGesture = frameGesture;

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
    switch( mPredictionMode )
    {
      case PREDICTION_NONE:
      {
        break;
      }
      case PREDICTION_1:
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
          mCurrentPredictionAmount = mPredictionAmount; // Reset the prediction amount for each new gesture
        }

        mCurrentPredictionAmount = std::max( mMinPredictionAmount, std::min( mCurrentPredictionAmount, mMaxPredictionAmount ) );

        // Calculate the delta of positions before the prediction
        Vector2 deltaPosition = frameGesture.screen.position - mLastUnmodifiedGesture.screen.position;

        // Make latest gesture equal to current gesture before interpolation
        PredictiveAlgorithm1( frameInfo.eventsThisFrame, frameGesture, mPanHistory, lastVSyncTime, nextVSyncTime );

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
        break;
      }
    }

    // Perform smoothing.
    switch( mSmoothingMode )
    {
      case SMOOTHING_NONE:
      {
        // No smoothing
        break;
      }
      case SMOOTHING_LAST_VALUE:
      {
        SmoothingAlgorithm1( frameInfo.justStarted, frameGesture, lastVSyncTime );
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
: mPanning(),
  mGestures(),
  mWritePosition( 0 ),
  mReadPosition( 0 ),
  mNotAtTarget( false ),
  mInGesture( false ),
  mPredictionMode( DEFAULT_PREDICTION_MODE ),
  mPredictionAmount( DEFAULT_PREDICTION_INTERPOLATION ),
  mCurrentPredictionAmount( DEFAULT_PREDICTION_INTERPOLATION ),
  mMaxPredictionAmount( DEFAULT_MAX_PREDICTION_INTERPOLATION ),
  mMinPredictionAmount( DEFAULT_MIN_PREDICTION_INTERPOLATION ),
  mPredictionAmountAdjustment( DEFAULT_PREDICTION_INTERPOLATION_ADJUSTMENT ),
  mSmoothingMode( DEFAULT_SMOOTHING_MODE ),
  mSmoothingAmount( DEFAULT_SMOOTHING_AMOUNT ),
  mProfiling( NULL )
{
}


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
