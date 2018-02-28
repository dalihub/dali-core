#ifndef DALI_INTEGRATION_CORE_ENUMERATIONS_H
#define DALI_INTEGRATION_CORE_ENUMERATIONS_H

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

// EXTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/view-mode.h>
#include <dali/integration-api/context-notifier.h>
#include <dali/integration-api/resource-policies.h>

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

} // namespace Integration

} // namespace Dali

#endif // DALI_INTEGRATION_CORE_ENUMERATIONS_H
