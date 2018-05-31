#ifndef DALI_STAGE_DEVEL_H
#define DALI_STAGE_DEVEL_H

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
#include <dali/public-api/common/stage.h>

namespace Dali
{

namespace DevelStage
{

typedef Signal< bool (const KeyEvent&) > KeyEventGeneratedSignalType;      ///< Stage key event generated signal type

/**
 * @brief The user would connect to this signal to get a KeyEvent when KeyEvent is generated.
 *
 * @param[in] stage The stage to emit a signal
 * @return The return is true if KeyEvent is consumed, otherwise false.
 */
DALI_CORE_API KeyEventGeneratedSignalType& KeyEventGeneratedSignal( Dali::Stage stage );

} // namespace DevelStage

} // namespace Dali

#endif // DALI_STAGE_DEVEL_H
