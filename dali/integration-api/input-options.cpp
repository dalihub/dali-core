#include "input-options.h"

#include <dali/integration-api/debug.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/events/gesture-event-processor.h>

using Dali::Internal::GestureEventProcessor;
using Dali::Internal::ThreadLocalStorage;

namespace DALI_NAMESPACE
{
namespace Integration
{
void SetPanGesturePredictionMode(int mode)
{
  DALI_LOG_RELEASE_INFO("mode=%d\n", mode);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGesturePredictionMode(mode);
}

void SetPanGesturePredictionAmount(unsigned int amount)
{
  DALI_LOG_RELEASE_INFO("amount=%u\n", amount);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGesturePredictionAmount(amount);
}

void SetPanGestureMaximumPredictionAmount(unsigned int amount)
{
  DALI_LOG_RELEASE_INFO("amount=%u\n", amount);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureMaximumPredictionAmount(amount);
}

void SetPanGestureMinimumPredictionAmount(unsigned int amount)
{
  DALI_LOG_RELEASE_INFO("amount=%u\n", amount);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureMinimumPredictionAmount(amount);
}

void SetPanGesturePredictionAmountAdjustment(unsigned int amount)
{
  DALI_LOG_RELEASE_INFO("amount=%u\n", amount);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGesturePredictionAmountAdjustment(amount);
}

void SetPanGestureSmoothingMode(int mode)
{
  DALI_LOG_RELEASE_INFO("mode=%d\n", mode);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureSmoothingMode(mode);
}

void SetPanGestureSmoothingAmount(float amount)
{
  DALI_LOG_RELEASE_INFO("amount=%f\n", amount);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureSmoothingAmount(amount);
}

void SetPanGestureUseActualTimes(bool value)
{
  DALI_LOG_RELEASE_INFO("value=%d\n", value);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureUseActualTimes(value);
}

void SetPanGestureInterpolationTimeRange(int value)
{
  DALI_LOG_RELEASE_INFO("value=%d\n", value);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureInterpolationTimeRange(value);
}

void SetPanGestureScalarOnlyPredictionEnabled(bool value)
{
  DALI_LOG_RELEASE_INFO("value=%d\n", value);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureScalarOnlyPredictionEnabled(value);
}

void SetPanGestureTwoPointPredictionEnabled(bool value)
{
  DALI_LOG_RELEASE_INFO("value=%d\n", value);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureTwoPointPredictionEnabled(value);
}

void SetPanGestureTwoPointInterpolatePastTime(int value)
{
  DALI_LOG_RELEASE_INFO("value=%d\n", value);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureTwoPointInterpolatePastTime(value);
}

void SetPanGestureTwoPointVelocityBias(float value)
{
  DALI_LOG_RELEASE_INFO("value=%f\n", value);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureTwoPointVelocityBias(value);
}

void SetPanGestureTwoPointAccelerationBias(float value)
{
  DALI_LOG_RELEASE_INFO("value=%f\n", value);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureTwoPointAccelerationBias(value);
}

void SetPanGestureMultitapSmoothingRange(int value)
{
  DALI_LOG_RELEASE_INFO("value=%d\n", value);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureMultitapSmoothingRange(value);
}

void SetPanGestureMinimumDistance(int value)
{
  DALI_LOG_RELEASE_INFO("value=%d\n", value);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureMinimumDistance(value);
}

int GetPanGestureMinimumDistance()
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  return eventProcessor.GetPanGestureProcessor().GetMinimumDistance();
}

void SetPanGestureMinimumPanEvents(int value)
{
  DALI_LOG_RELEASE_INFO("value=%d\n", value);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureMinimumPanEvents(value);
}

int GetPanGestureMinimumPanEvents()
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  return eventProcessor.GetPanGestureProcessor().GetMinimumPanEvents();
}

void SetPinchGestureMinimumDistance(float value)
{
  DALI_LOG_RELEASE_INFO("value=%f\n", value);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPinchGestureMinimumDistance(value);
}

float GetPinchGestureMinimumDistance()
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  return eventProcessor.GetPinchGestureProcessor().GetMinimumPinchDistance();
}

void SetPinchGestureMinimumTouchEvents(uint32_t value)
{
  DALI_LOG_RELEASE_INFO("value=%u\n", value);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPinchGestureMinimumTouchEvents(value);
}

uint32_t GetPinchGestureMinimumTouchEvents()
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  return eventProcessor.GetPinchGestureProcessor().GetMinimumTouchEvents();
}

void SetPinchGestureMinimumTouchEventsAfterStart(uint32_t value)
{
  DALI_LOG_RELEASE_INFO("value=%u\n", value);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPinchGestureMinimumTouchEventsAfterStart(value);
}

uint32_t GetPinchGestureMinimumTouchEventsAfterStart()
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  return eventProcessor.GetPinchGestureProcessor().GetMinimumTouchEventsAfterStart();
}

void SetRotationGestureMinimumTouchEvents(uint32_t value)
{
  DALI_LOG_RELEASE_INFO("value=%u\n", value);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetRotationGestureMinimumTouchEvents(value);
}

uint32_t GetRotationGestureMinimumTouchEvents()
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  return eventProcessor.GetRotationGestureProcessor().GetMinimumTouchEvents();
}

void SetRotationGestureMinimumTouchEventsAfterStart(uint32_t value)
{
  DALI_LOG_RELEASE_INFO("value=%u\n", value);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetRotationGestureMinimumTouchEventsAfterStart(value);
}

uint32_t GetRotationGestureMinimumTouchEventsAfterStart()
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  return eventProcessor.GetRotationGestureProcessor().GetMinimumTouchEventsAfterStart();
}

void SetLongPressGestureMinimumHoldingTime(uint32_t time)
{
  DALI_LOG_RELEASE_INFO("time=%u\n", time);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetLongPressGestureMinimumHoldingTime(time);
}

uint32_t GetLongPressGestureMinimumHoldingTime()
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  return eventProcessor.GetLongPressGestureMinimumHoldingTime();
}

void SetTapGestureMaximumMultiTapInterval(uint32_t interval)
{
  DALI_LOG_RELEASE_INFO("interval=%u\n", interval);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetTapGestureMaximumMultiTapInterval(interval);
}

uint32_t GetTapGestureMaximumMultiTapInterval()
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  return eventProcessor.GetTapGestureMaximumMultiTapInterval();
}

void SetTapGestureMaximumHoldingTime(uint32_t time)
{
  DALI_LOG_RELEASE_INFO("time=%u\n", time);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetTapGestureMaximumHoldingTime(time);
}

uint32_t GetTapGestureMaximumHoldingTime()
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  return eventProcessor.GetTapGestureMaximumHoldingTime();
}

void SetTapGestureMaximumMotionDistance(float distance)
{
  DALI_LOG_RELEASE_INFO("distance=%f\n", distance);
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetTapGestureMaximumMotionDistance(distance);
}

float GetTapGestureMaximumMotionDistance()
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  return eventProcessor.GetTapGestureMaximumMotionDistance();
}

} // namespace Integration

} //namespace DALI_NAMESPACE
