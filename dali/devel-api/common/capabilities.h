#ifndef DALI_CAPABILITIES_H
#define DALI_CAPABILITIES_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/rendering/renderer-devel.h>

namespace Dali
{
namespace Capabilities
{
/**
 * @brief Returns whether the blend equation is supported in the system or not.
 * @param[in] blendEquation blend equation to be checked.
 * @return True if the blend equation supported.
 */
DALI_CORE_API bool IsBlendEquationSupported(BlendEquation::Type blendEquation);

/**
 * @brief Returns whether the blend equation is supported in the system or not.
 * @param[in] blendEquation blend equation to be checked.
 * @return True if the blend equation supported.
 */
DALI_CORE_API bool IsBlendEquationSupported(DevelBlendEquation::Type blendEquation);

} // namespace Capabilities

} // namespace Dali

#endif // DALI_CAPABILITIES_H