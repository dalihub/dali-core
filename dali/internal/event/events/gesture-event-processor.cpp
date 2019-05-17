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

// CLASS HEADER
#include <dali/internal/event/events/gesture-event-processor.h>

#if defined(DEBUG_ENABLED)
#include <sstream>
#endif

// INTERNAL INCLUDES
#include <dali/integration-api/render-controller.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/events/pinch-gesture-detector-impl.h>
#include <dali/internal/update/gestures/scene-graph-pan-gesture.h>
#include <dali/public-api/events/pan-gesture.h>
#include <dali/integration-api/debug.h>


namespace Dali
{

namespace Internal
{
GestureEventProcessor::GestureEventProcessor( SceneGraph::UpdateManager& updateManager, Integration::RenderController& renderController )
: mLongPressGestureProcessor(),
  mPanGestureProcessor( updateManager ),
  mPinchGestureProcessor(),
  mTapGestureProcessor(),
  mRenderController( renderController ),
  envOptionMinimumPanDistance(-1),
  envOptionMinimumPanEvents(-1)
{
}

GestureEventProcessor::~GestureEventProcessor()
{
}

void GestureEventProcessor::ProcessTouchEvent( Scene& scene, const Integration::TouchEvent& event)
{
  mLongPressGestureProcessor.ProcessTouch(scene, event);
  mPanGestureProcessor.ProcessTouch(scene, event);
  mPinchGestureProcessor.ProcessTouch(scene, event);
  mTapGestureProcessor.ProcessTouch(scene, event);
}

void GestureEventProcessor::AddGestureDetector(GestureDetector* gestureDetector, Scene& scene)
{
  switch (gestureDetector->GetType())
  {
    case Gesture::LongPress:
    {
      LongPressGestureDetector* longPress = static_cast<LongPressGestureDetector*>(gestureDetector);
      mLongPressGestureProcessor.AddGestureDetector(longPress, scene);
      break;
    }

    case Gesture::Pan:
    {
      PanGestureDetector* pan = static_cast<PanGestureDetector*>(gestureDetector);
      mPanGestureProcessor.AddGestureDetector(pan, scene, envOptionMinimumPanDistance, envOptionMinimumPanEvents);
      break;
    }

    case Gesture::Pinch:
    {
      PinchGestureDetector* pinch = static_cast<PinchGestureDetector*>(gestureDetector);
      mPinchGestureProcessor.AddGestureDetector(pinch, scene);
      break;
    }

    case Gesture::Tap:
    {
      TapGestureDetector* tap = static_cast<TapGestureDetector*>(gestureDetector);
      mTapGestureProcessor.AddGestureDetector(tap, scene);
      break;
    }
  }
}

void GestureEventProcessor::RemoveGestureDetector(GestureDetector* gestureDetector)
{
  switch (gestureDetector->GetType())
  {
    case Gesture::LongPress:
    {
      LongPressGestureDetector* longPress = static_cast<LongPressGestureDetector*>(gestureDetector);
      mLongPressGestureProcessor.RemoveGestureDetector(longPress);
      break;
    }

    case Gesture::Pan:
    {
      PanGestureDetector* pan = static_cast<PanGestureDetector*>(gestureDetector);
      mPanGestureProcessor.RemoveGestureDetector(pan);
      break;
    }

    case Gesture::Pinch:
    {
      PinchGestureDetector* pinch = static_cast<PinchGestureDetector*>(gestureDetector);
      mPinchGestureProcessor.RemoveGestureDetector(pinch);
      break;
    }

    case Gesture::Tap:
    {
      TapGestureDetector* tap = static_cast<TapGestureDetector*>(gestureDetector);
      mTapGestureProcessor.RemoveGestureDetector(tap);
      break;
    }
  }
}

void GestureEventProcessor::GestureDetectorUpdated(GestureDetector* gestureDetector)
{
  switch (gestureDetector->GetType())
  {
    case Gesture::LongPress:
    {
      LongPressGestureDetector* longPress = static_cast<LongPressGestureDetector*>(gestureDetector);
      mLongPressGestureProcessor.GestureDetectorUpdated(longPress);
      break;
    }

    case Gesture::Pan:
    {
      PanGestureDetector* pan = static_cast<PanGestureDetector*>(gestureDetector);
      mPanGestureProcessor.GestureDetectorUpdated(pan);
      break;
    }

    case Gesture::Pinch:
    {
      PinchGestureDetector* pinch = static_cast<PinchGestureDetector*>(gestureDetector);
      mPinchGestureProcessor.GestureDetectorUpdated(pinch);
      break;
    }

    case Gesture::Tap:
    {
      TapGestureDetector* tap = static_cast<TapGestureDetector*>(gestureDetector);
      mTapGestureProcessor.GestureDetectorUpdated(tap);
      break;
    }
  }
}

void GestureEventProcessor::SetGestureProperties( const Gesture& gesture )
{
  bool requestUpdate = false;

  switch ( gesture.type )
  {
    case Gesture::Pan:
    {
      const PanGesture& pan = static_cast< const PanGesture& >( gesture );
      requestUpdate = mPanGestureProcessor.SetPanGestureProperties( pan );
      break;
    }

    case Gesture::LongPress:
    case Gesture::Pinch:
    case Gesture::Tap:
    {
      DALI_ASSERT_DEBUG( false && "Gesture type does not have scene object\n" );
      break;
    }
  }

  if( requestUpdate )
  {
    // We may not be updating so we need to ask the render controller for an update.
    mRenderController.RequestUpdate( false );
  }
}

bool GestureEventProcessor::NeedsUpdate()
{
  bool updateRequired = false;

  updateRequired |= mLongPressGestureProcessor.NeedsUpdate();
  updateRequired |= mPanGestureProcessor.NeedsUpdate();
  updateRequired |= mPinchGestureProcessor.NeedsUpdate();
  updateRequired |= mTapGestureProcessor.NeedsUpdate();

  return updateRequired;
}

void GestureEventProcessor::EnablePanGestureProfiling()
{
  mPanGestureProcessor.EnableProfiling();
}

void GestureEventProcessor::SetPanGesturePredictionMode(int mode)
{
  mPanGestureProcessor.SetPredictionMode(mode);
}

void GestureEventProcessor::SetPanGesturePredictionAmount( uint32_t amount )
{
  mPanGestureProcessor.SetPredictionAmount(amount);
}

void GestureEventProcessor::SetPanGestureMaximumPredictionAmount( uint32_t amount )
{
  mPanGestureProcessor.SetMaximumPredictionAmount(amount);
}

void GestureEventProcessor::SetPanGestureMinimumPredictionAmount( uint32_t amount )
{
  mPanGestureProcessor.SetMinimumPredictionAmount(amount);
}

void GestureEventProcessor::SetPanGesturePredictionAmountAdjustment( uint32_t amount )
{
  mPanGestureProcessor.SetPredictionAmountAdjustment(amount);
}

void GestureEventProcessor::SetPanGestureSmoothingMode( int32_t mode )
{
  mPanGestureProcessor.SetSmoothingMode(mode);
}

void GestureEventProcessor::SetPanGestureSmoothingAmount( float amount )
{
  mPanGestureProcessor.SetSmoothingAmount(amount);
}

void GestureEventProcessor::SetPanGestureUseActualTimes( bool value )
{
  mPanGestureProcessor.SetUseActualTimes( value );
}

void GestureEventProcessor::SetPanGestureInterpolationTimeRange( int32_t value )
{
  mPanGestureProcessor.SetInterpolationTimeRange( value );
}

void GestureEventProcessor::SetPanGestureScalarOnlyPredictionEnabled( bool value )
{
  mPanGestureProcessor.SetScalarOnlyPredictionEnabled( value );
}

void GestureEventProcessor::SetPanGestureTwoPointPredictionEnabled( bool value )
{
  mPanGestureProcessor.SetTwoPointPredictionEnabled( value );
}

void GestureEventProcessor::SetPanGestureTwoPointInterpolatePastTime( int value )
{
  mPanGestureProcessor.SetTwoPointInterpolatePastTime( value );
}

void GestureEventProcessor::SetPanGestureTwoPointVelocityBias( float value )
{
  mPanGestureProcessor.SetTwoPointVelocityBias( value );
}

void GestureEventProcessor::SetPanGestureTwoPointAccelerationBias( float value )
{
  mPanGestureProcessor.SetTwoPointAccelerationBias( value );
}

void GestureEventProcessor::SetPanGestureMultitapSmoothingRange( int32_t value )
{
  mPanGestureProcessor.SetMultitapSmoothingRange( value );
}

void GestureEventProcessor::SetPanGestureMinimumDistance( int32_t value )
{
  envOptionMinimumPanDistance =  value;
}

void GestureEventProcessor::SetPanGestureMinimumPanEvents( int32_t value )
{
  envOptionMinimumPanEvents = value;
}

void GestureEventProcessor::SetPinchGestureMinimumDistance( float value)
{
  mPinchGestureProcessor.SetMinimumPinchDistance( value );
}

const PanGestureProcessor& GestureEventProcessor::GetPanGestureProcessor()
{
  return mPanGestureProcessor;
}

} // namespace Internal

} // namespace Dali
