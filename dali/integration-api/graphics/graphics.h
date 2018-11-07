#ifndef DALI_INTEGRATION_GRAPHICS_H
#define DALI_INTEGRATION_GRAPHICS_H

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
#include <dali/integration-api/graphics/graphics-interface.h>

// EXTERNAL INCLUDES
#include <memory>

#define EXPORT_API __attribute__ ((visibility ("default")))

namespace Dali
{
namespace Graphics
{
namespace API
{
class Controller;
} // API

// frame buffer id
using FBID = int32_t;

namespace Vulkan
{
class Graphics;
} // Vulkan
using GraphicsImpl = Vulkan::Graphics;

} // Graphics

namespace Integration
{
namespace Graphics
{
class SurfaceFactory;
struct EXPORT_API Surface
{
  Surface( Dali::Graphics::GraphicsImpl* graphicsImpl, Dali::Graphics::FBID fbid );
  ~Surface();
  Dali::Graphics::GraphicsImpl* mGraphicsImpl;
  Dali::Graphics::FBID frambufferId;
};

enum class DepthStencilMode
{
  /**
   * No depth/stencil at all
   */
  NONE,

  /**
   * Optimal depth ( chosen by the implementation )
   */
  DEPTH_OPTIMAL,

  /**
   * Optimal depth and stencil ( chosen by the implementation )
   */
  DEPTH_STENCIL_OPTIMAL,

  /**
   * Depth and stencil with explicit format set in depthStencilFormat
   */
  DEPTH_STENCIL_EXPLICIT,
};

enum class SwapchainBufferingMode
{
  OPTIMAL = 0,

  DOUBLE_BUFFERING = 2,

  TRIPLE_BUFFERING = 3,
};

struct GraphicsCreateInfo
{
  uint32_t                    surfaceWidth;
  uint32_t                    surfaceHeight;
  DepthStencilMode            depthStencilMode;
  SwapchainBufferingMode      swapchainBufferingMode;
};

/**
 * Graphics implementation class
 */
class EXPORT_API Graphics final : public GraphicsInterface
{
public:

  Graphics( const GraphicsCreateInfo& info,
            Integration::DepthBufferAvailable depthBufferAvailable,
            Integration::StencilBufferAvailable stencilBufferRequired );

  ~Graphics();

  void Initialize() override;

  /**
   *
   * @param surfaceFactory
   * @return
   */
  std::unique_ptr<Surface> CreateSurface( SurfaceFactory& surfaceFactory );

  /**
   * When creating Graphics at least one surfaceFactory must be supplied ( no headless mode )
   * @param surfaceFactory the surface factory.
   *
   * @note This should be called from the render thread
   */
  void Create() override;

  void Destroy() override;

  /**
   * Lifecycle event for pausing application
   */
  void Pause() override;

  /**
   * Lifecycle event for resuming application
   */
  void Resume() override;

  /**
   * Prerender
   */
  void PreRender() override;

  /*
   * Postrender
   */
  void PostRender() override;

  /**
   * Returns controller object
   * @return
   */
  Dali::Graphics::API::Controller& GetController();

  /*
   * Surface resized
   */
  void SurfaceResized( unsigned int width, unsigned int height );

  // this function is used only by the standalone test
  template <class T>
  T& GetImplementation() const
  {
    return static_cast<T&>(*mGraphicsImpl.get());
  }

  GraphicsCreateInfo& GetCreateInfo()
  {
    return mCreateInfo;
  }

private:
  GraphicsCreateInfo mCreateInfo;
  std::unique_ptr<Dali::Graphics::GraphicsImpl> mGraphicsImpl;
};

namespace GraphicsFactory
{
/**
 * Creates new instance of Graphics integration object
 * @param info
 * @return
 */
std::unique_ptr<Dali::Integration::Graphics::Graphics> Create( const GraphicsCreateInfo& info );
}

/**
 * fixme: dummy function to make sure the static library won't be discarded entirely during linking
 */
EXPORT_API void IncludeThisLibrary();

} // Namespace Graphics
} // Namespace Integration
} // Namespace Dali

#endif // DALI_INTEGRATION_GRAPHICS_H
