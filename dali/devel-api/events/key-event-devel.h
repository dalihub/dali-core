#ifndef DALI_KEY_EVENT_DEVEL_H
#define DALI_KEY_EVENT_DEVEL_H

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

// INTERNAL_INCLUDES
#include <dali/public-api/events/key-event.h>

namespace Dali
{

namespace DevelKeyEvent
{
  /**
   * @brief Get the device name the key event originated from
   *
   * @param[in] keyEvent the KeyEvent to retrieve the device name from
   * @return the device name
   */
  std::string GetDeviceName( const KeyEvent& keyEvent );

  /**
   * @brief Set the device name to the KeyEvent
   *
   * @param[in] keyEvent the KeyEvent to set the device name on
   * @param[in] deviceName device name string to set
   */
  void SetDeviceName( KeyEvent& keyEvent, const std::string& deviceName );

}

}

#endif  //DALI_KEY_EVENT_DEVEL_H
