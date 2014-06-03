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
#include <dali/internal/update/touch/touch-resampler.h>

// INTERNAL INCLUDES
#include <dali/internal/update/touch/history.h>
#include <dali/integration-api/events/touch-data.h>
#include <dali/integration-api/events/touch-event-integ.h>

namespace Dali
{

namespace Internal
{

namespace
{

static const unsigned int MaxPendingTouches(16u); ///< Queue can keep track of a maximum of 16 events per update refresh.

/**
 * Conversion function. To convert from raw touch data (which is either
 * in a Down, Up or Motion state) to TouchPoint states.
 * @param[in] touch The raw touch data state.
 * @return[out] The TouchPoint state.
 */
TouchPoint::State GetStateFromTouch( const Dali::Integration::TouchData& touch )
{
  switch(touch.type)
  {
    case Dali::Integration::TouchData::Down:
    {
      return TouchPoint::Down;
    }
    case Dali::Integration::TouchData::Up:
    {
      return TouchPoint::Up;
    }
    case Dali::Integration::TouchData::Motion:
    {
      return TouchPoint::Motion;
    }
  } // end switch

  DALI_ASSERT_ALWAYS(false); // should never reach here as we handle all possible cases in above switch

  return TouchPoint::Last;
}

}

/**
 * TouchTracker keeps track of a single finger's touch movement from touch
 * down to release. While the caller can then inspect the touch position and
 * incidents that occured at a convenient time i.e. update-time.
 * Typically there is 1 to 2 touches that occur between two updates.
 *
 * The application developer is interested in the touch position at a constant
 * time point relative to the visual update (e.g. Always 5ms before the next
 * render) - while touches rarely occur at the same time due to a difference
 * in frequency amongst other timing issues.
 *
 * The application developer also wants to know if Up, Down or Motion events
 * occured. But multiple motion "Motion 1 & Motion 2" events within a frame are
 * wasteful information so they're consolidated into just "Motion 2".
 * "Motion & Up" events within a frame are also wasteful, they're consolidated
 * into just "Up". However "Down & Motion", or "Down & Up" events both need
 * to be propagated to the user.
 *
 * Theoretically a double tap, given sufficient slow down of the update thread
 * could also be propagated to the user with this system as "Down 1, Up 1, Down 2, Up 2"
 */
class TouchTracker
{

public:

  /**
   * Construct a TouchTracker based on an inital touch.
   * This initial touch should be a Down press.
   * @param[in] data The initial TouchData to populate this with.
   */
  TouchTracker(Dali::Integration::TouchData& data)
  : mActive(true),
    mIndex(data.index),
    mPreviousType(data.type),
    mStartTimestamp(data.timestamp)
  {
    mTouchHistory.SetMaxSize(5);
    mIncidents.push_back(data);

    mTouchHistory.Add(0.0f, Vector2(data.x, data.y));
  }

  /**
   * Call SendTouchData to send this object some touch data.
   * @param[in] data The data to send
   */
  void SendTouchData(Dali::Integration::TouchData& data)
  {
    mTouchHistory.Add(data.timestamp - mStartTimestamp, Vector2(data.x, data.y));

    if(!mIncidents.size() || mPreviousType != data.type)
    {
      mIncidents.push_back(data);
      mPreviousType = data.type;
    }
    else // If there are multiple touches on this frame, and previous is the same, then overwrite previous.
    {
      mIncidents[mIncidents.size()-1] = data;
    }
  }

  /**
   * Query the this object's touch position at a given timestamp
   * @param[in] timestamp The point in time where the touch is wished
   * to be known (using same timestamp scale as that provided in the
   * TouchData)
   * @return The interpolated touch position is returned.
   */
  Vector2 GetTouchPosition(unsigned int timestamp) const
  {
    float time( timestamp - mStartTimestamp );
    Vector2 position = mTouchHistory.Get(time);
    return position;
  }

  bool mActive;
  unsigned int mIndex;
  Dali::Integration::TouchDataContainer mIncidents;   ///< All of the touches from down to up for this
  Dali::Integration::TouchData::TouchType mPreviousType;
  History mTouchHistory;                              ///< Keep track of recent touches
  unsigned int mStartTimestamp;                       ///< Timestamp of the first touch. Touch history has time relative to first touch.
};

typedef std::vector<TouchTracker> TouchTrackerContainer;
typedef TouchTrackerContainer::iterator TouchTrackerIter;
typedef TouchTrackerContainer::const_iterator TouchTrackerConstIter;

/**
 * TouchPointsTracker keeps track of all the touch points,
 * receives touch data and sends to an individual TouchTracker
 * to handle. It also generates TouchEvents based on all
 * the TouchTrackers' event data.
 */
class TouchPointsTracker
{
public:

  /**
   * Call SendTouchData to send this tracker some touch data.
   * @param[in] data The data to send
   */
  void SendTouchData( Dali::Integration::TouchData& data );

  /**
   * Query the number of touch events that can be generated from calling GetNextTouchEvent
   * @return The number of touch events available.
   */
  unsigned int GetTouchEventCount() const;

