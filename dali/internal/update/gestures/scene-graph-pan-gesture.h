#ifndef __DALI_INTERNAL_SCENE_GRAPH_PAN_GESTURE_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PAN_GESTURE_H__

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

// INTERNAL INCLUDES
#include <dali/devel-api/threading/mutex.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/gestures/gesture-properties.h>

namespace Dali
{

struct PanGesture;

namespace Internal
{

struct PanGestureProfiling;

namespace SceneGraph
{

/**
 * The latest pan gesture information is stored in this scene object.
 */
class PanGesture : public PropertyOwner
{
public:

  enum PredictionMode
  {
    PREDICTION_NONE = 0,
    PREDICTION_1,
    PREDICTION_2,
  };

  enum SmoothingMode
  {
    SMOOTHING_NONE,           // No smoothing.
    SMOOTHING_LAST_VALUE,     // Smooth between last value and latest value.
    SMOOTHING_MULTI_TAP,      // Uses multitap smoothing, only available with Prediction mode 2.
  };

  static const PredictionMode DEFAULT_PREDICTION_MODE;
  static const int NUM_PREDICTION_MODES;

  static const SmoothingMode DEFAULT_SMOOTHING_MODE;
  static const int NUM_SMOOTHING_MODES;

  // Latest Pan Information

  /**
   * Only stores the information we actually require from Dali::PanGesture
   */
  struct PanInfo
  {
    /**
     * Stores the velocity, displacement and position.
     */
    struct Info
    {
      Info()
      {
      }

      /**
       * Copy constructor
       */
      Info( const Info& rhs )
      : velocity( rhs.velocity ),
        displacement( rhs.displacement ),
        position( rhs.position )
      {
      }

      /**
       * Assignment operator
       */
      Info& operator=( const Info& rhs )
      {
        if( this != &rhs )
        {
          velocity = rhs.velocity;
          displacement = rhs.displacement;
          position = rhs.position;
        }

        return *this;
      }

      // Data

      Vector2 velocity;
      Vector2 displacement;
      Vector2 position;
    };

    /**
     * Constructor
     */
    PanInfo()
    : time( 0u ),
      state( Gesture::Clear ),
      read( true )
    {
    }

    /**
     * Copy constructor
     */
    PanInfo( const PanInfo& rhs )
    : time( rhs.time ),
      state( rhs.state ),
      local( rhs.local ),
      screen( rhs.screen ),
      read( true )
    {
    }

    /**
     * Assignment operator
     */
    PanInfo& operator=( const PanInfo& rhs )
    {
      if( this != &rhs )
      {
        time = rhs.time;
        state = rhs.state;
        local = rhs.local;
        screen = rhs.screen;
        read = rhs.read;
      }

      return *this;
    }

    /**
     * Assignment operator
     * @param[in] gesture A Dali::Gesture
     */
    PanInfo& operator=( const Dali::PanGesture& rhs )
    {
      time = rhs.time;
      state = rhs.state;

      local.velocity = rhs.velocity;
      local.displacement = rhs.displacement;
      local.position = rhs.position;

      screen.velocity = rhs.screenVelocity;
      screen.displacement = rhs.screenDisplacement;
      screen.position = rhs.screenPosition;

      return *this;
    }

    // Data
    unsigned int time;
    Gesture::State state;
    Info local;
    Info screen;
    volatile bool read;
  };

  typedef std::vector<PanInfo> PanInfoHistory;
  typedef PanInfoHistory::iterator PanInfoHistoryIter;
  typedef PanInfoHistory::const_iterator PanInfoHistoryConstIter;

private:
  static const unsigned int PAN_GESTURE_HISTORY = 30u;

public:

  /**
   * Create a new PanGesture
   */
  static PanGesture* New();

  /**
   * Virtual destructor
   */
  virtual ~PanGesture();

  /**
   * Adds a PanGesture to the internal circular-buffer waiting to be handled by UpdateProperties.
   * @param[in]  gesture  The latest pan gesture.
   */
  void AddGesture( const Dali::PanGesture& gesture );

