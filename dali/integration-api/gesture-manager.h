#ifndef __DALI_INTEGRATION_GESTURE_MANAGER_H__
#define __DALI_INTEGRATION_GESTURE_MANAGER_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/events/gesture.h>
#include <dali/integration-api/events/gesture-requests.h>

namespace Dali
{

namespace Integration
{

/**
 * GestureManager is an abstract interface, used by Dali to register and unregister gestures provided
 * by the adaptor. A concrete implementation must be created for each adaptor, and provided when creating
 * the Dali::Integration::Core object.
 */
class DALI_CORE_API GestureManager
{
public:

  /**
   * Called by Dali to enable the adaptor to start detecting the required gesture type.
   * @param[in]  request  The required gesture and details.
   */
  virtual void Register(const GestureRequest& request) = 0;

  /**
   * Called by Dali to inform the adaptor that it no longer requires a GestureEvent when the state
   * gesture type is detected.
   * @param[in]  request  The gesture that is no longer required.
   */
  virtual void Unregister(const GestureRequest& request) = 0;

  /**
   * Called by Dali to inform the adaptor that the detection parameters of a previously requested
   * gesture have now changed.
   * @param[in]  request  The gesture and updated details.
   */
  virtual void Update(const GestureRequest& request) = 0;

protected:

  /**
   * Virtual destructor, no deletion through this interface
   */
  virtual ~GestureManager() {}

}; // class GestureManager

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_GESTURE_MANAGER_H__
