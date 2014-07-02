#include "input-options.h"

#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/events/gesture-event-processor.h>

using Dali::Internal::GestureEventProcessor;
using Dali::Internal::ThreadLocalStorage;

namespace Dali
{

namespace Integration
{

void SetPanGesturePredictionMode( int mode )
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGesturePredictionMode(mode);
}

void SetPanGesturePredictionAmount(unsigned int amount)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGesturePredictionAmount(amount);
}

void SetPanGestureSmoothingMode(int mode)
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureSmoothingMode(mode);
}

void SetPanGestureSmoothingAmount( float amount )
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();
  eventProcessor.SetPanGestureSmoothingAmount(amount);
}

} // namespace Integration

} // namespace Dali