  /**
   * @brief Removes pan events from the history that are older than maxAge, leaving at least minEvents
   *
   * @param[in] panHistory The pan event history container
   * @param[in] currentTime The current frame time
   * @param[in] maxAge Maximum age of an event before removing (in millis)
   * @param[in] minEvents The minimum number of events to leave in history, oldest events are removed before newest
   */
  void RemoveOldHistory(PanInfoHistory& panHistory, unsigned int currentTime, unsigned int maxAge, unsigned int minEvents);

  /**
   * Uses elapsed time and time stamps
   */
  void PredictionMode1(int eventsThisFrame, PanInfo& gestureOut, PanInfoHistory& panHistory, unsigned int lastVSyncTime, unsigned int nextVSyncTime);

  /**
   * Blends two points together.
   * The blend value ranges are:
   * 0.0f = use 100% of current value.
   * 1.0f = use half-way average of current and last value.
   *
   * @param[in,out] gesture     Pass in current gesture, outputs result of blend.
   * @param[in]     lastGesture Pass in gesture to blend between.
   */
  void BlendPoints( PanInfo& gesture, PanInfo& lastGesture, float blendValue );

  /**
   * Called by the update manager so that we can update the value of our properties.
   * @param[in]  nextRenderTime  The estimated time of the next render (in milliseconds).
   * @return true, if properties were updated.
   */
  bool UpdateProperties( unsigned int lastRenderTime, unsigned int nextRenderTime );

  /**
   * Retrieves a reference to the panning flag property.
   * @return The panning flag property.
   */
  const GesturePropertyBool& GetPanningProperty() const;

  /**
   * Retrieves a reference to the screen position property.
   * @return The screen position property.
   */
  const GesturePropertyVector2& GetScreenPositionProperty() const;

  /**
   * Retrieves a reference to the screen velocity property.
   * @return The screen velocity property.
   */
  const GesturePropertyVector2& GetScreenVelocityProperty() const;

  /**
   * Retrieves a reference to the screen displacement property.
   * @return The screen displacement property.
   */
  const GesturePropertyVector2& GetScreenDisplacementProperty() const;

  /**
   * Retrieves a reference to the local position property.
   * @return The local position property.
   */
  const GesturePropertyVector2& GetLocalPositionProperty() const;

  /**
   * Retrieves a reference to the local displacement property.
   * @return The local displacement property.
   */
  const GesturePropertyVector2& GetLocalDisplacementProperty() const;

  /**
   * Retrieves a reference to the local velocity property.
   * @return The local velocity property.
   */
  const GesturePropertyVector2& GetLocalVelocityProperty() const;

  /**
   * @brief Sets the prediction mode of the pan gesture
   *
   * @param[in] mode The prediction mode
   */
  void SetPredictionMode(PredictionMode mode);

  /**
   * @brief Sets the prediction amount of the pan gesture
   *
   * @param[in] amount The prediction amount in milliseconds
   */
  void SetPredictionAmount(unsigned int amount);

  /**
   * @brief Sets the upper bound of the prediction amount for clamping
   *
   * @param[in] amount The prediction amount in milliseconds
   */
  void SetMaximumPredictionAmount(unsigned int amount);

  /**
   * @brief Sets the lower bound of the prediction amount for clamping
   *
   * @param[in] amount The prediction amount in milliseconds
   */
  void SetMinimumPredictionAmount(unsigned int amount);

  /**
   * @brief Sets the amount of prediction interpolation to adjust when the pan velocity is changed
   *
   * @param[in] amount The prediction amount in milliseconds
   */
  void SetPredictionAmountAdjustment(unsigned int amount);

  /**
   * @brief Sets the prediction mode of the pan gesture
   *
   * @param[in] mode The prediction mode
   */
  void SetSmoothingMode(SmoothingMode mode);

