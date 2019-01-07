#ifndef TEST_GRAPHICS_H
#define TEST_GRAPHICS_H

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

// INTERNAL INCLUDES
#include <dali/integration-api/graphics/graphics-interface.h>

// EXTERNAL INCLUDES
#include <memory>

#define EXPORT_API __attribute__ ((visibility ("default")))

namespace Test
{
class Controller;

/**
 * Graphics implementation class
 */
class Graphics : public Dali::Integration::Graphics::GraphicsInterface
{
public:

  Graphics( const Dali::Integration::Graphics::GraphicsCreateInfo& info,
            Dali::Integration::DepthBufferAvailable depthBufferAvailable,
            Dali::Integration::StencilBufferAvailable stencilBufferRequired );

  ~Graphics();

  void Initialize() override;

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
  Dali::Graphics::Controller& GetController();

  /*
   * Surface resized
   */
  void SurfaceResized( unsigned int width, unsigned int height );

  Dali::Integration::Graphics::GraphicsCreateInfo& GetCreateInfo()
  {
    return mCreateInfo;
  }

private:
  Dali::Integration::Graphics::GraphicsCreateInfo mCreateInfo;

  struct Impl;
  Impl* mImpl;
};


} // Namespace Test

#endif // TEST_GRAPHICS_H
