#ifndef __DALI_INTERNAL_SCENE_GRAPH_PAN_GESTURE_H__
#define __DALI_INTERNAL_SCENE_GRAPH_PAN_GESTURE_H__

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
   * Called by the update manager so that we can update the value of our properties.
   * @param[in]  lastVSyncTime  The last VSync time (in milliseconds).
   * @param[in]  nextVSyncTime  The estimated time of the next VSync (in milliseconds).
   */
  virtual void UpdateProperties( unsigned int lastVSyncTime, unsigned int nextVSyncTime );

  /**
   * Retrieves a reference to the screen position property.
   * @return The screen position property.
   */
  const GesturePropertyVector2& GetScreenPositionProperty() const;

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
  GesturePropertyVector2 mScreenPosition;     ///< screen-position
  GesturePropertyVector2 mScreenDisplacement; ///< screen-displacement
  GesturePropertyVector2 mLocalPosition;      ///< local-position
  GesturePropertyVector2 mLocalDisplacement;  ///< local-displacement

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
      state( Gesture::Clear )
    {
    }

    /**
     * Copy constructor
     */
    PanInfo( const PanInfo& rhs )
    : time( rhs.time ),
      state( rhs.state ),
      local( rhs.local ),
      screen( rhs.screen )
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
  };

  PanInfo mGestures[4];         ///< Circular buffer storing the 4 most recent gestures.
  unsigned int mWritePosition;  ///< The next PanInfo buffer to write to. (starts at 0)
  unsigned int mReadPosition;   ///< The next PanInfo buffer to read. (starts at 0)

  PanInfo mLatestGesture;       ///< The latest gesture. (this update frame)
  PanInfo mPreviousGesture;     ///< The previous gesture. (one update frame ago)
  bool mInGesture;              ///< True if the gesture is currently being handled i.e. between Started <-> Finished/Cancelled

  PanGestureProfiling* mProfiling; ///< NULL unless pan-gesture profiling information is required.
};

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_SCENE_GRAPH_PAN_GESTURE_H__
