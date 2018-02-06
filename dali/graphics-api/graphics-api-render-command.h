#ifndef DALI_GRAPHICS_API_RENDER_COMMAND_H
#define DALI_GRAPHICS_API_RENDER_COMMAND_H

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
#include <tuple>
#include <utility>
#include <vector>

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-api-buffer.h>
#include <dali/graphics-api/utility/utility-strong-type.h>

namespace Dali
{
namespace Graphics
{
namespace API
{

using BufferInfo = std::tuple<Buffer::UsageHint, Buffer>;
using BufferList = Utility::StrongType<std::vector<BufferInfo>, struct BufferListTag>;


/**
 * @brief Interface class for RenderCommand types in the graphics API.
 */
class RenderCommand
{
public:
  // not copyable
  RenderCommand(const RenderCommand&) = delete;
  RenderCommand& operator=(const RenderCommand&) = delete;

  virtual ~RenderCommand() = default;

protected:
  // derived types should not be moved direcly to prevent slicing
  RenderCommand(RenderCommand&&) = default;
  RenderCommand& operator=(RenderCommand&&) = default;

private:
  /**
   * Objects of this type should not directly.
   */
  RenderCommand() = default;

  friend 
};

} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_RENDER_COMMAND_H
