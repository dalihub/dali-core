#ifndef DALI_TEST_GRAPHICS_SAMPLER_H
#define DALI_TEST_GRAPHICS_SAMPLER_H

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

#include <dali/graphics-api/graphics-sampler-create-info.h>
#include <dali/graphics-api/graphics-sampler.h>
#include <dali/graphics-api/graphics-types.h>
#include <dali/integration-api/gl-defines.h>
#include <cstdint>
#include <map>
#include "test-gl-abstraction.h"

namespace Dali
{
/**
 * Maintains a cache of parameters per texture/texture target
 */
class TestGraphicsSampler : public Graphics::Sampler
{
public:
  TestGraphicsSampler(TestGlAbstraction& glAbstraction, const Graphics::SamplerCreateInfo& createInfo);

  /**
   * Apply sampler to target texture.
   */
  void Apply(GLuint target);

  static void SetTexParameter(TestGlAbstraction& glAbstraction, GLuint target, GLenum pname, GLint value);

  static GLint FilterModeToGL(Graphics::SamplerFilter filterMode);

  static GLint FilterModeToGL(Graphics::SamplerFilter filterMode, Graphics::SamplerMipmapMode mipmapMode);

  /**
   * @brief Convert from a WrapMode to its corresponding GL enumeration
   * @param[in] wrapMode The wrap mode
   * @param[in] defaultWrapMode The mode to use if WrapMode is Default
   * @return The equivalent GL wrap mode
   */
  static GLint WrapModeToGL(Graphics::SamplerAddressMode wrapMode);

  static uint32_t GetTexParamHash(TestGlAbstraction& glAbstraction, GLuint target, GLenum pname);

public:
  static std::map<uint32_t, GLint> mParamCache;

  TestGlAbstraction&          mGlAbstraction;
  Graphics::SamplerCreateInfo mCreateInfo;
};

} // namespace Dali

#endif //DALI_TEST_GRAPHICS_SAMPLER_H
