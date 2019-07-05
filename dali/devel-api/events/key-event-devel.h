#ifndef DALI_KEY_EVENT_DEVEL_H
#define DALI_KEY_EVENT_DEVEL_H

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

// INTERNAL INCLUDES
#include <dali/public-api/events/key-event.h>

namespace Dali
{

namespace DevelKeyEvent
{

/**
 * @brief Gets the logical key string.
 *
 * For example, when the user presses 'shift' key and '1' key together, the logical key is "exclamation".
 * Plus, the keyPressedName is "1", and the keyPressed is "!".
 *
 * @param[in] keyEvent The instance of KeyEvent.
 * @return The logical key symbol
 */
DALI_CORE_API std::string GetLogicalKey( KeyEvent keyEvent );

} // namespace DevelKeyEvent

} // namespace Dali

#endif // DALI_KEY_EVENT_DEVEL_H
