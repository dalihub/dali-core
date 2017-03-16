#ifndef DALI_GRAPHICS_TEST_IMPLEMENTATION_H
#define DALI_GRAPHICS_TEST_IMPLEMENTATION_H

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
#include <dali/graphics-api/graphics-api-base-factory.h>
#include <dali/graphics-api/graphics-api-controller.h>
#include <dali/graphics/graphics-object-owner.h>

namespace Dali
{

namespace Graphics
{
namespace Test
{

template< typename T, typename BaseT >
struct FactoryImpl : public API::BaseFactory< BaseT >
{
  FactoryImpl() = default;

  virtual typename API::BaseFactory< BaseT >::PointerType Create() const override
  {
    return typename API::BaseFactory< BaseT >::PointerType{new T{}};
  }

  FactoryImpl(const FactoryImpl&) = delete;
  FactoryImpl& operator=(const FactoryImpl&) = delete;

  FactoryImpl(FactoryImpl&&) = default;
  FactoryImpl& operator=(FactoryImpl&&) = default;

  virtual ~FactoryImpl() = default;
};

struct Shader final : public API::Shader
{
};
using ShaderFactory = FactoryImpl< Shader, API::Shader >;

struct Texture final : public API::Texture
{
};
using TextureFactory = FactoryImpl< Texture, API::Texture >;

struct TextureSet final : public API::TextureSet
{
  virtual void AddTexture(const API::Accessor<API::Texture>& ) override {}
};
using TextureSetFactory = FactoryImpl< TextureSet, API::TextureSet >;

struct DynamicBuffer final : public API::DynamicBuffer
{
};
using DynamicBufferFactory = FactoryImpl< DynamicBuffer, API::DynamicBuffer >;

struct StaticBuffer final : public API::StaticBuffer
{
};
using StaticBufferFactory = FactoryImpl< StaticBuffer, API::StaticBuffer >;

struct Sampler final : public API::Sampler
{
};
using SamplerFactory = FactoryImpl< Sampler, API::Sampler >;

struct Framebuffer final : public API::Framebuffer
{
};
using FramebufferFactory = FactoryImpl< Framebuffer, API::Framebuffer >;

class Controller final : public API::Controller
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

private:
  ObjectOwner< API::Shader >        mShaders;
  ObjectOwner< API::Texture >       mTextures;
  ObjectOwner< API::TextureSet >    mTextureSets;
  ObjectOwner< API::DynamicBuffer > mDynamicBuffers;
  ObjectOwner< API::StaticBuffer >  mStaticBuffers;
  ObjectOwner< API::Sampler >       mSamplers;
  ObjectOwner< API::Framebuffer >   mFramebuffers;
};

} // namespace Test
} // namespace Graphics
} // namespace Dali

#endif // DALI_GRAPHICS_TEST_IMPLEMENTATION_H
