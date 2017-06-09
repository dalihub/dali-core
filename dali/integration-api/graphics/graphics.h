#ifndef DALI_INTEGRATION_GRAPHICS_H
#define DALI_INTEGRATION_GRAPHICS_H

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

// EXTERNAL INCLUDES
#include <memory>

namespace Dali
{
namespace Graphics
{

// frame buffer id
using FBID = uint32_t;

namespace Vulkan
{
class Graphics;
} // Vulkan
using GraphicsImpl = Vulkan::Graphics;

} // Graphics

#define DALI_IMPORT_API __attribute__ ((visibility ("default")))

namespace Integration
{
namespace Graphics
{
class SurfaceFactory;
class DALI_IMPORT_API Graphics final
{
public:


  Graphics();
  ~Graphics();

  /**
   *
   * @param surfaceFactory
   * @return
   */
  Dali::Graphics::FBID CreateSurface( std::unique_ptr<Dali::Integration::Graphics::SurfaceFactory> surfaceFactory );


  /**
   * When creating Graphics at least one surfaceFactory must be supplied ( no headless mode )
   * @param surfaceFactory
   * @return
   */
  Dali::Graphics::FBID Create( std::unique_ptr<Dali::Integration::Graphics::SurfaceFactory> surfaceFactory );

  /**
   * Prerender
   */
  void PreRender( Dali::Graphics::FBID framebufferId = 0u );

  /*
   * Postrender
   */
  void PostRender( Dali::Graphics::FBID framebufferId = 0u );

private:

  std::unique_ptr<Dali::Graphics::GraphicsImpl> mGraphicsImpl;
};

/**
 * fixme: dummy function to make sure the static library won't be discarded entirely during linking
 */
void IncludeThisLibrary();

} // Namespace Graphics
} // Namespace Integration
} // Namespace Dali

#endif // DALI_INTEGRATION_GRAPHICS_H
