/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/graphics/vulkan/internal/vulkan-surface.h>
#include <dali/graphics/vulkan/internal/vulkan-debug.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/integration-api/graphics/vulkan/vk-surface-factory.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

using Dali::Integration::Graphics::Vulkan::VkSurfaceFactory;

Surface::Surface( Graphics& graphics, std::unique_ptr< SurfaceFactory > surfaceFactory )
: mGraphics( &graphics ),
  mSurfaceFactory( std::move( surfaceFactory ) ),
  mVulkanSurfaceFactory( dynamic_cast<VkSurfaceFactory*>( mSurfaceFactory.get() ) )
{
}

Surface::~Surface() = default;

vk::SurfaceKHR Surface::GetVkHandle() const
{
  return mSurface;
}

const vk::SurfaceCapabilitiesKHR& Surface::GetCapabilities() const
{
  return mCapabilities;
}

bool Surface::OnDestroy()
{
  auto instance = mGraphics->GetInstance();
  auto surface = mSurface;
  auto allocator = &mGraphics->GetAllocator();

  mGraphics->DiscardResource( [ instance, surface, allocator ]() {
    DALI_LOG_INFO( gVulkanFilter, Debug::General, "Invoking deleter function: surface->%p\n",
                   static_cast< void* >( surface ) )
    instance.destroySurfaceKHR( surface, allocator );
  } );

  return false;
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