  /**
   * @brief Sets the amount of smoothing to apply for the current smoothing mode
   *
   * @param[in] amount The amount of smoothing [0.0f,1.0f]
   */
  void SetSmoothingAmount(float amount);

  /*
   * @brief Sets whether to use actual times of the real gesture and frames or not.
   *
   * @param[in] value True = use actual times, False = use perfect values
   */
  void SetUseActualTimes( bool value );

  /**
   * @brief Sets the interpolation time range (ms) of past points to use (with weights) when interpolating.
   *
   * @param[in] value Time range in ms
   */
  void SetInterpolationTimeRange( int value );

  /**
   * @brief Sets whether to use scalar only prediction, which when enabled, ignores acceleration.
   *
   * @param[in] value True = use scalar prediction only
   */
  void SetScalarOnlyPredictionEnabled( bool value );

  /**
   * @brief Sets whether to use two point prediction. This combines two interpolated points to get more steady acceleration and velocity values.
   *
   * @param[in] value True = use two point prediction
   */
  void SetTwoPointPredictionEnabled( bool value );

  /**
   * @brief Sets the time in the past to interpolate the second point when using two point interpolation.
   *
   * @param[in] value Time in past in ms
   */
  void SetTwoPointInterpolatePastTime( int value );

  /**
   * @brief Sets the two point velocity bias. This is the ratio of first and second points to use for velocity.
   *
   * @param[in] value 0.0f = 100% first point. 1.0f = 100% of second point.
   */
  void SetTwoPointVelocityBias( float value );

  /**
   * @brief Sets the two point acceleration bias. This is the ratio of first and second points to use for acceleration.
   *
   * @param[in] value 0.0f = 100% first point. 1.0f = 100% of second point.
   */
  void SetTwoPointAccelerationBias( float value );

  /**
   * @brief Sets the range of time (ms) of points in the history to perform multitap smoothing with (if enabled).
   *
   * @param[in] value Time in past in ms
   */
  void SetMultitapSmoothingRange( int value );

  /**
   * Called to provide pan-gesture profiling information.
   */
  void EnableProfiling();

  /**
   * Reset default properties, custom ones not supported due to this being the only object in scene side
   * @param updateBufferIndex index to use
   */
  void ResetDefaultProperties( BufferIndex updateBufferIndex );

private:

  /**
   * Protected constructor.
   */
  PanGesture();

  // Undefined
  PanGesture(const PanGesture&);

private:

  // Struct to keep pairs of local and screen data together.
  // TODO: This can encapsulate some functionality also.
  typedef struct
  {
    Vector2 local;
    Vector2 screen;
  } RelativeVectors;

  /**
   * Houses new code to process input events and generate an output point.
   *
   * @param[in]  lastVSyncTime The time of the last render (in milliseconds)
   * @param[in]  nextVSyncTime The estimated time of the next render (in milliseconds)
   */
  bool NewAlgorithm( unsigned int lastVSyncTime, unsigned int nextVSyncTime );

  /**
   * Gets the (absolute) time difference between two times.
   * This is limited by minimumDelta, so it can be safe to use as a divisor.
   * This function is wrapped so that the behviour can be overridden to return a "perfect" time difference (overrideDifference).
   *
   * @param[in]  timeA The first time to calculate from
   * @param[in]  timeB The second time to calculate from
   * @param[in]  minimumDelta The smallest amount the difference can become
   * @param[in]  overrideDifference The time difference to return if using perfect times
   */
  inline float GetDivisibleTimeDifference( int timeA, int timeB, float minimumDelta, float overrideDifference );

  /**
   * This limits the change currentAcceleration can have over lastAcceleration by the specified changeLimit value.
   *
   * @param[in]  currentAcceleration The acceleration to modify
   * @param[in]  lastAcceleration The acceleration to limit against
   * @param[in]  changeLimit The maximum change (in either direction)
   */
  void LimitAccelerationChange( RelativeVectors& currentAcceleration, RelativeVectors& lastAcceleration, float changeLimit );

