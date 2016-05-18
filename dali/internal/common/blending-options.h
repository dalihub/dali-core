#ifndef DALI_BLENDING_OPTIONS_H
#define DALI_BLENDING_OPTIONS_H

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
#include <dali/devel-api/rendering/renderer.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{

namespace Internal
{

// This is an optimization to avoid storing 6 separate blending values
struct BlendingOptions
{
  /**
   * Create some default blending options.
   */
  BlendingOptions();

  /**
   * Non-virtual destructor.
   */
  ~BlendingOptions();

  /**
   * Set the blending options.
   * @param[in] A bitmask of blending options.
   */
  void SetBitmask( unsigned int bitmask );

  /**
   * Retrieve the blending options as a bitmask.
   * @return A bitmask of blending options.
   */
  unsigned int GetBitmask() const;

  /**
   * @copydoc Dali::RenderableActor::SetBlendFunc()
   */
  void SetBlendFunc( BlendFactor::Type srcFactorRgb,   BlendFactor::Type destFactorRgb,
                     BlendFactor::Type srcFactorAlpha, BlendFactor::Type destFactorAlpha );

  /**
   * @copydoc Dali::RenderableActor::GetBlendFunc()
   */
  BlendFactor::Type GetBlendSrcFactorRgb() const;

  /**
   * @copydoc Dali::RenderableActor::GetBlendFunc()
   */
  BlendFactor::Type GetBlendDestFactorRgb() const;

  /**
   * @copydoc Dali::RenderableActor::GetBlendFunc()
   */
  BlendFactor::Type GetBlendSrcFactorAlpha() const;

  /**
   * @copydoc Dali::RenderableActor::GetBlendFunc()
   */
  BlendFactor::Type GetBlendDestFactorAlpha() const;

  /**
   * @copydoc Dali::RenderableActor::SetBlendEquation()
   */
  void SetBlendEquation( BlendEquation::Type equationRgb, BlendEquation::Type equationAlpha );

  /**
   * @copydoc Dali::RenderableActor::GetBlendEquation()
   */
  BlendEquation::Type GetBlendEquationRgb() const;

  /**
   * @copydoc Dali::RenderableActor::GetBlendEquation()
   */
  BlendEquation::Type GetBlendEquationAlpha() const;

  /**
   * Set the blend color.
   * @param[in] color The blend color.
   * @return True if the blend color changed, otherwise it was already the same color.
   */
  void SetBlendColor( const Vector4& color );

  /**
   * Query the blend color.
   * The blend color, or NULL if no blend color was set.
   */
  const Vector4* GetBlendColor() const;

private:

  // Undefined copy constructor.
  BlendingOptions(const BlendingOptions& typePath);

  // Undefined copy constructor.
  BlendingOptions& operator=(const BlendingOptions& rhs);

private:

  unsigned int mBitmask; ///< A bitmask of blending options

  Vector4* mBlendColor; ///< A heap-allocated color (owned)

};

} // namespace Internal

} // namespace Dali

#endif // DALI_BLENDING_OPTIONS_H
