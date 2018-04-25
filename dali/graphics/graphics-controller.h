#ifndef DALI_GRAPHICS_CONTROLLER_H
#define DALI_GRAPHICS_CONTROLLER_H

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

#include <dali/graphics-api/graphics-api-controller.h>
#include <dali/graphics/graphics-object-owner.h>

namespace Dali
{
namespace Graphics
{

/**
 * @brief Controller implementation of the graphics API
 */
class Controller : public API::Controller
{
public:
  virtual API::Accessor< API::Shader > CreateShader(
      const API::BaseFactory< API::Shader >& factory) override;

  virtual API::Accessor< API::Texture > CreateTexture(
      const API::BaseFactory< API::Texture >& factory) override;

  virtual API::Accessor< API::TextureSet > CreateTextureSet(
      const API::BaseFactory< API::TextureSet >& factory) override;

  virtual API::Accessor< API::DynamicBuffer > CreateDynamicBuffer(
      const API::BaseFactory< API::DynamicBuffer >& factory) override;

  virtual API::Accessor< API::StaticBuffer > CreateStaticBuffer(
      const API::BaseFactory< API::StaticBuffer >& factory) override;

  virtual API::Accessor< API::Sampler > CreateSampler(
      const API::BaseFactory< API::Sampler >& factory) override;

  virtual API::Accessor< API::Framebuffer > CreateFramebuffer(
      const API::BaseFactory< API::Framebuffer >& factory) override;

  virtual void GetRenderItemList() override;

public:
  Controller();

  Controller(const Controller&) = delete;
  Controller& operator=(const Controller&) = delete;

  Controller(Controller&&);
  Controller& operator=(Controller&&) = default;

  ~Controller() override;

protected:
  ObjectOwner< API::Shader >        mShaders;
  ObjectOwner< API::Texture >       mTextures;
  ObjectOwner< API::TextureSet >    mTextureSets;
  ObjectOwner< API::DynamicBuffer > mDynamicBuffers;
  ObjectOwner< API::StaticBuffer >  mStaticBuffers;
  ObjectOwner< API::Sampler >       mSamplers;
  ObjectOwner< API::Framebuffer >   mFramebuffers;
};

} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_CONTROLLER_H
