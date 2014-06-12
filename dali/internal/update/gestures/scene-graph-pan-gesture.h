#ifndef __DALI_INTERNAL_SCENE_GRAPH_PAN_GESTURE_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PAN_GESTURE_H__

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
    PREDICTION_1
  };

  enum SmoothingMode
  {
    SMOOTHING_NONE,           // no smoothing
    SMOOTHING_LAST_VALUE,     // smooth between last value and latest value
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
        velocity = rhs.velocity;
        displacement = rhs.displacement;
        position = rhs.position;

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
      time = rhs.time;
      state = rhs.state;
      local = rhs.local;
      screen = rhs.screen;

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
  static const unsigned int PAN_GESTURE_HISTORY = 10u;

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
  void PredictiveAlgorithm1(int eventsThisFrame, PanInfo& gestureOut, PanInfoHistory& panHistory, unsigned int lastVSyncTime, unsigned int nextVSyncTime);

  /**
   * Uses last two gestures
   *
   * @param[in]  justStarted Whether the pan has just started.
   * @param[out] gestureOut Output gesture using average values from last two gestures
   * @param[in]  lastVSyncTime The time to set on gestureOut.
   */
  void SmoothingAlgorithm1(bool justStarted, PanInfo& gestureOut, unsigned int lastVSyncTime);

  /**
   * Future smoothing method, implementation not complete
   */
  void SmoothingAlgorithm2(bool justStarted, PanInfo& gestureOut, unsigned int lastVSyncTime);

  /**
   * Called by the update manager so that we can update the value of our properties.
   * @param[in]  nextRenderTime  The estimated time of the next render (in milliseconds).
   * @return true, if properties were updated.
   */
  virtual bool UpdateProperties( unsigned int lastRenderTime, unsigned int nextRenderTime );

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

  /**
   * Called to provide pan-gesture profiling information.
   */
  void EnableProfiling();

private:

  /**
   * Protected constructor.
   */
  PanGesture();

  // Undefined
  PanGesture(const PanGesture&);

  // Undefined
  PanGesture& operator=(const PanGesture&);

  // PropertyOwner
  virtual void ResetDefaultProperties( BufferIndex updateBufferIndex );

private:

  // Properties
  GesturePropertyBool    mPanning;            ///< panning flag
  GesturePropertyVector2 mScreenPosition;     ///< screen-position
  GesturePropertyVector2 mScreenDisplacement; ///< screen-displacement
  GesturePropertyVector2 mScreenVelocity;     ///< screen-velocity
  GesturePropertyVector2 mLocalPosition;      ///< local-position
  GesturePropertyVector2 mLocalDisplacement;  ///< local-displacement
  GesturePropertyVector2 mLocalVelocity;      ///< local-velocity

  PanInfo mGestures[PAN_GESTURE_HISTORY];         ///< Circular buffer storing the 4 most recent gestures.
  PanInfoHistory mPanHistory;
  PanInfoHistory mPredictionHistory;
  unsigned int mWritePosition;  ///< The next PanInfo buffer to write to. (starts at 0)
  unsigned int mReadPosition;   ///< The next PanInfo buffer to read. (starts at 0)

  PanInfo mEventGesture;        ///< Result of all pan events received this frame
  PanInfo mLastEventGesture;    ///< The last frame's event gesture.
  PanInfo mLastGesture;         ///< The latest gesture. (this update frame)
  PanInfo mLatestGesture;       ///< The latest gesture. (this update frame)
  bool mInGesture;              ///< True if the gesture is currently being handled i.e. between Started <-> Finished/Cancelled

  PredictionMode mPredictionMode;  ///< The pan gesture prediction mode
  unsigned int mPredictionAmount;  ///< how far into future to predict in milliseconds
  SmoothingMode mSmoothingMode;    ///< The pan gesture prediction mode
  float         mSmoothingAmount;  ///< How much smoothing to apply [0.0f,1.0f]
  PanGestureProfiling* mProfiling; ///< NULL unless pan-gesture profiling information is required.
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_PAN_GESTURE_H__
