#ifndef DALI_CAPABILITIES_H
#define DALI_CAPABILITIES_H

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

// INTERNAL INCLUDES
#include <dali/public-api/rendering/renderer.h>

namespace Dali
{
/**
 * @addtogroup dali_core_common
 * @{
 */

namespace Capabilities
{
/**
 * @brief Queries whether the given blend equation is supported on the current system.
 *
 * Some BlendEquation::Type values, such as the advanced blend equations (e.g. MULTIPLY,
 * SCREEN, and so on), require OpenGL ES 3.2 or the KHR_blend_equation_advanced extension,
 * and may not be supported on every device.
 *
 * @SINCE_2_5.30
 * @param[in] blendEquation The blend equation to check.
 * @return True if the blend equation is supported on the current system, false otherwise.
 */
DALI_CORE_API bool IsBlendEquationSupported(BlendEquation::Type blendEquation);

} // namespace Capabilities

/**
 * @}
 */
} // namespace Dali

#endif // DALI_CAPABILITIES_H