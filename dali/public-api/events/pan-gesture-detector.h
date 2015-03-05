#ifndef __DALI_PAN_GESTURE_DETECTOR_H__
#define __DALI_PAN_GESTURE_DETECTOR_H__

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
#include <dali/public-api/events/gesture-detector.h>
#include <dali/public-api/object/property-index-ranges.h>
#include <dali/public-api/signals/dali-signal.h>

namespace Dali
{

struct Radian;

namespace Internal DALI_INTERNAL
{
class PanGestureDetector;
}

struct PanGesture;

/**
 * @brief This class looks for panning (or dragging) gestures.
 *
 * The user will be pressing one or more fingers on an actor while they pan it.
 *
 * The application programmer can use this gesture detector as follows:
 * @code
 * PanGestureDetector detector = PanGestureDetector::New();
 * detector.Attach(myActor);
 * detector.DetectedSignal().Connect(this, &MyApplication::OnPan);
 *
 * // Detect pan gesture for single and double touch.
 * detector.SetMaximumTouchesRequired(2);
 * @endcode
 *
 * @see PanGesture
 *
 * Signals
 * | %Signal Name | Method                |
 * |--------------|-----------------------|
 * | pan-detected | @ref DetectedSignal() |
 */
class DALI_IMPORT_API PanGestureDetector : public GestureDetector
{
public:

  /**
   * @brief An enumeration of properties belonging to the PanGestureDetector class.
   */
  struct Property
  {
    enum
    {
      SCREEN_POSITION = DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX, ///< name "screen-position",     type Vector2
      SCREEN_DISPLACEMENT,                                             ///< name "screen-displacement", type Vector2
      SCREEN_VELOCITY,                                                 ///< name "screen-velocity",     type Vector2
      LOCAL_POSITION,                                                  ///< name "local-position",      type Vector2
      LOCAL_DISPLACEMENT,                                              ///< name "local-displacement",  type Vector2
      LOCAL_VELOCITY,                                                  ///< name "local-velocity",      type Vector2
      PANNING,                                                         ///< name "panning",             type Boolean
    };
  };

  // Typedefs
  typedef Signal< void ( Actor, const PanGesture& ) > DetectedSignalType; ///< Pan gesture detected signal type

  // Directional Pan
  typedef std::pair< Radian, Radian > AngleThresholdPair; ///< Range of angles for a direction
  typedef std::vector< AngleThresholdPair > AngleContainer; ///< Group of angular thresholds for all directions

  static const Radian DIRECTION_LEFT;       ///< For a left pan (-PI Radians).
  static const Radian DIRECTION_RIGHT;      ///< For a right pan (0 Radians).
  static const Radian DIRECTION_UP;         ///< For an up pan (-0.5 * PI Radians).
  static const Radian DIRECTION_DOWN;       ///< For a down pan (0.5 * PI Radians).
  static const Radian DIRECTION_HORIZONTAL; ///< For a left and right pan (PI Radians). Useful for AddDirection().
  static const Radian DIRECTION_VERTICAL;   ///< For an up and down pan (-0.5 * PI Radians). Useful for AddDirection().

  static const Radian DEFAULT_THRESHOLD;    ///< The default threshold is PI * 0.25 radians (or 45 degrees).

public: // Creation & Destruction

  /**
   * @brief Create an uninitialized PanGestureDetector; this can be initialized with PanGestureDetector::New().
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  PanGestureDetector();

  /**
   * @brief Create an initialized PanGestureDetector.
   *
   * @return A handle to a newly allocated Dali resource.
   */
  static PanGestureDetector New();