  /**
   * Reads all events received this frame into a linear buffer.
   * A lock is held while this is done.
   */
  unsigned int ReadFrameEvents();

  /**
   * Converts between input rate and internal rate (typically 60Hz internally).
   * Also writes to the pan history container.
   * TODO: Does not need to return the gesture if it is in the history also, but currently it's used.
   * (if rate conversion does not generate a point there are points still in history, but this can been done with a bool property).
   *
   * @param[out] rateConvertedGesture Result gesture for this frame is writen here.
   * @param[in]  eventsThisFrame Number of events to convert
   * @param[in]  currentFrameTime Time of the frame we will render to
   * @param[in]  lastFrameTime Time of the last rendered frame
   * @param[out] justStarted Set to true if we are now starting a new gesture
   * @param[out] justFinished Set to true if we are now finishing a gesture
   */
  bool InputRateConversion( PanInfo& rateConvertedGesture, unsigned int eventsThisFrame,
      unsigned int currentFrameTime, unsigned int lastFrameTime, bool& justStarted, bool& justFinished );

  /**
   * Generates an interpolated point at the specified point in time.
   *
   * @param[in]  history of points to use
   * @param[in]  currentTime Time of the frame we will render to
   * @param[in]  targetTime Time of the point to generate
   * @param[in]  range Range of time (each side of target time) to use points from
   * @param[out] outPoint Generated point
   * @param[out] acceleration Generated acceleration
   * @param[in]  outputTimeGranularity Time difference between output point (typically 60Hz)
   * @param[in]  eraseUnused Set to true to clean up any history not used by the function
   */
  bool InterpolatePoint( PanInfoHistory& history, unsigned int currentTime, unsigned int targetTime, unsigned int range,
      PanInfo& outPoint, RelativeVectors& acceleration, int outputTimeGranularity, bool eraseUnused );

  /**
   * Predicts a point in the future, based on the supplied point and acceleration.
   * Other user configuration settings are considered.
   *
   * @param[in] startPoint Starting point to use. Position and velocity are taken from here.
   * @param[in] accelerationToUse The acceleration to use.
   * @param[out] predictedPoint Generated predicted point
   * @param[in]  currentFrameTime Time of the frame we will render to
   * @param[in]  previousFrameTime Time of the last rendered frame
   * @param[in]  noPreviousData Set to true if we are just starting a gesture
   */
  void PredictionMode2( PanInfo& startPoint, RelativeVectors& accelerationToUse,
      PanInfo& predictedPoint, unsigned int currentFrameTime, unsigned int previousFrameTime, bool noPreviousData );

private:

  // Undefined
  PanGesture& operator=(const PanGesture&);

  // Defines information to be gathered by the gesture reading code.
  struct FrameGestureInfo
  {
    PanGesture::PanInfo frameGesture;
    float acceleration;
    unsigned int eventsThisFrame;
    bool justStarted;
    bool justFinished;

    FrameGestureInfo()
    : acceleration( 0.0f ),
      eventsThisFrame( 0 ),
      justStarted( false ),
      justFinished( false )
    {
    }
  };

  /**
   * Reads gestures from input, builds history.
   * @param[out] info Written to with information about gestures read this frame.
   * @param[in] currentTimestamp The time of this frame.
   */
  bool ReadGestures( FrameGestureInfo& info, unsigned int currentTimestamp );

  /**
   * Reads gestures from input and resamples data, builds history.
   * @param[out] info Written to with information about gestures read this frame.
   * @param[in] currentTimestamp The time of this frame.
   */
  bool ReadAndResampleGestures( FrameGestureInfo& info, unsigned int currentTimestamp );

private:

  // Properties
  GesturePropertyBool    mPanning;            ///< panning flag
  GesturePropertyVector2 mScreenPosition;     ///< screenPosition
  GesturePropertyVector2 mScreenDisplacement; ///< screenDisplacement
  GesturePropertyVector2 mScreenVelocity;     ///< screenVelocity
  GesturePropertyVector2 mLocalPosition;      ///< localPosition
  GesturePropertyVector2 mLocalDisplacement;  ///< localDisplacement
  GesturePropertyVector2 mLocalVelocity;      ///< localVelocity

