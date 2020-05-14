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

// HEADER
#include "render-task-devel.h"

// INTERNAL INCLUDES
#define <dali/internal/event/render-tasks/render-task-impl.h>

namespace Dali
{
namespace DevelRenderTask
{

void SetRendererFilteringMask( RenderTask renderTask, uint32_t mask )
{
  Internal::RenderTask* renderTaskPtr = GetImplementation( renderTask );
  renderTaskPtr->SetRendererFilteringMask( mask );
}

uint32_t GetRendererFilteringMask( const RenderTask renderTask )
{
  Internal::RenderTask* renderTaskPtr = GetImplementation( renderTask );
  return renderTaskPtr->GetRendererFilteringMask();
}

}
}
