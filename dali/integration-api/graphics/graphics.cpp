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

#include <dali/graphics/vulkan/vulkan-types.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-surface.h>
#include <dali/graphics/vulkan/vulkan-framebuffer.h>
#include <dali/graphics-api/graphics-api-controller.h>
#include <dali/integration-api/graphics/graphics.h>

extern "C" std::vector<uint32_t> GraphicsGetBuiltinShader( const std::string& tag );

namespace Dali
{
/// fixme: substituting directly the vulkan implementation
namespace Graphics
{
using GraphicsImpl = Dali::Graphics::Vulkan::Graphics;
}

namespace Integration
{
namespace Graphics
{
using Swapchain = Dali::Graphics::Vulkan::Swapchain;
using SwapchainRef = Dali::Graphics::Vulkan::RefCountedSwapchain;

Graphics::Graphics()
{
  // create device
  auto impl = Dali::Graphics::MakeUnique<Dali::Graphics::GraphicsImpl>();

  impl->Create();

  mGraphicsImpl = std::move(impl);
}

Graphics::~Graphics()
{

}

Dali::Graphics::FBID Graphics::Create(
  std::unique_ptr<Dali::Integration::Graphics::SurfaceFactory> surfaceFactory)
{
  //@todo do we really need to have a surface that early???

  // create surface
  auto retval = mGraphicsImpl->CreateSurface(std::move(surfaceFactory));

  // create device
  mGraphicsImpl->CreateDevice();

  // create swapchain from surface
  auto surface = mGraphicsImpl->GetSurface( retval );

  // create swapchain
  mGraphicsImpl->CreateSwapchainForSurface( surface );

  return retval;
}

Dali::Graphics::FBID Graphics::CreateSurface(
  std::unique_ptr<Dali::Integration::Graphics::SurfaceFactory> surfaceFactory)
{
  return 0u;
}

void Graphics::PreRender(Dali::Graphics::FBID framebufferId)
{
  assert(framebufferId != 0 && "Invalid FBID!");
  auto swapchain = mGraphicsImpl->GetSwapchainForFBID( framebufferId );
  swapchain->AcquireNextFramebuffer();
}

Dali::Graphics::API::Controller& Graphics::GetController()
{
  return mGraphicsImpl->GetController();
}

/*
 * Postrender
 */
void Graphics::PostRender(Dali::Graphics::FBID framebufferId)
{
  auto swapchain = mGraphicsImpl->GetSwapchainForFBID( framebufferId );
  swapchain->Present();
}

void IncludeThisLibrary()
{
// dummy function to create linker dependency
  GraphicsGetBuiltinShader("");
}

} // Namespace Graphics
} // Namespace Integration
} // Namespace Dali
