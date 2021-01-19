/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

#include "test-graphics-sampler.h"

namespace Dali
{
std::map<uint32_t, GLint> TestGraphicsSampler::mParamCache;

TestGraphicsSampler::TestGraphicsSampler(TestGlAbstraction& glAbstraction, const Graphics::SamplerCreateInfo& createInfo)
: mGlAbstraction(glAbstraction),
  mCreateInfo(createInfo)
{
}

GLint TestGraphicsSampler::FilterModeToGL(Graphics::SamplerFilter filterMode)
{
  switch(filterMode)
  {
    case Graphics::SamplerFilter::NEAREST:
    {
      return GL_NEAREST;
    }
    case Graphics::SamplerFilter::LINEAR:
    {
      return GL_LINEAR;
    }
  }
  return GL_LINEAR;
}

GLint TestGraphicsSampler::FilterModeToGL(Graphics::SamplerFilter filterMode, Graphics::SamplerMipmapMode mipmapMode)
{
  if(filterMode == Graphics::SamplerFilter::NEAREST)
  {
    switch(mipmapMode)
    {
      case Graphics::SamplerMipmapMode::NONE:
        return GL_NEAREST;
      case Graphics::SamplerMipmapMode::NEAREST:
        return GL_NEAREST_MIPMAP_NEAREST;
      case Graphics::SamplerMipmapMode::LINEAR:
        return GL_NEAREST_MIPMAP_LINEAR;
    }
  }
  else
  {
    switch(mipmapMode)
    {
      case Graphics::SamplerMipmapMode::NONE:
        return GL_LINEAR;
      case Graphics::SamplerMipmapMode::NEAREST:
        return GL_LINEAR_MIPMAP_NEAREST;
      case Graphics::SamplerMipmapMode::LINEAR:
        return GL_LINEAR_MIPMAP_LINEAR;
    }
  }
  return GL_LINEAR;
}

/**
 * @brief Convert from a WrapMode to its corresponding GL enumeration
 * @param[in] wrapMode The wrap mode
 * @param[in] defaultWrapMode The mode to use if WrapMode is Default
 * @return The equivalent GL wrap mode
 */
GLint TestGraphicsSampler::WrapModeToGL(Graphics::SamplerAddressMode wrapMode)
{
  switch(wrapMode)
  {
    case Graphics::SamplerAddressMode::CLAMP_TO_EDGE:
    {
      return GL_CLAMP_TO_EDGE;
    }
    case Graphics::SamplerAddressMode::CLAMP_TO_BORDER:
    {
      return GL_CLAMP_TO_EDGE;
    }
    case Graphics::SamplerAddressMode::REPEAT:
    {
      return GL_REPEAT;
    }
    case Graphics::SamplerAddressMode::MIRRORED_REPEAT:
    {
      return GL_MIRRORED_REPEAT;
    }
    case Graphics::SamplerAddressMode::MIRROR_CLAMP_TO_EDGE:
    {
      return GL_MIRRORED_REPEAT;
    }
  }
  return GL_REPEAT;
}

void TestGraphicsSampler::Apply(GLuint target)
{
  SetTexParameter(mGlAbstraction, target, GL_TEXTURE_MIN_FILTER, FilterModeToGL(mCreateInfo.minFilter, mCreateInfo.mipMapMode));
  SetTexParameter(mGlAbstraction, target, GL_TEXTURE_MAG_FILTER, FilterModeToGL(mCreateInfo.magFilter));
  SetTexParameter(mGlAbstraction, target, GL_TEXTURE_WRAP_S, WrapModeToGL(mCreateInfo.addressModeU));
  SetTexParameter(mGlAbstraction, target, GL_TEXTURE_WRAP_T, WrapModeToGL(mCreateInfo.addressModeV));
  if(target == GL_TEXTURE_CUBE_MAP)
  {
    TestGraphicsSampler::SetTexParameter(mGlAbstraction, target, GL_TEXTURE_WRAP_R, WrapModeToGL(mCreateInfo.addressModeW));
  }
}

uint32_t TestGraphicsSampler::GetTexParamHash(TestGlAbstraction& glAbstraction, GLuint target, GLenum pname)
{
  uint32_t targetFlags = 0;
  switch(target)
  {
    case GL_TEXTURE_2D:
      targetFlags = 0x01;
      break;
    case GL_TEXTURE_CUBE_MAP:
      targetFlags = 0x02;
      break;
    default:
      targetFlags = 0x03;
      break;
  }
  switch(pname)
  {
    case GL_TEXTURE_WRAP_S:
      targetFlags |= (0x01) << 2;
      break;
    case GL_TEXTURE_WRAP_T:
      targetFlags |= (0x02) << 2;
      break;
    case GL_TEXTURE_WRAP_R:
      targetFlags |= (0x03) << 2;
      break;
    case GL_TEXTURE_MAG_FILTER:
      targetFlags |= (0x04) << 2;
      break;
    case GL_TEXTURE_MIN_FILTER:
      targetFlags |= (0x05) << 2;
      break;
    default:
      targetFlags |= (0x07) << 2;
      break;
  }
  auto& textures = glAbstraction.GetBoundTextures(glAbstraction.GetActiveTextureUnit());
  targetFlags |= (textures.back() << 5);

  return targetFlags;
}

void TestGraphicsSampler::SetTexParameter(TestGlAbstraction& glAbstraction, GLuint target, GLenum pname, GLint value)
{
  // Works on the currently active texture

  uint32_t hash = GetTexParamHash(glAbstraction, target, pname);

  if(mParamCache.find(hash) != mParamCache.end())
  {
    if(mParamCache[hash] != value)
    {
      mParamCache[hash] = value;
      glAbstraction.TexParameteri(target, pname, value);
    }
  }
  else
  {
    mParamCache[hash] = value;
    glAbstraction.TexParameteri(target, pname, value);
  }
}

} // namespace Dali
