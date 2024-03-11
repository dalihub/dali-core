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
std::map<uint32_t, int32_t> TestGraphicsSampler::mParamCache;

TestGraphicsSampler::TestGraphicsSampler(const Graphics::SamplerCreateInfo& createInfo)
: mCreateInfo(createInfo)
{
}

int32_t TestGraphicsSampler::FilterModeToGL(Graphics::SamplerFilter filterMode)
{
  return 0;
}

int32_t TestGraphicsSampler::FilterModeToGL(Graphics::SamplerFilter filterMode, Graphics::SamplerMipmapMode mipmapMode)
{
  return 0;
}

/**
 * @brief Convert from a WrapMode to its corresponding GL enumeration
 * @param[in] wrapMode The wrap mode
 * @param[in] defaultWrapMode The mode to use if WrapMode is Default
 * @return The equivalent GL wrap mode
 */
int TestGraphicsSampler::WrapModeToGL(Graphics::SamplerAddressMode wrapMode)
{
  return 0;
}

void TestGraphicsSampler::Apply(uint32_t target)
{
}

uint32_t TestGraphicsSampler::GetTexParamHash(uint32_t target, uint32_t pname)
{
  return 0;
}

void TestGraphicsSampler::SetTexParameter(uint32_t target, uint32_t pname, int32_t value)
{
  // Works on the currently active texture

  uint32_t hash = GetTexParamHash(target, pname);

  if(mParamCache.find(hash) != mParamCache.end())
  {
    if(mParamCache[hash] != value)
    {
      mParamCache[hash] = value;
      //glAbstraction.TexParameteri(target, pname, value);
    }
  }
  else
  {
    mParamCache[hash] = value;
    //glAbstraction.TexParameteri(target, pname, value);
  }
}

} // namespace Dali
