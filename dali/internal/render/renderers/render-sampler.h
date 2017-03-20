#ifndef DALI_INTERNAL_RENDER_SAMPLER_H
#define DALI_INTERNAL_RENDER_SAMPLER_H

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
 */

#include <dali/public-api/actors/sampling.h>
#include <dali/public-api/rendering/sampler.h>

namespace Dali
{
namespace Internal
{
namespace Render
{

struct Sampler
{

  typedef Dali::FilterMode::Type FilterMode;
  typedef Dali::WrapMode::Type   WrapMode;

  /**
   * Constructor
   */
  Sampler()
  :mMinificationFilter(FilterMode::DEFAULT),
   mMagnificationFilter(FilterMode::DEFAULT),
   mSWrapMode(WrapMode::DEFAULT),
   mTWrapMode(WrapMode::DEFAULT),
   mRWrapMode(WrapMode::DEFAULT)
  {}

  /**
   * Destructor
   */
  ~Sampler()
  {}

  bool operator==(const Sampler& rhs) const
  {
    return ( ( mMinificationFilter == rhs.mMinificationFilter ) &&
             ( mMagnificationFilter == rhs.mMagnificationFilter ) &&
             ( mSWrapMode == rhs.mSWrapMode ) &&
             ( mTWrapMode == rhs.mTWrapMode ) &&
             ( mRWrapMode == rhs.mRWrapMode ) );
  }

  bool operator!=(const Sampler& rhs) const
  {
    return !(*this == rhs);
  }

  FilterMode  mMinificationFilter   : 4;    ///< The minify filter
  FilterMode  mMagnificationFilter  : 4;    ///< The magnify filter
  WrapMode    mSWrapMode            : 4;    ///< The horizontal wrap mode
  WrapMode    mTWrapMode            : 4;    ///< The vertical wrap mode
  WrapMode    mRWrapMode            : 4;    ///< The vertical wrap mode
};

} // namespace Render

} // namespace Internal

} // namespace Dali


#endif //  DALI_INTERNAL_RENDER_SAMPLER_H
