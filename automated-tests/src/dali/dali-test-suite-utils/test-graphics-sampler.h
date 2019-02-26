#ifndef TEST_GRAPHICS_SAMPLER_H
#define TEST_GRAPHICS_SAMPLER_H

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

#include <dali/graphics-api/graphics-api-sampler.h>
#include <dali/graphics-api/graphics-api-types.h>

namespace Test
{
class GraphicsController;

struct GraphicsSamplerCreateInfo
{
  Dali::Graphics::SamplerAddressMode mAddressModeU {};
  Dali::Graphics::SamplerAddressMode mAddressModeW {};
  Dali::Graphics::SamplerAddressMode mAddressModeV {};
  Dali::Graphics::SamplerFilter      mMinFilter {};
  Dali::Graphics::SamplerFilter      mMagFilter {};
  Dali::Graphics::SamplerMipmapMode  mMipmapMode {};
  Dali::Graphics::CompareOp          mCompareOp {};
  float                              mMaxAnisotropy { 0.0f };
  float                              mMinLod { 0.0f };
  float                              mMaxLod { 0.0f };
  bool                               mCompareEnable {};
  bool                               mAnisotropyEnable { false };
  bool                               mUnnormalizedCoordinates { false };
};

class GraphicsSampler : public Dali::Graphics::Sampler
{
public:
  explicit GraphicsSampler(GraphicsController& controller, GraphicsSamplerCreateInfo createInfo);

  ~GraphicsSampler() override;

public:
  GraphicsController& mController;
  GraphicsSamplerCreateInfo mCreateInfo;
};

} // namespace Test

#endif // TEST_GRAPHICS_SAMPLER_H
