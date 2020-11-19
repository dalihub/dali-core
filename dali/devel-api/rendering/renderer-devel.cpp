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
#include <dali/devel-api/rendering/renderer-devel.h>
#include <dali/internal/event/rendering/renderer-impl.h>

namespace Dali
{
namespace DevelRenderer
{
bool IsAdvancedBlendEquationApplied(const Renderer& renderer)
{
  return GetImplementation(renderer).IsAdvancedBlendEquationApplied();
}

void AddDrawCommand(Dali::Renderer renderer, const DrawCommand& drawCommand)
{
  auto& impl = GetImplementation(renderer);
  impl.AddDrawCommand(drawCommand);
}

} // namespace DevelRenderer
} // namespace Dali