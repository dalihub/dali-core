#ifndef DALI_INTEGRATION_GRAPHICS_INTERFACE_H
#define DALI_INTEGRATION_GRAPHICS_INTERFACE_H

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
 */

#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/core-enumerations.h>

namespace Dali
{

namespace Internal
{
namespace Adaptor
{
class EnvironmentOptions;
}
}

namespace Integration
{
namespace Graphics
{
//class SurfaceFactory;

/**
 * Graphics interface
 */
class DALI_CORE_API GraphicsInterface
{
public:
  /**
   * Constructor
   */
  GraphicsInterface()
  : mDepthBufferRequired( Integration::DepthBufferAvailable::FALSE ),
    mStencilBufferRequired( Integration::StencilBufferAvailable::FALSE )
  {
  };

  /**
   * Initialize the graphics interface
   * @param[in]  environmentOptions  The environment options.
   */
  virtual void Initialize(Internal::Adaptor::EnvironmentOptions& environmentOptions ) = 0;

  /**
   * Create the  Graphics Factory implementation
   */
  virtual void Create() = 0;

  /**
   * Destroy the Graphics Factory implementation
   */
  virtual void Destroy() = 0;

  /**
   * Lifecycle event for pausing application
   */
  virtual void Pause() = 0;

  /**
   * Lifecycle event for resuming application
   */
  virtual void Resume() = 0;

  /**
   * @param surfaceFactory
   * @return
   */
  //virtual std::unique_ptr< SurfaceInterface > CreateSurface( SurfaceFactory& surfaceFactory ) = 0;

  /**
   * Prerender
   */
  virtual void PreRender() = 0;

  /*
   * Postrender
   */
  virtual void PostRender() = 0;

  /**
   * Get whether the depth buffer is required
   * @return TRUE if the depth buffer is required
   */
  Integration::DepthBufferAvailable& GetDepthBufferRequired()
  {
    return mDepthBufferRequired;
  };

  /**
   * Get whether the stencil buffer is required
   * @return TRUE if the stencil buffer is required
   */
  Integration::StencilBufferAvailable GetStencilBufferRequired()
  {
    return mStencilBufferRequired;
  };

protected:
  // No destruction through this interface
  ~GraphicsInterface() = default;

protected:
  Integration::DepthBufferAvailable mDepthBufferRequired;       ///< Whether the depth buffer is required
  Integration::StencilBufferAvailable mStencilBufferRequired;   ///< Whether the stencil buffer is required
};

} // namespace Graphics
} // namespace Integration
} // namespace Dali

#endif //DALI_INTEGRATION_GRAPHICS_INTERFACE_H
