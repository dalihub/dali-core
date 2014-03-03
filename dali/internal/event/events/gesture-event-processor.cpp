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

// CLASS HEADER
#include <dali/internal/event/events/gesture-event-processor.h>

// INTERNAL INCLUDES
#include <dali/integration-api/events/gesture-event.h>
#include <dali/integration-api/events/long-press-gesture-event.h>
#include <dali/integration-api/events/pan-gesture-event.h>
#include <dali/integration-api/events/pinch-gesture-event.h>
#include <dali/integration-api/events/tap-gesture-event.h>
#include <dali/integration-api/gesture-manager.h>
#include <dali/integration-api/render-controller.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/events/pinch-gesture-detector-impl.h>
#include <dali/public-api/events/pan-gesture.h>

namespace Dali
{

namespace Internal
{

GestureEventProcessor::GestureEventProcessor(Stage& stage, Integration::GestureManager& gestureManager, Integration::RenderController& renderController)
: mStage(stage),
  mGestureManager(gestureManager),
  mLongPressGestureProcessor(stage, gestureManager),
  mPanGestureProcessor(stage, gestureManager),
  mPinchGestureProcessor(stage, gestureManager),
  mTapGestureProcessor(stage, gestureManager),
  mRenderController(renderController),
  mUpdateRequired( false )
{
}

GestureEventProcessor::~GestureEventProcessor()
{
}

void GestureEventProcessor::ProcessGestureEvent(const Integration::GestureEvent& event)
{
  if( Gesture::Started == event.state || Gesture::Continuing == event.state )
  {
    SetUpdateRequired();
  }

  switch(event.gestureType)
  {
    case Gesture::LongPress:
      mLongPressGestureProcessor.Process(static_cast<const Integration::LongPressGestureEvent&>(event));
      break;

    case Gesture::Pan:
      mPanGestureProcessor.Process(static_cast<const Integration::PanGestureEvent&>(event));
      break;

    case Gesture::Pinch:
      mPinchGestureProcessor.Process(static_cast<const Integration::PinchGestureEvent&>(event));
      break;

    case Gesture::Tap:
      mTapGestureProcessor.Process(static_cast<const Integration::TapGestureEvent&>(event));
      break;

    default:
      DALI_ASSERT_ALWAYS( false && "Invalid gesture type sent from Integration\n" );
      break;
  }
}

void GestureEventProcessor::AddGestureDetector(GestureDetector* gestureDetector)
{
  switch (gestureDetector->GetType())
  {
    case Gesture::LongPress:
    {
      LongPressGestureDetector* longPress = static_cast<LongPressGestureDetector*>(gestureDetector);
      mLongPressGestureProcessor.AddGestureDetector(longPress);
      break;
    }

    case Gesture::Pan:
    {
      PanGestureDetector* pan = static_cast<PanGestureDetector*>(gestureDetector);
      mPanGestureProcessor.AddGestureDetector(pan);
      break;
    }

    case Gesture::Pinch:
    {
      PinchGestureDetector* pinch = static_cast<PinchGestureDetector*>(gestureDetector);
      mPinchGestureProcessor.AddGestureDetector(pinch);
      break;
    }

    case Gesture::Tap:
    {
      TapGestureDetector* tap = static_cast<TapGestureDetector*>(gestureDetector);
      mTapGestureProcessor.AddGestureDetector(tap);
      break;
    }

    default:
      DALI_ASSERT_DEBUG( false && "Invalid gesture detector type created\n" );
      break;
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

    default:
      DALI_ASSERT_DEBUG( false && "Invalid gesture detector type removal request\n" );
      break;
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

    default:
      DALI_ASSERT_DEBUG( false && "Invalid gesture type update request\n" );
      break;
  }
}

void GestureEventProcessor::SetUpdateRequired()
{
  mUpdateRequired = true;
}

void GestureEventProcessor::SetGestureProperties( const Gesture& gesture )
{
  if( Gesture::Started == gesture.state || Gesture::Continuing == gesture.state )
  {
    SetUpdateRequired();

    // We may not be updating so we need to ask the render controller for an update.
    mRenderController.RequestUpdate();
  }

  switch ( gesture.type )
  {
    case Gesture::Pan:
    {
      const PanGesture& pan = static_cast< const PanGesture& >( gesture );
      mPanGestureProcessor.SetPanGestureProperties( pan );
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
}

bool GestureEventProcessor::NeedsUpdate()
{
  bool updateRequired( mUpdateRequired );

  mUpdateRequired = false;

  return updateRequired;
}

} // namespace Internal

} // namespace Dali
