#ifndef __DALI_PAN_GESTURE_DETECTOR_H__
#define __DALI_PAN_GESTURE_DETECTOR_H__

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
 * @addtogroup dali_core_events
 * @{
 */

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
 * @SINCE_1_0.0
 * @see PanGesture
 *
 * Signals
 * | %Signal Name | Method                |
 * |--------------|-----------------------|
 * | panDetected  | @ref DetectedSignal() |
 */
class DALI_IMPORT_API PanGestureDetector : public GestureDetector
{
public:

  /**
   * @brief An enumeration of properties belonging to the PanGestureDetector class.
   * @SINCE_1_0.0
   */
  struct Property
  {
    enum
    {
      SCREEN_POSITION = DEFAULT_GESTURE_DETECTOR_PROPERTY_START_INDEX, ///< name "screenPosition",      type Vector2 @SINCE_1_0.0
      SCREEN_DISPLACEMENT,                                             ///< name "screenDisplacement",  type Vector2 @SINCE_1_0.0
      SCREEN_VELOCITY,                                                 ///< name "screenVelocity",      type Vector2 @SINCE_1_0.0
      LOCAL_POSITION,                                                  ///< name "localPosition",       type Vector2 @SINCE_1_0.0
      LOCAL_DISPLACEMENT,                                              ///< name "localDisplacement",   type Vector2 @SINCE_1_0.0
      LOCAL_VELOCITY,                                                  ///< name "localVelocity",       type Vector2 @SINCE_1_0.0
      PANNING,                                                         ///< name "panning",             type bool @SINCE_1_0.0
    };
  };

  // Typedefs
  typedef Signal< void ( Actor, const PanGesture& ) > DetectedSignalType; ///< Pan gesture detected signal type @SINCE_1_0.0

  // Directional Pan
  typedef std::pair< Radian, Radian > AngleThresholdPair; ///< Range of angles for a direction @SINCE_1_0.0

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
   * @SINCE_1_0.0
   */
  PanGestureDetector();

  /**
   * @brief Create an initialized PanGestureDetector.
   *
   * @SINCE_1_0.0
   * @return A handle to a newly allocated Dali resource.
   */
  static PanGestureDetector New();

  /**
   * @brief Downcast an Object handle to PanGestureDetector handle.
   *
   * If handle points to a PanGestureDetector object the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @SINCE_1_0.0
   * @param[in] handle to An object
   * @return handle to a PanGestureDetector object or an uninitialized handle
   */
  static PanGestureDetector DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @SINCE_1_0.0
   */
  ~PanGestureDetector();

  /**
   * @brief This copy constructor is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] handle A reference to the copied handle
   */
  PanGestureDetector(const PanGestureDetector& handle);

  /**
   * @brief This assignment operator is required for (smart) pointer semantics.
   *
   * @SINCE_1_0.0
   * @param [in] rhs  A reference to the copied handle
   * @return A reference to this
   */
  PanGestureDetector& operator=(const PanGestureDetector& rhs);

public: // Setters

  /**
   * @brief This is the minimum number of touches required for the pan gesture to be detected.
   *
   * @SINCE_1_0.0
   * @param[in]  minimum  Minimum touches required.
   * @pre The gesture detector has been initialized.
   * @note The default minimum is '1'.
   */
  void SetMinimumTouchesRequired(unsigned int minimum);

  /**
   * @brief This is the maximum number of touches required for the pan gesture to be detected.
   *
   * @SINCE_1_0.0
   * @param[in]  maximum  Maximum touches required.
   * @pre The gesture detector has been initialized.
   * @note The default maximum is '1'.
   */
  void SetMaximumTouchesRequired(unsigned int maximum);

public: // Getters

  /**
   * @brief Retrieves the minimum number of touches required for the pan gesture to be detected.
   *
   * @SINCE_1_0.0
   * @return The minimum touches required.
   * @pre The gesture detector has been initialized.
   */
  unsigned int GetMinimumTouchesRequired() const;

  /**
   * @brief Retrieves the maximum number of touches required for the pan gesture to be detected.
   *
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
   * @param[in]  angle      The angle that pan should be allowed.
   * @param[in]  threshold  The threshold around that angle.
   *
   * @pre The gesture detector has been initialized.
   * @note The angle added using this API is only checked when the gesture first starts, after that,
   *       this detector will emit the gesture regardless of what angle the pan is moving.
   * @note The user can add as many angles as they require.
   * @note If an angle outside the range above is given, then it is wrapped within the range, i.e.
   *       190 degrees becomes -170 degrees and 370 degrees becomes 10 degrees.
   * @note As long as you specify the type, you can also pass in a Dali::Degree to this method.
   * @note If no threshold is provided, then the default threshold (PI * 0.25) is used.
   * @note If the threshold is greater than PI, then PI will be used as the threshold.
   *
   */
  void AddAngle( Radian angle, Radian threshold = DEFAULT_THRESHOLD );

  /**
   * @brief A helper method for adding bi-directional angles where the pan should take place.
   *
   * In other words, if 0 is requested, then PI will also be added so that we have both left and
   * right scrolling.
   *
   * @SINCE_1_0.0
   * @param[in]  direction  The direction of panning required.
   * @param[in]  threshold  The threshold.
   *
   * @pre The gesture detector has been initialized.
   *
   * @note If a direction outside the range above is given, then it is wrapped within the range, i.e.
   *       190 degrees becomes -170 degrees and 370 degrees becomes 10 degrees.
   * @note If no threshold is provided, then the default threshold (PI * 0.25) is used.
   * @note If the threshold is greater than PI, then PI will be used as the threshold.
   * @note As long as you specify the type, you can also pass in a Dali::Degree to this method.
   *
   * @see AddAngle
   */
  void AddDirection( Radian direction, Radian threshold = DEFAULT_THRESHOLD );

  /**
   * @brief Returns the count of angles that this pan gesture detector emits a signal.
   *
   * @SINCE_1_0.0
   * @return The count.
   * @pre The gesture detector has been initialized.
   */
  size_t GetAngleCount() const;

  /**
   * @brief Returns the angle by index that this pan gesture detector emits a signal.
   *
   * @SINCE_1_0.0
   * @return an angle threshold pair, or a zero valued angle pair when index is invalid.
   * @pre The gesture detector has been initialized.
   * @pre The index is less than GetAngleCount()
   */
  AngleThresholdPair GetAngle(size_t index) const;

  /**
   * @brief Clears any directional angles that are used by the gesture detector.
   *
   * After this, the pan gesture
   * will be emitted for a pan in ANY direction.
   * @SINCE_1_0.0
   * @pre The gesture detector has been initialized.
   */
  void ClearAngles();

  /**
   * @brief Removes the angle specified from the container.
   *
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
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
   * @SINCE_1_0.0
   * @return The signal to connect to.
   * @pre The gesture detector has been initialized.
   */
  DetectedSignalType& DetectedSignal();

public: // Pan Properties Setters

  /**
   * @brief Allows setting of the pan properties that are returned in constraints.
   *
   * @SINCE_1_0.0
   * @param[in]  pan  The pan gesture to set.
   * @note If a normal pan is taking place, then any value set is ignored.
   */
  static void SetPanGestureProperties( const PanGesture& pan );

public: // Not intended for Application developers

  /**
   * @brief This constructor is used by Dali New() methods.
   *
   * @SINCE_1_0.0
   * @param [in]  internal  A pointer to a newly allocated Dali resource.
   */
  explicit DALI_INTERNAL PanGestureDetector(Internal::PanGestureDetector* internal);

};

/**
 * @}
 */

} // namespace Dali

#endif // __DALI_PAN_GESTURE_DETECTOR_H__
