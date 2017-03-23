#ifndef DALI_GRAPHICS_API_CONTROLLER_H
#define DALI_GRAPHICS_API_CONTROLLER_H

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <dali/graphics-api/graphics-api-accessor.h>
#include <dali/graphics-api/graphics-api-base-factory.h>
#include <dali/graphics-api/graphics-api-dynamic-buffer.h>
#include <dali/graphics-api/graphics-api-framebuffer.h>
#include <dali/graphics-api/graphics-api-sampler.h>
#include <dali/graphics-api/graphics-api-shader.h>
#include <dali/graphics-api/graphics-api-static-buffer.h>
#include <dali/graphics-api/graphics-api-texture-set.h>
#include <dali/graphics-api/graphics-api-texture.h>
#include <dali/graphics-api/graphics-api-texture-factory.h>

namespace Dali
{
namespace Graphics
{
namespace API
{

/**
 * @brief Interface class for Manager types in the graphics API.
 */
class Controller
{
public:
  /**
   * @brief Create a new object
   */
  virtual Accessor< Shader > CreateShader(const BaseFactory< Shader >& factory) = 0;

  /**
   * @brief Create a new object
   */
  virtual Accessor< Texture > CreateTexture(const BaseFactory< Texture >& factory) = 0;

  /**
   * @brief Create a new object
   */
  virtual Accessor< TextureSet > CreateTextureSet(const BaseFactory< TextureSet >& factory) = 0;

  /**
   * @brief Create a new object
   */
  virtual Accessor< DynamicBuffer > CreateDynamicBuffer(
      const BaseFactory< DynamicBuffer >& factory) = 0;

  /**
   * @brief Create a new object
   */
  virtual Accessor< StaticBuffer > CreateStaticBuffer(
      const BaseFactory< StaticBuffer >& factory) = 0;

  /**
   * @brief Create a new object
   */
  virtual Accessor< Sampler > CreateSampler(const BaseFactory< Sampler >& factory) = 0;

  /**
   * @brief Create a new object
   */
  virtual Accessor< Framebuffer > CreateFramebuffer(const BaseFactory< Framebuffer >& factory) = 0;

  /**
   * @brief Get a render list
   */
  virtual void GetRenderItemList() = 0;

public:
  // not copyable
  Controller(const Controller&) = delete;
  Controller& operator=(const Controller&) = delete;

  virtual ~Controller() = default;

protected:
  // derived types should not be moved direcly to prevent slicing
  Controller(Controller&&) = default;
  Controller& operator=(Controller&&) = default;

  /**
   * Objects of this type should not directly.
   */
  Controller() = default;
};

} // namespace API
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_API_CONTROLLER_H
