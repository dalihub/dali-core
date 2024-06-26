#ifndef DALI_INTERNAL_GESTURE_RECOGNIZER_H
#define DALI_INTERNAL_GESTURE_RECOGNIZER_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/integration-api/events/touch-event-integ.h>
#include <dali/internal/event/events/gesture-event.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/events/gesture.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/ref-object.h>

namespace Dali
{
namespace Integration
{
struct TouchEvent;
}

namespace Internal
{
struct GestureRequest;
class Scene;

template<typename T>
class RecognizerObserver
{
public:
  virtual void Process(Scene& scene, const T& event) = 0;

  virtual ~RecognizerObserver() = default;
  ;
};

/**
 * Abstract Base class for all adaptor gesture detectors.
 *
 * @note this may be replaced by gesture events sent directly from X.
 */
class GestureRecognizer : public RefObject
{
public:
  /**
   * Called when it gets a touch event.  The gesture recognizer should
   * evaluate this event along with previously received events to determine
   * whether the gesture they require has taken place.
   * @param[in]  event  The latest touch event.
   */
  virtual void SendEvent(const Integration::TouchEvent& event) = 0;

  /**
   * @brief This is canceling the ongoing gesture recognition process.
   * If the gesture recognition was in progress, it will be stopped immediately after calling this function.
   */
  virtual void CancelEvent() = 0;

  /**
   * Called when Core updates the gesture's detection requirements.
   * @param[in]  request  The updated detection requirements.
   */
  virtual void Update(const GestureRequest& request) = 0;

  /**
   * Returns the type of gesture detector.
   * @return Type of gesture detector.
   */
  GestureType::Value GetType() const
  {
    return mType;
  }

  /**
   * Called when we get a touch event.
   * @param[in]  scene  The scene the touch event has occurred on
   * @param[in]  event  The latest touch event
   */
  void SendEvent(Scene& scene, const Integration::TouchEvent& event)
  {
    mScene = &scene;
    if(event.GetPointCount() > 0)
    {
      const Integration::Point& point       = event.points[0];
      MouseButton::Type         mouseButton = point.GetMouseButton();
      if(mouseButton != MouseButton::INVALID)
      {
        Device::Class::Type type = point.GetDeviceClass();
        if(type == Device::Class::Type::MOUSE)
        {
          mSourceType = GestureSourceType::MOUSE;
        }
        else if(type == Device::Class::Type::TOUCH)
        {
          mSourceType = GestureSourceType::TOUCH;
        }
        switch(mouseButton)
        {
          case MouseButton::PRIMARY:
          {
            mSourceData = GestureSourceData::MOUSE_PRIMARY;
            break;
          }
          case MouseButton::SECONDARY:
          {
            mSourceData = GestureSourceData::MOUSE_SECONDARY;
            break;
          }
          case MouseButton::TERTIARY:
          {
            mSourceData = GestureSourceData::MOUSE_TERTIARY;
            break;
          }
          default:
          {
            mSourceData = GestureSourceData::INVALID;
            break;
          }
        }
      }
    }
    SendEvent(event);
  }

protected:
  /**
   * Protected Constructor. Should only be able to create derived class objects.
   * @param[in]  screenSize    The size of the screen.
   * @param[in]  detectorType  The type of gesture detector.
   */
  GestureRecognizer(Vector2 screenSize, GestureType::Value detectorType)
  : mScreenSize(screenSize),
    mType(detectorType),
    mScene(nullptr),
    mSourceType(GestureSourceType::INVALID),
    mSourceData(GestureSourceData::INVALID)
  {
  }

  /**
   * Protected Constructor. Should only be able to create derived class objects.
   *
   * Use this constructor with the screen size is not used in the dereived class.
   * @param[in]  detectorType  The type of gesture detector.
   */
  GestureRecognizer(GestureType::Value detectorType)
  : GestureRecognizer(Vector2::ZERO, detectorType)
  {
  }

  /**
   * Virtual destructor.
   */
  ~GestureRecognizer() override = default;

protected:
  Vector2            mScreenSize;
  GestureType::Value mType;
  Scene*             mScene;
  GestureSourceType  mSourceType; /// < Gesture input source type.
  GestureSourceData  mSourceData; /// < Gesture input source data.
};

using GestureRecognizerPtr = IntrusivePtr<GestureRecognizer>;

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_GESTURE_RECOGNIZER_H
