#ifndef DALI_INTERNAL_RENDER_SAMPLER_H
#define DALI_INTERNAL_RENDER_SAMPLER_H

/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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

#include <dali/graphics-api/graphics-controller.h>
#include <dali/public-api/actors/sampling.h>
#include <dali/public-api/rendering/sampler.h>

namespace Dali
{
namespace Internal
{
namespace Render
{
/**
 * The sampler class holds the min/mag filter and texture wrap modes.
 * It's graphics counterpart is only created when needed, and also
 * only created when the filters and wrap modes are not default
 * values.
 */
struct Sampler
{
  using FilterMode = Dali::FilterMode::Type;
  using WrapMode   = Dali::WrapMode::Type;

  /**
   * Constructor
   */
  Sampler();

  /**
   * Destructor
   */
  ~Sampler() = default;

  bool operator==(const Sampler& rhs) const
  {
    return ((mMinificationFilter == rhs.mMinificationFilter) &&
            (mMagnificationFilter == rhs.mMagnificationFilter) &&
            (mSWrapMode == rhs.mSWrapMode) &&
            (mTWrapMode == rhs.mTWrapMode) &&
            (mRWrapMode == rhs.mRWrapMode));
  }

  bool operator!=(const Sampler& rhs) const
  {
    return !(*this == rhs);
  }

  /**
   * Returns Graphics API sampler object
   * @return Pointer to API sampler or nullptr
   */
  const Dali::Graphics::Sampler* GetGraphicsObject();

  [[nodiscard]] static inline Graphics::SamplerAddressMode GetGraphicsSamplerAddressMode(WrapMode mode)
  {
    switch(mode)
    {
      case WrapMode::REPEAT:
        return Graphics::SamplerAddressMode::REPEAT;
      case WrapMode::MIRRORED_REPEAT:
        return Graphics::SamplerAddressMode::MIRRORED_REPEAT;
      case WrapMode::CLAMP_TO_EDGE:
      case WrapMode::DEFAULT:
        return Graphics::SamplerAddressMode::CLAMP_TO_EDGE;
    }
    return {};
  }

  [[nodiscard]] static inline Graphics::SamplerMipmapMode GetGraphicsSamplerMipmapMode(FilterMode mode)
  {
    switch(mode)
    {
      case FilterMode::LINEAR_MIPMAP_LINEAR:
      case FilterMode::NEAREST_MIPMAP_LINEAR:
        return Graphics::SamplerMipmapMode::LINEAR;
      case FilterMode::NEAREST_MIPMAP_NEAREST:
      case FilterMode::LINEAR_MIPMAP_NEAREST:
        return Graphics::SamplerMipmapMode::NEAREST;
      default:
        return Graphics::SamplerMipmapMode::NONE;
    }
    return {};
  }

  [[nodiscard]] static inline Graphics::SamplerFilter GetGraphicsFilter(FilterMode mode)
  {
    switch(mode)
    {
      case FilterMode::LINEAR:
      case FilterMode::LINEAR_MIPMAP_LINEAR:
      case FilterMode::LINEAR_MIPMAP_NEAREST:
        return Graphics::SamplerFilter::LINEAR;
      case FilterMode::NEAREST:
      case FilterMode::NEAREST_MIPMAP_LINEAR:
      case FilterMode::NEAREST_MIPMAP_NEAREST:
        return Graphics::SamplerFilter::NEAREST;
      case FilterMode::DEFAULT:
        return Graphics::SamplerFilter::LINEAR;
      case FilterMode::NONE:
        return Graphics::SamplerFilter::NEAREST;
      default:
        return {};
    }
    return {};
  }

  /**
   * Sets the filter modes for an existing sampler
   * @param[in] sampler The sampler
   * @param[in] minFilterMode The filter to use under minification
   * @param[in] magFilterMode The filter to use under magnification
   */
  inline void SetFilterMode(Dali::FilterMode::Type minFilterMode, Dali::FilterMode::Type magFilterMode)
  {
    mMinificationFilter  = minFilterMode;
    mMagnificationFilter = magFilterMode;
    mIsDirty             = true;
  }

  /**
   * Sets the wrap mode for an existing sampler
   * @param[in] sampler The sampler
   * @param[in] rWrapMode Wrapping mode in z direction
   * @param[in] sWrapMode Wrapping mode in x direction
   * @param[in] tWrapMode Wrapping mode in y direction
   */
  inline void SetWrapMode(Dali::WrapMode::Type rWrapMode, Dali::WrapMode::Type sWrapMode, Dali::WrapMode::Type tWrapMode)
  {
    mRWrapMode = rWrapMode;
    mSWrapMode = sWrapMode;
    mTWrapMode = tWrapMode;
    mIsDirty   = true;
  }

  /**
   * Check if the sampler has default values
   */
  [[nodiscard]] inline bool IsDefaultSampler() const
  {
    return (mMagnificationFilter == FilterMode::DEFAULT &&
            mMinificationFilter == FilterMode::DEFAULT &&
            mSWrapMode == WrapMode::DEFAULT &&
            mTWrapMode == WrapMode::DEFAULT &&
            mRWrapMode == WrapMode::DEFAULT);
  }

  void Initialize(Graphics::Controller& graphicsController);

  Graphics::Sampler* CreateGraphicsObject();

  Graphics::Controller*                  mGraphicsController;
  Graphics::UniquePtr<Graphics::Sampler> mGraphicsSampler;

  FilterMode mMinificationFilter : 4;  ///< The minify filter
  FilterMode mMagnificationFilter : 4; ///< The magnify filter
  WrapMode   mSWrapMode : 4;           ///< The horizontal wrap mode
  WrapMode   mTWrapMode : 4;           ///< The vertical wrap mode
  WrapMode   mRWrapMode : 4;           ///< The vertical wrap mode
  bool       mIsDirty : 1;             ///< If parameters have been set thru API
};

} // namespace Render

} // namespace Internal

} // namespace Dali

#endif //  DALI_INTERNAL_RENDER_SAMPLER_H
