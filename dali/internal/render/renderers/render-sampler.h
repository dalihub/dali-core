#ifndef DALI_INTERNAL_RENDER_SAMPLER_H
#define DALI_INTERNAL_RENDER_SAMPLER_H

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/actors/sampling.h>
#include <dali/devel-api/rendering/sampler.h>
#include <dali/internal/render/renderers/render-sampler.h>

namespace Dali
{
namespace Internal
{
namespace Render
{
class Sampler
{
public:
  typedef Dali::FilterMode::Type FilterMode;
  typedef Dali::WrapMode::Type   WrapMode;

  /**
   * Constructor
   */
  Sampler()
  : mMinFilter( Dali::FilterMode::DEFAULT),
    mMagFilter( Dali::FilterMode::DEFAULT ),
    mUWrapMode( Dali::WrapMode::DEFAULT ),
    mVWrapMode( Dali::WrapMode::DEFAULT )
  {}

  /**
   * Destructor
   */
  virtual ~Sampler(){}

  /**
   * Set the filter modes for minify and magnify filters
   * @param[in] bufferIndex The buffer index to use
   * @param[in] minFilter The minify filter
   * @param[in] magFilter The magnify filter
   */
  void SetFilterMode(FilterMode minFilter, FilterMode magFilter )
  {
    mMinFilter = minFilter;
    mMagFilter = magFilter;
  }

  /**
   * @param[in] bufferIndex The buffer index to use
   */
  void SetWrapMode(WrapMode uWrap, WrapMode vWrap )
  {
    mUWrapMode = uWrap;
    mVWrapMode = vWrap;
  }

public:

  /**
   * Get the filter mode
   * @return The minify filter mode
   */
  virtual FilterMode GetMinifyFilterMode() const
  {
    return mMinFilter;
  }

  /**
   * Get the filter mode
   * @return The magnify filter mode
   */
  virtual FilterMode GetMagnifyFilterMode() const
  {
    return mMagFilter;
  }

  /**
   * Get the horizontal wrap mode
   * @return The horizontal wrap mode
   */
  virtual WrapMode GetUWrapMode() const
  {
    return mUWrapMode;
  }

  /**
   * Get the vertical wrap mode
   * @return The vertical wrap mode
   */
  virtual WrapMode GetVWrapMode() const
  {
    return mVWrapMode;
  }

private:

  FilterMode  mMinFilter;    ///< The minify filter
  FilterMode  mMagFilter;    ///< The magnify filter
  WrapMode  mUWrapMode;    ///< The horizontal wrap mode
  WrapMode  mVWrapMode;    ///< The vertical wrap mode
};

} // namespace Render



} // namespace Internal
} // namespace Dali


#endif //  DALI_INTERNAL_RENDER_SAMPLER_H
