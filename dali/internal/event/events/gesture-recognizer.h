#ifndef DALI_INTERNAL_GESTURE_RECOGNIZER_H
#define DALI_INTERNAL_GESTURE_RECOGNIZER_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/events/gesture.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/object/ref-object.h>
#include <dali/devel-api/events/gesture-devel.h>
#include <dali/internal/event/events/gesture-event.h>

namespace Dali
{

namespace Integration
{
struct TouchEvent;
}

namespace Internal
{
class GestureRequest;
class Scene;

template< typename T>
class RecognizerObserver
{
public:
  virtual void Process( Scene& scene, const T& event ) = 0;

  virtual ~RecognizerObserver(){};
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
   * Called when Core updates the gesture's detection requirements.
   * @param[in]  request  The updated detection requirements.
   */
  virtual void Update(const GestureRequest& request) = 0;

  /**
   * Returns the type of gesture detector.
   * @return Type of gesture detector.
   */
  DevelGesture::Type GetType() const { return mType; }

  /**
   * Called when we get a touch event.
   * @param[in]  scene  The scene the touch event has occurred on
   * @param[in]  event  The latest touch event
   */
  void SendEvent( Scene& scene, const Integration::TouchEvent& event )
  {
    mScene = &scene;
    SendEvent( event );
  }

protected:

  /**
   * Protected Constructor. Should only be able to create derived class objects.
   * @param[in]  screenSize    The size of the screen.
   * @param[in]  detectorType  The type of gesture detector.
   */
  GestureRecognizer( Vector2 screenSize, DevelGesture::Type detectorType )
  : mScreenSize( screenSize ),
    mType( detectorType ),
    mScene( nullptr )
  {
  }

  /**
   * copydoc GestureRecognizer( Vector2, DevelGesture::Type )
   */
  GestureRecognizer( Vector2 screenSize, Gesture::Type detectorType )
  : GestureRecognizer( screenSize, static_cast< DevelGesture::Type >( detectorType ) )
  {
  }

  /**
   * Protected Constructor. Should only be able to create derived class objects.
   *
   * Use this constructor with the screen size is not used in the dereived class.
   * @param[in]  detectorType  The type of gesture detector.
   */
  GestureRecognizer( DevelGesture::Type detectorType )
  : GestureRecognizer( Vector2::ZERO, detectorType )
  {
  }

  /**
   * Virtual destructor.
   */
  virtual ~GestureRecognizer() {}

protected:
  Vector2 mScreenSize;
  DevelGesture::Type mType;
  Scene* mScene;
};

typedef IntrusivePtr<GestureRecognizer> GestureRecognizerPtr;

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_GESTURE_RECOGNIZER_H
