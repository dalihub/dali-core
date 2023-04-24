#include "input-options.h"

#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/events/gesture-event-processor.h>

using Dali::Internal::GestureEventProcessor;
using Dali::Internal::ThreadLocalStorage;

namespace Dali
{
namespace Integration
{
void SetPanGesturePredictionMode(int mode)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGesturePredictionMode(mode);
}

void SetPanGesturePredictionAmount(unsigned int amount)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGesturePredictionAmount(amount);
}

void SetPanGestureMaximumPredictionAmount(unsigned int amount)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureMaximumPredictionAmount(amount);
}

void SetPanGestureMinimumPredictionAmount(unsigned int amount)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureMinimumPredictionAmount(amount);
}

void SetPanGesturePredictionAmountAdjustment(unsigned int amount)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGesturePredictionAmountAdjustment(amount);
}

void SetPanGestureSmoothingMode(int mode)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureSmoothingMode(mode);
}

void SetPanGestureSmoothingAmount(float amount)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureSmoothingAmount(amount);
}

void SetPanGestureUseActualTimes(bool value)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureUseActualTimes(value);
}

void SetPanGestureInterpolationTimeRange(int value)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureInterpolationTimeRange(value);
}

void SetPanGestureScalarOnlyPredictionEnabled(bool value)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureScalarOnlyPredictionEnabled(value);
}

void SetPanGestureTwoPointPredictionEnabled(bool value)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureTwoPointPredictionEnabled(value);
}

void SetPanGestureTwoPointInterpolatePastTime(int value)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureTwoPointInterpolatePastTime(value);
}

void SetPanGestureTwoPointVelocityBias(float value)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureTwoPointVelocityBias(value);
}

void SetPanGestureTwoPointAccelerationBias(float value)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureTwoPointAccelerationBias(value);
}

void SetPanGestureMultitapSmoothingRange(int value)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureMultitapSmoothingRange(value);
}

void SetPanGestureMinimumDistance(int value)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureMinimumDistance(value);
}

void SetPanGestureMinimumPanEvents(int value)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureMinimumPanEvents(value);
}

void SetPinchGestureMinimumDistance(float value)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPinchGestureMinimumDistance(value);
}

void SetPinchGestureMinimumTouchEvents(uint32_t value)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPinchGestureMinimumTouchEvents(value);
}

void SetPinchGestureMinimumTouchEventsAfterStart(uint32_t value)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPinchGestureMinimumTouchEventsAfterStart(value);
}

void SetRotationGestureMinimumTouchEvents(uint32_t value)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetRotationGestureMinimumTouchEvents(value);
}

void SetRotationGestureMinimumTouchEventsAfterStart(uint32_t value)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetRotationGestureMinimumTouchEventsAfterStart(value);
}

void SetLongPressMinimumHoldingTime(unsigned int value)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetLongPressMinimumHoldingTime(value);
}

void SetTapMaximumAllowedTime(uint32_t time)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetTapMaximumAllowedTime(time);
}

void SetTapRecognizerTime(uint32_t time)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetTapRecognizerTime(time);
}

} // namespace Integration

} // namespace Dali
