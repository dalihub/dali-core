/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <algorithm> // for std::remove
#if defined(DEBUG_ENABLED)
#include <sstream>
#endif

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/render-controller.h>
#include <dali/internal/event/events/pan-gesture/pan-gesture-impl.h>
#include <dali/internal/event/events/pinch-gesture/pinch-gesture-detector-impl.h>
#include <dali/internal/update/gestures/scene-graph-pan-gesture.h>

namespace DALI_NAMESPACE
{
namespace Internal
{
GestureEventProcessor::GestureEventProcessor(SceneGraph::UpdateManager& updateManager, Integration::RenderController& renderController)
: mLongPressGestureProcessor(),
  mPanGestureProcessor(updateManager),
  mPinchGestureProcessor(),
  mTapGestureProcessor(),
  mRotationGestureProcessor(),
  mRenderController(renderController),
  mGestureDetectors(),
  mGestureOptionsEpoch(0u),
  mPanDeviceThresholds(),
  mTapDeviceThresholds(),
  mLongPressDeviceThresholds(),
  mPinchDeviceThresholds(),
  mRotationDeviceThresholds()
{
}

GestureEventProcessor::~GestureEventProcessor() = default;

void GestureEventProcessor::ProcessTouchEvent(Scene& scene, const Integration::TouchEvent& event)
{
  mLongPressGestureProcessor.ProcessTouch(scene, event);
  mPanGestureProcessor.ProcessTouch(scene, event);
  mPinchGestureProcessor.ProcessTouch(scene, event);
  mTapGestureProcessor.ProcessTouch(scene, event);
  mRotationGestureProcessor.ProcessTouch(scene, event);
}

bool GestureEventProcessor::IsRegisterGestureDetector(GestureDetector* gestureDetector)
{
  return (find(mGestureDetectors.begin(), mGestureDetectors.end(), gestureDetector) != mGestureDetectors.end());
}

void GestureEventProcessor::RegisterGestureDetector(GestureDetector* gestureDetector)
{
  if(!IsRegisterGestureDetector(gestureDetector))
  {
    mGestureDetectors.push_back(gestureDetector);
  }
}

void GestureEventProcessor::CancelAllOtherGestureDetectors(GestureDetector* gestureDetector)
{
  for(auto itr = mGestureDetectors.begin(); itr != mGestureDetectors.end(); itr++)
  {
    if((*itr) && (*itr) != gestureDetector)
    {
      (*itr)->CancelProcessing();
      (*itr)->SetDetected(false);
    }
  }
  mGestureDetectors.clear();
  mGestureDetectors.push_back(gestureDetector);
}

void GestureEventProcessor::UnregisterGestureDetector(GestureDetector* gestureDetector)
{
  // Find detector ...
  GestureDetectorContainer::iterator endIter = std::remove(mGestureDetectors.begin(), mGestureDetectors.end(), gestureDetector);
  if(endIter != mGestureDetectors.end())
  {
    // ... and remove it
    mGestureDetectors.erase(endIter, mGestureDetectors.end());
  }
}

void GestureEventProcessor::AddGestureDetector(GestureDetector* gestureDetector, Scene& scene)
{
  switch(gestureDetector->GetType())
  {
    case GestureType::LONG_PRESS:
    {
      LongPressGestureDetector* longPress = static_cast<LongPressGestureDetector*>(gestureDetector);
      mLongPressGestureProcessor.AddGestureDetector(longPress, scene);
      break;
    }

    case GestureType::PAN:
    {
      PanGestureDetector* pan = static_cast<PanGestureDetector*>(gestureDetector);
      mPanGestureProcessor.AddGestureDetector(pan, scene);
      break;
    }

    case GestureType::PINCH:
    {
      PinchGestureDetector* pinch = static_cast<PinchGestureDetector*>(gestureDetector);
      mPinchGestureProcessor.AddGestureDetector(pinch, scene);
      break;
    }

    case GestureType::TAP:
    {
      TapGestureDetector* tap = static_cast<TapGestureDetector*>(gestureDetector);
      mTapGestureProcessor.AddGestureDetector(tap, scene);
      break;
    }

    case GestureType::ROTATION:
    {
      RotationGestureDetector* rotation = static_cast<RotationGestureDetector*>(gestureDetector);
      mRotationGestureProcessor.AddGestureDetector(rotation, scene);
      break;
    }
  }
}

void GestureEventProcessor::RemoveGestureDetector(GestureDetector* gestureDetector)
{
  switch(gestureDetector->GetType())
  {
    case GestureType::LONG_PRESS:
    {
      LongPressGestureDetector* longPress = static_cast<LongPressGestureDetector*>(gestureDetector);
      mLongPressGestureProcessor.RemoveGestureDetector(longPress);
      break;
    }

    case GestureType::PAN:
    {
      PanGestureDetector* pan = static_cast<PanGestureDetector*>(gestureDetector);
      mPanGestureProcessor.RemoveGestureDetector(pan);
      break;
    }

    case GestureType::PINCH:
    {
      PinchGestureDetector* pinch = static_cast<PinchGestureDetector*>(gestureDetector);
      mPinchGestureProcessor.RemoveGestureDetector(pinch);
      break;
    }

    case GestureType::TAP:
    {
      TapGestureDetector* tap = static_cast<TapGestureDetector*>(gestureDetector);
      mTapGestureProcessor.RemoveGestureDetector(tap);
      break;
    }

    case GestureType::ROTATION:
    {
      RotationGestureDetector* rotation = static_cast<RotationGestureDetector*>(gestureDetector);
      mRotationGestureProcessor.RemoveGestureDetector(rotation);
      break;
    }
  }
}

void GestureEventProcessor::GestureDetectorUpdated(GestureDetector* gestureDetector)
{
  switch(gestureDetector->GetType())
  {
    case GestureType::LONG_PRESS:
    {
      LongPressGestureDetector* longPress = static_cast<LongPressGestureDetector*>(gestureDetector);
      mLongPressGestureProcessor.GestureDetectorUpdated(longPress);
      break;
    }

    case GestureType::PAN:
    {
      PanGestureDetector* pan = static_cast<PanGestureDetector*>(gestureDetector);
      mPanGestureProcessor.GestureDetectorUpdated(pan);
      break;
    }

    case GestureType::PINCH:
    {
      PinchGestureDetector* pinch = static_cast<PinchGestureDetector*>(gestureDetector);
      mPinchGestureProcessor.GestureDetectorUpdated(pinch);
      break;
    }

    case GestureType::TAP:
    {
      TapGestureDetector* tap = static_cast<TapGestureDetector*>(gestureDetector);
      mTapGestureProcessor.GestureDetectorUpdated(tap);
      break;
    }

    case GestureType::ROTATION:
    {
      // Nothing to do
      break;
    }
  }
}

void GestureEventProcessor::SetGestureProperties(const Dali::Gesture& gesture)
{
  DALI_ASSERT_DEBUG(gesture.GetType() == GestureType::PAN && "Only PanGesture has a scene object\n");

  const Dali::PanGesture& pan = static_cast<const Dali::PanGesture&>(gesture);
  if(mPanGestureProcessor.SetPanGestureProperties(pan))
  {
    // We may not be updating so we need to ask the render controller for an update.
    mRenderController.RequestUpdate();
  }
}

bool GestureEventProcessor::NeedsUpdate()
{
  bool updateRequired = false;

  updateRequired |= mLongPressGestureProcessor.NeedsUpdate();
  updateRequired |= mPanGestureProcessor.NeedsUpdate();
  updateRequired |= mPinchGestureProcessor.NeedsUpdate();
  updateRequired |= mTapGestureProcessor.NeedsUpdate();
  updateRequired |= mRotationGestureProcessor.NeedsUpdate();

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

void GestureEventProcessor::SetPanGesturePredictionAmount(uint32_t amount)
{
  mPanGestureProcessor.SetPredictionAmount(amount);
}

void GestureEventProcessor::SetPanGestureMaximumPredictionAmount(uint32_t amount)
{
  mPanGestureProcessor.SetMaximumPredictionAmount(amount);
}

void GestureEventProcessor::SetPanGestureMinimumPredictionAmount(uint32_t amount)
{
  mPanGestureProcessor.SetMinimumPredictionAmount(amount);
}

void GestureEventProcessor::SetPanGesturePredictionAmountAdjustment(uint32_t amount)
{
  mPanGestureProcessor.SetPredictionAmountAdjustment(amount);
}

void GestureEventProcessor::SetPanGestureSmoothingMode(int32_t mode)
{
  mPanGestureProcessor.SetSmoothingMode(mode);
}

void GestureEventProcessor::SetPanGestureSmoothingAmount(float amount)
{
  mPanGestureProcessor.SetSmoothingAmount(amount);
}

void GestureEventProcessor::SetPanGestureUseActualTimes(bool value)
{
  mPanGestureProcessor.SetUseActualTimes(value);
}

void GestureEventProcessor::SetPanGestureInterpolationTimeRange(int32_t value)
{
  mPanGestureProcessor.SetInterpolationTimeRange(value);
}

void GestureEventProcessor::SetPanGestureScalarOnlyPredictionEnabled(bool value)
{
  mPanGestureProcessor.SetScalarOnlyPredictionEnabled(value);
}

void GestureEventProcessor::SetPanGestureTwoPointPredictionEnabled(bool value)
{
  mPanGestureProcessor.SetTwoPointPredictionEnabled(value);
}

void GestureEventProcessor::SetPanGestureTwoPointInterpolatePastTime(int value)
{
  mPanGestureProcessor.SetTwoPointInterpolatePastTime(value);
}

void GestureEventProcessor::SetPanGestureTwoPointVelocityBias(float value)
{
  mPanGestureProcessor.SetTwoPointVelocityBias(value);
}

void GestureEventProcessor::SetPanGestureTwoPointAccelerationBias(float value)
{
  mPanGestureProcessor.SetTwoPointAccelerationBias(value);
}

void GestureEventProcessor::SetPanGestureMultitapSmoothingRange(int32_t value)
{
  mPanGestureProcessor.SetMultitapSmoothingRange(value);
}

void GestureEventProcessor::SetPanGestureMinimumDistance(int32_t value)
{
  ++mGestureOptionsEpoch;
  mPanGestureProcessor.SetMinimumDistance(value);
}

void GestureEventProcessor::SetPanGestureMinimumPanEvents(int32_t value)
{
  ++mGestureOptionsEpoch;
  mPanGestureProcessor.SetMinimumPanEvents(value);
}

void GestureEventProcessor::SetPinchGestureMinimumDistance(float value)
{
  ++mGestureOptionsEpoch;
  mPinchGestureProcessor.SetMinimumPinchDistance(value);
}

void GestureEventProcessor::SetPinchGestureMinimumTouchEvents(uint32_t value)
{
  ++mGestureOptionsEpoch;
  mPinchGestureProcessor.SetMinimumTouchEvents(value);
}

void GestureEventProcessor::SetPinchGestureMinimumTouchEventsAfterStart(uint32_t value)
{
  ++mGestureOptionsEpoch;
  mPinchGestureProcessor.SetMinimumTouchEventsAfterStart(value);
}

void GestureEventProcessor::SetRotationGestureMinimumTouchEvents(uint32_t value)
{
  ++mGestureOptionsEpoch;
  mRotationGestureProcessor.SetMinimumTouchEvents(value);
}

void GestureEventProcessor::SetRotationGestureMinimumTouchEventsAfterStart(uint32_t value)
{
  ++mGestureOptionsEpoch;
  mRotationGestureProcessor.SetMinimumTouchEventsAfterStart(value);
}

void GestureEventProcessor::SetLongPressGestureMinimumHoldingTime(uint32_t value)
{
  ++mGestureOptionsEpoch;
  mLongPressGestureProcessor.SetMinimumHoldingTime(value);
}

uint32_t GestureEventProcessor::GetLongPressGestureMinimumHoldingTime() const
{
  return mLongPressGestureProcessor.GetMinimumHoldingTime();
}

const PanGestureProcessor& GestureEventProcessor::GetPanGestureProcessor()
{
  return mPanGestureProcessor;
}

void GestureEventProcessor::SetTapGestureMaximumMultiTapInterval(uint32_t time)
{
  ++mGestureOptionsEpoch;
  mTapGestureProcessor.SetMaximumMultiTapInterval(time);
}

uint32_t GestureEventProcessor::GetTapGestureMaximumMultiTapInterval() const
{
  return mTapGestureProcessor.GetMaximumMultiTapInterval();
}

void GestureEventProcessor::SetTapGestureMaximumHoldingTime(uint32_t time)
{
  ++mGestureOptionsEpoch;
  mTapGestureProcessor.SetMaximumHoldingTime(time);
}

uint32_t GestureEventProcessor::GetTapGestureMaximumHoldingTime() const
{
  return mTapGestureProcessor.GetMaximumHoldingTime();
}

void GestureEventProcessor::SetTapGestureMaximumMotionDistance(float distance)
{
  ++mGestureOptionsEpoch;
  mTapGestureProcessor.SetMaximumMotionDistance(distance);
}

float GestureEventProcessor::GetTapGestureMaximumMotionDistance() const
{
  return mTapGestureProcessor.GetMaximumMotionDistance();
}

uint32_t GestureEventProcessor::GetGestureOptionsEpoch() const
{
  return mGestureOptionsEpoch;
}

void GestureEventProcessor::SetPanDeviceThresholds(const GestureDeviceSelector& selector, const PanThresholdValues& thresholds)
{
  mPanDeviceThresholds.Set(selector, thresholds);
  ++mGestureOptionsEpoch;
  mPanGestureProcessor.SetDeviceThresholds(mPanDeviceThresholds);
}

const PanThresholdValues* GestureEventProcessor::GetPanDeviceThresholds(const GestureDeviceSelector& selector) const
{
  return mPanDeviceThresholds.Find(selector);
}

void GestureEventProcessor::ClearPanDeviceThresholds(const GestureDeviceSelector& selector)
{
  if(mPanDeviceThresholds.Clear(selector))
  {
    ++mGestureOptionsEpoch;
    mPanGestureProcessor.SetDeviceThresholds(mPanDeviceThresholds);
  }
}

void GestureEventProcessor::SetTapDeviceThresholds(const GestureDeviceSelector& selector, const TapThresholdValues& thresholds)
{
  mTapDeviceThresholds.Set(selector, thresholds);
  ++mGestureOptionsEpoch;
  mTapGestureProcessor.SetDeviceThresholds(mTapDeviceThresholds);
}

const TapThresholdValues* GestureEventProcessor::GetTapDeviceThresholds(const GestureDeviceSelector& selector) const
{
  return mTapDeviceThresholds.Find(selector);
}

void GestureEventProcessor::ClearTapDeviceThresholds(const GestureDeviceSelector& selector)
{
  if(mTapDeviceThresholds.Clear(selector))
  {
    ++mGestureOptionsEpoch;
    mTapGestureProcessor.SetDeviceThresholds(mTapDeviceThresholds);
  }
}

void GestureEventProcessor::SetLongPressDeviceThresholds(const GestureDeviceSelector& selector, const LongPressThresholdValues& thresholds)
{
  mLongPressDeviceThresholds.Set(selector, thresholds);
  ++mGestureOptionsEpoch;
  mLongPressGestureProcessor.SetDeviceThresholds(mLongPressDeviceThresholds);
}

const LongPressThresholdValues* GestureEventProcessor::GetLongPressDeviceThresholds(const GestureDeviceSelector& selector) const
{
  return mLongPressDeviceThresholds.Find(selector);
}

void GestureEventProcessor::ClearLongPressDeviceThresholds(const GestureDeviceSelector& selector)
{
  if(mLongPressDeviceThresholds.Clear(selector))
  {
    ++mGestureOptionsEpoch;
    mLongPressGestureProcessor.SetDeviceThresholds(mLongPressDeviceThresholds);
  }
}

void GestureEventProcessor::SetPinchDeviceThresholds(const GestureDeviceSelector& selector, const PinchThresholdValues& thresholds)
{
  mPinchDeviceThresholds.Set(selector, thresholds);
  ++mGestureOptionsEpoch;
  mPinchGestureProcessor.SetDeviceThresholds(mPinchDeviceThresholds);
}

const PinchThresholdValues* GestureEventProcessor::GetPinchDeviceThresholds(const GestureDeviceSelector& selector) const
{
  return mPinchDeviceThresholds.Find(selector);
}

void GestureEventProcessor::ClearPinchDeviceThresholds(const GestureDeviceSelector& selector)
{
  if(mPinchDeviceThresholds.Clear(selector))
  {
    ++mGestureOptionsEpoch;
    mPinchGestureProcessor.SetDeviceThresholds(mPinchDeviceThresholds);
  }
}

void GestureEventProcessor::SetRotationDeviceThresholds(const GestureDeviceSelector& selector, const RotationThresholdValues& thresholds)
{
  mRotationDeviceThresholds.Set(selector, thresholds);
  ++mGestureOptionsEpoch;
  mRotationGestureProcessor.SetDeviceThresholds(mRotationDeviceThresholds);
}

const RotationThresholdValues* GestureEventProcessor::GetRotationDeviceThresholds(const GestureDeviceSelector& selector) const
{
  return mRotationDeviceThresholds.Find(selector);
}

void GestureEventProcessor::ClearRotationDeviceThresholds(const GestureDeviceSelector& selector)
{
  if(mRotationDeviceThresholds.Clear(selector))
  {
    ++mGestureOptionsEpoch;
    mRotationGestureProcessor.SetDeviceThresholds(mRotationDeviceThresholds);
  }
}

const LongPressGestureProcessor& GestureEventProcessor::GetLongPressGestureProcessor()
{
  return mLongPressGestureProcessor;
}

const TapGestureProcessor& GestureEventProcessor::GetTapGestureProcessor()
{
  return mTapGestureProcessor;
}

const PinchGestureProcessor& GestureEventProcessor::GetPinchGestureProcessor()
{
  return mPinchGestureProcessor;
}

const RotationGestureProcessor& GestureEventProcessor::GetRotationGestureProcessor()
{
  return mRotationGestureProcessor;
}

} // namespace Internal

} //namespace DALI_NAMESPACE