  /**
   * @brief Downcast an Object handle to PanGestureDetector handle.
   *
   * If handle points to a PanGestureDetector object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a PanGestureDetector object or an uninitialized handle
   */
  static PanGestureDetector DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~PanGestureDetector();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @param [in] handle A reference to the copied handle
   */
  PanGestureDetector(const PanGestureDetector& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  PanGestureDetector& operator=(const PanGestureDetector& rhs);

public: // Setters

  /**
   * @brief This is the minimum number of touches required for the pan gesture to be detected.
   *
   * @param[in]  minimum  Minimum touches required.
   * @pre The gesture detector has been initialized.
   * @note The default minimum is '1'.
   */
  void SetMinimumTouchesRequired(unsigned int minimum);

  /**
   * @brief This is the maximum number of touches required for the pan gesture to be detected.
   *
   * @param[in]  maximum  Maximum touches required.
   * @pre The gesture detector has been initialized.
   * @note The default maximum is '1'.
   */
  void SetMaximumTouchesRequired(unsigned int maximum);

public: // Getters

  /**
   * @brief Retrieves the minimum number of touches required for the pan gesture to be detected.
   *
   * @return The minimum touches required.
   * @pre The gesture detector has been initialized.
   */
  unsigned int GetMinimumTouchesRequired() const;

  /**
   * @brief Retrieves the maximum number of touches required for the pan gesture to be detected.
   *
   * @return The maximum touches required.
   * @pre The gesture detector has been initialized.
   */
  unsigned int GetMaximumTouchesRequired() const;

public: // Directional Panning

  /**
   * @brief The pan gesture is only emitted if the pan occurs in the direction specified by this method with a +/- threshold allowance.
   *
   * The angle is from -180 -> 0 -> 180 degrees (or -M_PI -> 0 -> M_PI in radians) i.e:
   *
   * @code
   *           -90.0f ( -0.5f * PI )
   *                     |
   *                     |
   * 180.0f ( PI ) ------------- 0.0f ( 0.0f )
   *                     |
   *                     |
   *            90.0f ( 0.5f * PI )
   * @endcode
   *
   * If an angle of 0.0 degrees is specified and the threshold is 45 degrees then the acceptable
   * direction range is from -45 to 45 degrees.
   *
   * @param[in]  angle      The angle that pan should be allowed.
   * @param[in]  threshold  The threshold around that angle.
   *
   * @note The angle added using this API is only checked when the gesture first starts, after that,
   *       this detector will emit the gesture regardless of what angle the pan is moving.
   * @note The user can add as many angles as they require.
   * @note If an angle outside the range above is given, then it is wrapped within the range, i.e.
   *       190 degrees becomes -170 degrees and 370 degrees becomes 10 degrees.
   * @note As long as you specify the type, you can also pass in a Dali::Degree to this method.
   * @note If no threshold is provided, then the default threshold (PI * 0.25) is used.
   * @note If the threshold is greater than PI, then PI will be used as the threshold.
   *
   * @pre The gesture detector has been initialized.
   */
  void AddAngle( Radian angle, Radian threshold = DEFAULT_THRESHOLD );

  /**
   * @brief A helper method for adding bi-directional angles where the pan should take place.
   *
   * In other words, if 0 is requested, then PI will also be added so that we have both left and
   * right scrolling.
   *
   * @param[in]  direction  The direction of panning required.
   * @param[in]  threshold  The threshold.
   *
   * @note If a direction outside the range above is given, then it is wrapped within the range, i.e.
   *       190 degrees becomes -170 degrees and 370 degrees becomes 10 degrees.
   * @note If no threshold is provided, then the default threshold (PI * 0.25) is used.
   * @note If the threshold is greater than PI, then PI will be used as the threshold.
   * @note As long as you specify the type, you can also pass in a Dali::Degree to this method.
   *
   * @pre The gesture detector has been initialized.
   *
   * @see AddAngle
   */
  void AddDirection( Radian direction, Radian threshold = DEFAULT_THRESHOLD );

  /**
   * @brief Returns the container of all the angles this pan gesture detector emits a signal.
   *
   * @return a const reference to the container of all the angles.
   * @pre The gesture detector has been initialized.
   */
  const AngleContainer& GetAngles() const;

  /**
   * @brief Clears any directional angles that are used by the gesture detector.
   *
   * After this, the pan gesture
   * will be emitted for a pan in ANY direction.
   * @pre The gesture detector has been initialized.
   */
  void ClearAngles();

  /**
   * @brief Removes the angle specified from the container.
   *
   * @param[in]  angle  The angle to remove.
   * @pre The gesture detector has been initialized.
   * @note This will only remove the first instance of the angle found from the container.
   * @note If an angle outside the range in AddAngle() is given, then the value is wrapped within
   *       the range and that is removed.
   */
  void RemoveAngle( Radian angle );

  /**
   * @brief Removes the two angles that make up the direction from the container.
   *
   * @param[in]  direction  The direction to remove.
   * @pre The gesture detector has been initialized.
   * @note If a direction outside the range in AddAngle() is given, then the value is wrapped within
   *       the range and that is removed.
   */
  void RemoveDirection( Radian direction );

public: // Signals

  /**
   * @brief This signal is emitted when the pan gesture is detected on the attached actor.
   *
   * A callback of the following type may be connected:
   * @code
   *   void YourCallbackName( Actor actor, const PanGesture& gesture );
   * @endcode
   * @pre The gesture detector has been initialized.
   * @return The signal to connect to.
   */
  DetectedSignalType& DetectedSignal();

public: // Pan Properties Setters

  /**
   * @brief Allows setting of the pan properties that are returned in constraints.
   *
   * @param[in]  pan  The pan gesture to set.
   *@note If a normal pan is taking place, then any value set is ignored.
   */
  static void SetPanGestureProperties( const PanGesture& pan );

public: // Not intended for Application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @param [in]  internal  A pointer to a newly allocated Dali resource.
   */
  explicit DALI_INTERNAL PanGestureDetector(Internal::PanGestureDetector* internal);

};

} // namespace Dali

#endif // __DALI_PAN_GESTURE_DETECTOR_H__
