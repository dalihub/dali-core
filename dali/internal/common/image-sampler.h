#ifndef __DALI_IMAGE_SAMPLER_H__
#define __DALI_IMAGE_SAMPLER_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/sampling.h>

namespace Dali
{

namespace Internal
{

/**
 * ImageSampler represents a set of sampling settings that can be applied to a texture.
 */
namespace ImageSampler
{
  /**
   * @brief Pack the filter mode into a bitfield.
   *
   * @param[in] minify The minification filter.
   * @param[in] magnify The magnification filter.
   * @return Return the packed bitfield.
   */
   unsigned int PackBitfield( FilterMode::Type minify, FilterMode::Type magnify, WrapMode::Type uWrap = WrapMode::DEFAULT, WrapMode::Type vWrap = WrapMode::DEFAULT );

   /**
   * @brief Return the minification filter from a packed bitfield.
   *
   * @return Return the minification filter.
   */
   FilterMode::Type GetMinifyFilterMode( unsigned int bitfield );

   /**
   * @brief Return the magnification filter from a packed bitfield.
   *
   * @return Return the magnification filter.
   */
   FilterMode::Type GetMagnifyFilterMode( unsigned int bitfield );

   /**
    * @brief Return the wrap mode in x direction from a packed bitfield.
    *
    * @return Return the wrap mode.
    */
   WrapMode::Type GetUWrapMode( unsigned int bitfield );

   /**
    * @brief Return the wrap mode in y direction from a packed bitfield.
    *
    * @return Return the wrap mode.
    */
   WrapMode::Type GetVWrapMode( unsigned int bitfield );

} // namespace ImageSampler

} // namespace Internal

} // namespace Dali


#endif // __DALI_INTERNAL_IMAGE_SAMPLER_H__



