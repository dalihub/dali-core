#ifndef TEST_GRAPHICS_RENDER_COMMAND_H
#define TEST_GRAPHICS_RENDER_COMMAND_H

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

#include <dali/graphics-api/graphics-api-render-command.h>

namespace Test
{
class GraphicsController;

class GraphicsRenderCommand : public Dali::Graphics::RenderCommand
{
public:
  explicit GraphicsRenderCommand();

  ~GraphicsRenderCommand() override;

  // Can't trace methods called on graphics api RenderCommand - they're all inlined.
  // Instead, would need to check submitted commands, and check the mUpdateFlags.
  // So, to check if a draw happens, look thru submitted commands, and check mUpdateFlags contains
  // RENDER_COMMAND_UPDATE_DRAW_BIT, and see what the mDrawCommand type is.
};

} // namespace Test

#endif // TEST_GRAPHICS_RENDER_COMMAND_H
