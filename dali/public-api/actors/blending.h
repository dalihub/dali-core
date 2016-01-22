#ifndef __DALI_BLENDING_H__
#define __DALI_BLENDING_H__

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
 *
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
/**
 * @addtogroup dali_core_actors
 * @{
 */

namespace BlendingMode
{
/**
 * @brief Blending mode.
 * @SINCE_1_0.0
 */
enum Type
{
  OFF,  ///< Blending is disabled. @SINCE_1_0.0
  AUTO, ///< Blending is enabled if there is alpha channel. This is the default mode. @SINCE_1_0.0
  ON    ///< Blending is enabled. @SINCE_1_0.0
};

} //namespace BlendingMode

namespace BlendingFactor
{
/**
 * @brief Blending Factor.
 *
 * @SINCE_1_0.0
 * @see Dali::RenderableActor::SetBlendFunc() and Dali::RenderableActor::GetBlendFunc()
 */
enum Type
{
  ZERO                     = 0,
  ONE                      = 1,
  SRC_COLOR                = 0x0300,
  ONE_MINUS_SRC_COLOR      = 0x0301,
  SRC_ALPHA                = 0x0302,
  ONE_MINUS_SRC_ALPHA      = 0x0303,
  DST_ALPHA                = 0x0304,
  ONE_MINUS_DST_ALPHA      = 0x0305,
  DST_COLOR                = 0x0306,
  ONE_MINUS_DST_COLOR      = 0x0307,
  SRC_ALPHA_SATURATE       = 0x0308,
  CONSTANT_COLOR           = 0x8001,
  ONE_MINUS_CONSTANT_COLOR = 0x8002,
  CONSTANT_ALPHA           = 0x8003,
  ONE_MINUS_CONSTANT_ALPHA = 0x8004
};

} // namespace BlendingFactor

namespace BlendingEquation
{
/**
 * @brief Blending Equation.
 *
 * @SINCE_1_0.0
 * @see Dali::RenderableActor::SetBlendEquation() and Dali::RenderableActor::GetBlendEquation()
 */
enum Type
{
  ADD              = 0x8006,
  SUBTRACT         = 0x800A,
  REVERSE_SUBTRACT = 0x800B
};

} // namespace BlendingEquation

DALI_IMPORT_API extern const BlendingFactor::Type   DEFAULT_BLENDING_SRC_FACTOR_RGB;    ///< BlendingFactor::SRC_ALPHA
DALI_IMPORT_API extern const BlendingFactor::Type   DEFAULT_BLENDING_DEST_FACTOR_RGB;   ///< BlendingFactor::ONE_MINUS_SRC_ALPHA
DALI_IMPORT_API extern const BlendingFactor::Type   DEFAULT_BLENDING_SRC_FACTOR_ALPHA;  ///< BlendingFactor::ONE
DALI_IMPORT_API extern const BlendingFactor::Type   DEFAULT_BLENDING_DEST_FACTOR_ALPHA; ///< BlendingFactor::ONE_MINUS_SRC_ALPHA

DALI_IMPORT_API extern const BlendingEquation::Type DEFAULT_BLENDING_EQUATION_RGB;     ///< BlendingEquation::ADD
DALI_IMPORT_API extern const BlendingEquation::Type DEFAULT_BLENDING_EQUATION_ALPHA;   ///< BlendingEquation::ADD

/**
 * @}
 */
} // namespace Dali

#endif // __DALI_BLENDING_H__
