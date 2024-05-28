#ifndef DALI_GRAPHICS_SURFACE_H
#define DALI_GRAPHICS_SURFACE_H

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

#include <dali/graphics-api/graphics-types.h>

namespace Dali
{
namespace Graphics
{

struct SurfaceCreateInfo
{
  GraphicsStructureType type{GraphicsStructureType::SURFACE_CREATE_INFO_STRUCT};
  ExtensionCreateInfo* nextExtension{nullptr};
  Graphics::FramebufferId framebufferId;
  const AllocationCallbacks* allocationCallbacks{nullptr};

  auto& SetAllocationCallbacks(const AllocationCallbacks& value)
  {
    allocationCallbacks = &value;
    return *this;
  }
  auto& SetNextExtension(ExtensionCreateInfo* value)
  {
    nextExtension = value;
    return *this;
  }
  auto& SetFramebufferId(Graphics::FramebufferId framebufferId_)
  {
    framebufferId = framebufferId_;
    return *this;
  }
};

/**
 * @brief The Surface class represents a graphics surface.
 *
 * The surface can be backed by a window or external image.
 */
class Surface
{
public:
  Surface()          = default;
  virtual ~Surface() = default;

  // not copyable
  Surface(const Surface&) = delete;
  Surface& operator=(const Surface&) = delete;

protected:
  Surface(Surface&&) = default;
  Surface& operator=(Surface&&) = default;
};

} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_SURFACE_H
