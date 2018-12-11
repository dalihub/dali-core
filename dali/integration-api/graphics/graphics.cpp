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

#include <dali/graphics/vulkan/internal/vulkan-types.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/internal/vulkan-surface.h>
#include <dali/graphics/vulkan/internal/vulkan-framebuffer.h>
#include <dali/graphics-api/graphics-api-controller.h>
#include <dali/integration-api/graphics/graphics.h>
#include <dali/graphics/vulkan/internal/vulkan-swapchain.h>

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

Surface::Surface( Dali::Graphics::GraphicsImpl* graphicsImpl, Dali::Graphics::FBID fbid )
    : mGraphicsImpl( graphicsImpl ),
      frambufferId( fbid )
{
}

Surface::~Surface()
{
  if( auto surface = mGraphicsImpl->GetSurface( frambufferId ) )
  {
    mGraphicsImpl->DeviceWaitIdle();
    auto swapchain = mGraphicsImpl->GetSwapchainForFBID( frambufferId );
    swapchain->Destroy();
    surface->Destroy();
    mGraphicsImpl->CollectGarbage();
  }
}

using Dali::Integration::Graphics::SurfaceFactory;

Graphics::Graphics( const GraphicsCreateInfo& info,
                    Integration::DepthBufferAvailable depthBufferAvailable,
                    Integration::StencilBufferAvailable stencilBufferRequired )
: GraphicsInterface( depthBufferAvailable, stencilBufferRequired ),
  mCreateInfo( info )
{
  // create device
  auto impl = Dali::Graphics::MakeUnique<Dali::Graphics::GraphicsImpl>();

  mGraphicsImpl = std::move(impl);
}

Graphics::~Graphics() = default;


void Graphics::Initialize()
{
}

void Graphics::Create()
{
  mGraphicsImpl->Create();

  // create device
  mGraphicsImpl->CreateDevice();

  // initialise controller
  mGraphicsImpl->InitialiseController();
}

std::unique_ptr<Surface> Graphics::CreateSurface( SurfaceFactory& surfaceFactory)
{
  // create surface ( also takes surface factory ownership )
  auto retval = mGraphicsImpl->CreateSurface( surfaceFactory, mCreateInfo );

  // create swapchain from surface
  auto surface = mGraphicsImpl->GetSurface( retval );

  // create swapchain
  mGraphicsImpl->CreateSwapchainForSurface( surface );

  return std::unique_ptr<Surface>( new Surface( mGraphicsImpl.get(), retval ) );
}

void Graphics::Destroy()
{
}

void Graphics::Pause()
{
  mGraphicsImpl->GetController().Pause();
}

void Graphics::Resume()
{
  mGraphicsImpl->GetController().Resume();
}

void Graphics::PreRender()
{
  auto swapchain = mGraphicsImpl->GetSwapchainForFBID( 0u );
  swapchain->AcquireNextFramebuffer();
}

void Graphics::PostRender()
{
  auto swapchain = mGraphicsImpl->GetSwapchainForFBID( 0u );
  swapchain->Present();

  mGraphicsImpl->CollectGarbage();
}

Dali::Graphics::API::Controller& Graphics::GetController()
{
  return mGraphicsImpl->GetController();
}

void Graphics::SurfaceResized( unsigned int width, unsigned int height )
{
  mCreateInfo.surfaceWidth = width;
  mCreateInfo.surfaceHeight = height;

  mGraphicsImpl->SurfaceResized( width, height );
}

void IncludeThisLibrary()
{
  // dummy function to create linker dependency
  //GraphicsGetBuiltinShader("");
}

} // Namespace Graphics
} // Namespace Integration
} // Namespace Dali
