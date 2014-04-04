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
#include <dali/integration-api/profiling.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/events/gesture-event-processor.h>

using Dali::Internal::GestureEventProcessor;
using Dali::Internal::ThreadLocalStorage;

namespace Dali
{

namespace Integration
{

void EnableProfiling( ProfilingType type )
{
  GestureEventProcessor& eventProcessor = ThreadLocalStorage::Get().GetGestureEventProcessor();

  switch( type )
  {
    case PROFILING_TYPE_PAN_GESTURE:
    {
      eventProcessor.EnablePanGestureProfiling();
    }

    default:
    {
      // Do nothing
      break;
    }
  }
}

} // namespace Integration

} // namespace Dali