  PanInfoHistory mPanHistory;
  PanInfoHistory mPredictionHistory;
  PanInfo mGestures[PAN_GESTURE_HISTORY];     ///< Circular buffer storing the 4 most recent gestures.
  PanInfo mReadGestures[PAN_GESTURE_HISTORY]; ///< Linear buffer storing the most recent gestures (to reduce read lock time).
  PanInfo mLastGesture;                       ///< The last gesture. (last update frame).
  PanInfo mTargetGesture;                     ///< The most recent input gesture, if the current used gesture does not match.
  PanInfo mLastUnmodifiedGesture;             ///< The last gesture before any processing was done on it.
  PanInfo mLastSecondInterpolatedPoint;       ///< Stores the last second interpolated point we generated.
  PanInfo mLastFrameReadGesture;              ///< Stores the last gesture read.
  PanInfo mLastPredictedPoint;                ///< Stores the last predicted point we generated.
  RelativeVectors mLastAcceleration;          ///< Stores the acceleration value from the acceleration limiting last frame.
  RelativeVectors mLastInterpolatedAcceleration;  ///< Stores the second interpolated point acceleration value from the last frame.
  RelativeVectors mLastInitialAcceleration;   ///< Stores the initial acceleration value from the last frame.

  volatile unsigned int mWritePosition;       ///< The next PanInfo buffer to write to. (starts at 0).
  unsigned int mReadPosition;                 ///< The next PanInfo buffer to read. (starts at 0).
  bool mNotAtTarget;                          ///< Keeps track of if the last gesture used was the most recent received.
  bool mInGesture;                            ///< True if the gesture is currently being handled i.e. between Started <-> Finished/Cancelled.
  bool mPredictionAmountOverridden;
  bool mSmoothingAmountOverridden;

  PanGestureProfiling* mProfiling;            ///< NULL unless pan-gesture profiling information is required.
  Dali::Mutex mMutex;                         ///< Mutex to lock access.

  // Environment variables:

  PredictionMode mPredictionMode;             ///< The pan gesture prediction mode
  unsigned int mPredictionAmount;             ///< how far into future to predict in milliseconds
  unsigned int mCurrentPredictionAmount;      ///< the current prediction amount used by the prediction algorithm
  unsigned int mMaxPredictionAmount;          ///< the maximum prediction amount used by the prediction algorithm
  unsigned int mMinPredictionAmount;          ///< the minimum prediction amount used by the prediction algorithm
  unsigned int mPredictionAmountAdjustment;   ///< the prediction amount to adjust in milliseconds when pan velocity changes
  SmoothingMode mSmoothingMode;               ///< The pan gesture prediction mode
  float         mSmoothingAmount;             ///< How much smoothing to apply [0.0f,1.0f]
  bool mUseActualTimes;                       ///< Disable to optionally override actual times if they make results worse.
  int mInterpolationTimeRange;                ///< Time into past history (ms) to use points to interpolate the first point.
  bool mScalarOnlyPredictionEnabled;          ///< If enabled, prediction is done using velocity alone (no integration or acceleration).
  bool mTwoPointPredictionEnabled;            ///< If enabled, a second interpolated point is predicted and combined with the first to get more stable values.
  int mTwoPointPastInterpolateTime;           ///< The target time in the past to generate the second interpolated point.
  float mTwoPointVelocityBias;                ///< The ratio of first and second interpolated points to use for velocity. 0.0f = 100% of first point. 1.0f = 100% of second point.
  float mTwoPointAccelerationBias;            ///< The ratio of first and second interpolated points to use for acceleration. 0.0f = 100% of first point. 1.0f = 100% of second point.
  int mMultiTapSmoothingRange;                ///< The range in time (ms) of points in the history to smooth the final output against.
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_PAN_GESTURE_H__