  /**
   * Populate touchEvent with the next TouchEvent.
   * The touch event position information is interpolated to produce a touch event that
   * reflects the time provided (nextRenderTime)
   *
   * @note Call GetTouchEventCount() prior to calling this to know whether there are any
   * TouchEvents available.
   *
   * @param[in] touchEvent The touch event to be populated with touch event info.
   * @param[in] referenceTimestamp The timestamp required to assist touch smoothing.
   */
  bool GetNextTouchEvent( Dali::Integration::TouchEvent& touchEvent, unsigned int referenceTimestamp );

private:

  TouchTrackerContainer mTouches;                    ///< A container for each individual touch.

};

void TouchPointsTracker::SendTouchData( Dali::Integration::TouchData& data )
{
  // Check if we have a record of this touch index.
  TouchTrackerIter i = mTouches.begin();
  TouchTrackerIter endIter = mTouches.end();
  for(;i != endIter; ++i )
  {
    if(i->mIndex == data.index)
    {
      i->SendTouchData(data);
      return;
    }
  }

  // If no record exists, then create a new record for this touch index.
  mTouches.push_back( TouchTracker(data) );
}

unsigned int TouchPointsTracker::GetTouchEventCount() const
{
  TouchTrackerConstIter i = mTouches.begin();
  TouchTrackerConstIter endIter = mTouches.end();
  size_t maxTouches(0u);
  for(;i != endIter; ++i )
  {
    // check if touch has incident(s), get max number of incidents on any touch.
    maxTouches = std::max(maxTouches, i->mIncidents.size());
  }

  return maxTouches;
}

bool TouchPointsTracker::GetNextTouchEvent( Dali::Integration::TouchEvent& touchEvent, unsigned int referenceTimestamp )
{
  bool hasTouchEvent( GetTouchEventCount() > 0 );

  if( hasTouchEvent )
  {
    TouchTrackerIter i = mTouches.begin();
    TouchTrackerIter endIter = mTouches.end();
    unsigned int maxTimestamp = 0u;

    for(;i != endIter; ++i )
    {
      // check if touch has an incident, if so then take oldest incident off.
      if(i->mIncidents.size())
      {
        const Dali::Integration::TouchData& touchData(*i->mIncidents.begin());

        if(touchData.type == Dali::Integration::TouchData::Motion)
        {
          // For Motion, use a smoothed position.
          Vector2 smoothPosition = i->GetTouchPosition(referenceTimestamp);

          touchEvent.AddPoint( TouchPoint(i->mIndex, TouchPoint::Motion, smoothPosition.x, smoothPosition.y ) );
          maxTimestamp = std::max(maxTimestamp, touchData.timestamp);
        }
        else
        {
          // For Up/Down, use a actual touch position.
          touchEvent.AddPoint( TouchPoint(i->mIndex, GetStateFromTouch(touchData), touchData.x, touchData.y ) );
          maxTimestamp = std::max(maxTimestamp, touchData.timestamp);
        }

        i->mIncidents.erase(i->mIncidents.begin());
        i->mActive = touchData.type != Dali::Integration::TouchData::Up;
      }
      else
      {
        Vector2 smoothPosition = i->GetTouchPosition(referenceTimestamp);

        // Stationary (TODO: final position should gravitate to real last touch position)
        touchEvent.AddPoint( TouchPoint(i->mIndex, TouchPoint::Stationary, smoothPosition.x, smoothPosition.y ) );
      }
    }

    touchEvent.time = maxTimestamp;

    // clean up list. TODO: change to pointers.
    TouchTrackerContainer touchesCleaned;
    for(i = mTouches.begin();i != endIter; ++i )
    {
      if(i->mIncidents.size() || i->mActive)
      {
        touchesCleaned.push_back(*i);
      }
    }
    mTouches = touchesCleaned;
  }

  return hasTouchEvent;
}

TouchResampler* TouchResampler::New()
{
  return new TouchResampler();
}

TouchResampler::~TouchResampler()
{
  delete[] mTouchesQueue;
  delete mTouchPointsTracker;
}

void TouchResampler::SendTouchData( const Dali::Integration::TouchData& touch )
{
  mTouchesQueue[mTouchesWrite] = touch;
  mTouchesWrite = (mTouchesWrite + 1) % MaxPendingTouches;
  // TODO: Should only do this if it has been concluded that the down touch hits an Actor listening for touch.
  // I think needs to send a down touch to Event thread to evaluate hit-test, and if valid, then enable update Required.

  mUpdateRequired = true;
}

void TouchResampler::Update()
{
  while(mTouchesRead != mTouchesWrite)
  {
    mTouchPointsTracker->SendTouchData( mTouchesQueue[mTouchesRead] );

    mTouchesRead = (mTouchesRead + 1) % MaxPendingTouches;
  }
}

bool TouchResampler::GetNextTouchEvent( Dali::Integration::TouchEvent& touchEvent, unsigned int time )
{
  touchEvent.points.clear();
  touchEvent.time = 0u;

  return mTouchPointsTracker->GetNextTouchEvent( touchEvent, time );
}

bool TouchResampler::NeedsUpdate()
{
  bool updateRequired(mUpdateRequired);

  mUpdateRequired = false;

  return updateRequired;
}

TouchResampler::TouchResampler()
: mUpdateRequired(false),
  mTouchesQueue(new Dali::Integration::TouchData[MaxPendingTouches]),
  mTouchesRead(0u),
  mTouchesWrite(0u),
  mTouchPointsTracker(new TouchPointsTracker())
{
}

} // namespace Internal

} // namespace Dali

