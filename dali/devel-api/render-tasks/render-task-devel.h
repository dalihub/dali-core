#ifndef DALI_RENDER_TASK_DEVEL_H
#define DALI_RENDER_TASK_DEVEL_H
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
#include <dali/public-api/render-tasks/render-task.h>

namespace Dali
{
namespace DevelRenderTask
{

/**
 * @brief Default value of the mask used for Renderer filtering.
 */
enum { DEFAULT_RENDERER_FILTERING_MASK = 0xffffffff };

/**
 * @brief Sets the mask used for Renderer filtering.
 * @return The mask used for Renderer filtering.
 */
void SetRendererFilteringMask( RenderTask renderTask, uint32_t mask );

/**
 * @brief Gets the mask used for Renderer filtering.
 * @return The mask used for Renderer filtering.
 */
uint32_t GetRendererFilteringMask( const RenderTask renderTask );

}
}

#endif //DALI_RENDER_TASK_DEVEL_H
