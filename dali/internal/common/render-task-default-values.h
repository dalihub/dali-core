#ifndef DALI_INTERNAL_RENDER_TASK_DEFAULT_VALUES_H
#define DALI_INTERNAL_RENDER_TASK_DEFAULT_VALUES_H

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
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/render-tasks/render-task.h>

namespace DALI_NAMESPACE
{
namespace Internal
{
namespace RenderTaskDefaultValues
{
inline constexpr bool     EXCLUSIVE     = false;
inline constexpr bool     INPUT_ENABLED = true;
inline constexpr Vector4  CLEAR_COLOR{0.0f, 0.0f, 0.0f, 1.0f};
inline constexpr bool     CLEAR_ENABLED = false;
inline constexpr bool     CULL_MODE     = true;
inline constexpr uint32_t REFRESH_RATE  = Dali::RenderTask::REFRESH_ALWAYS;
} // namespace RenderTaskDefaultValues
} // namespace Internal
} // namespace DALI_NAMESPACE

#endif // DALI_INTERNAL_RENDER_TASK_DEFAULT_VALUES_H
