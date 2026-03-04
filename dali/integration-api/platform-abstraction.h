#ifndef DALI_INTEGRATION_PLATFORM_ABSTRACTION_H
#define DALI_INTEGRATION_PLATFORM_ABSTRACTION_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/signals/callback.h>

namespace Dali
{

namespace Integration
{
/**
 * PlatformAbstraction is an abstract interface, used by Dali to access platform specific services.
 * A concrete implementation must be created for each platform, and provided when creating the
 * Dali::Integration::Core object.
 */
class PlatformAbstraction
{
public:
  /**
   * Sets a callback to occur in the future
   * @param[in] milliseconds number of milliseconds to wait until executing the callback
   * @param[in] callback function to call when the timer expires
   * @result    a timer reference ID, to be used for cancelling the timer
   */
  virtual uint32_t StartTimer(uint32_t milliseconds, CallbackBase* callback) = 0;

  /**
   * Cancels a running timer
   * @param[in] timerId the ID reference returned when the timer was started
   */
  virtual void CancelTimer(uint32_t timerId) = 0;

protected:
  /**
   * Virtual destructor.
   */
  virtual ~PlatformAbstraction() = default;

}; // class PlatformAbstraction

} // namespace Integration

} // namespace Dali

#endif // DALI_INTEGRATION_PLATFORM_ABSTRACTION_H
