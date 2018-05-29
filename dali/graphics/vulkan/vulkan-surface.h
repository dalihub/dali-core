#ifndef DALI_GRAPHICS_VULKAN_SURFACE
#define DALI_GRAPHICS_VULKAN_SURFACE

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

// INTERNAL INCLUDES
#include <dali/graphics/vulkan/vulkan-types.h>

namespace Dali
{
namespace Integration
{
namespace Graphics
{
class SurfaceFactory;
}
}
namespace Graphics
{
namespace Vulkan
{
using SurfaceFactory = Dali::Integration::Graphics::SurfaceFactory;
class Graphics;
class Surface : public VkManaged
{
public:

  static RefCountedSurface New( Graphics& graphics, std::unique_ptr<SurfaceFactory> surfaceFactory );

  Surface(Graphics& graphics, std::unique_ptr<SurfaceFactory> surfaceFactory );
  ~Surface() final;

  /**
   * Creates surface from given factory
   */
  bool Create();

  /**
   *
   * @return
   */
  vk::SurfaceKHR GetSurfaceKHR() const;

  /**
   * Returns size of surface
   * @return
   */
  vk::Extent2D GetSize() const;

private:

  struct Impl;
  std::unique_ptr<Impl> mImpl;
};

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_VULKAN_SURFACE
