#ifndef DALI_INTEGRATION_CORE_ENUMERATIONS_H
#define DALI_INTEGRATION_CORE_ENUMERATIONS_H

/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/devel-api/common/bitwise-enum.h>
#include <dali/integration-api/context-notifier.h>
#include <dali/integration-api/resource-policies.h>
#include <dali/public-api/common/dali-common.h>

#undef FALSE
#undef TRUE

namespace Dali
{
namespace Integration
{
/**
 * @brief Enumerations to specify whether we should render to the frame-buffer.
 */
enum class RenderToFrameBuffer
{
  FALSE = 0,
  TRUE
};

/**
 * @brief Enumerations to specify whether the depth buffer is available.
 */
enum class DepthBufferAvailable
{
  FALSE = 0,
  TRUE
};

/**
 * @brief Enumerations to specify whether the stencil buffer is available.
 */
enum class StencilBufferAvailable
{
  FALSE = 0,
  TRUE
};

/**
 * @brief Enumerations to specify whether the partial update is available.
 */
enum class PartialUpdateAvailable
{
  FALSE = 0,
  TRUE
};

/**
 * The additional policy of core update / rendering process.
 * @note Policy cannot be changed after the Core initialized.
 */
enum class CorePolicyFlags
{
  DEFAULT                  = 0,      ///< Zero means that no additional policy is set.
  RENDER_TO_FRAME_BUFFER   = 1 << 0, ///< Whether we should render to the frame-buffer or not.
  DEPTH_BUFFER_AVAILABLE   = 1 << 1, ///< Whether the depth buffer is available or not.
  STENCIL_BUFFER_AVAILABLE = 1 << 2, ///< Whether the stencil buffer is available or not.
  PARTIAL_UPDATE_AVAILABLE = 1 << 3, ///< Whether the partial update is available or not.
};

} // namespace Integration

// specialization has to be done in the same namespace
template<>
struct EnableBitMaskOperators<Integration::CorePolicyFlags>
{
  static const bool ENABLE = true;
};
} // namespace Dali

#endif // DALI_INTEGRATION_CORE_ENUMERATIONS_H
