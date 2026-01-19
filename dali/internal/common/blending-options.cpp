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
 *
 */

// CLASS HEADER
#include <dali/internal/common/blending-options.h>

namespace // unnamed namespace
{
using namespace Dali;

const int MASK_SRC_FACTOR_RGB    = 0x0000000F;
const int MASK_SRC_FACTOR_ALPHA  = 0x000000F0;
const int MASK_DEST_FACTOR_RGB   = 0x00000F00;
const int MASK_DEST_FACTOR_ALPHA = 0x0000F000;
const int MASK_EQUATION_RGB      = 0x00FF0000;
const int MASK_EQUATION_ALPHA    = 0xFF000000;

const int SHIFT_TO_SRC_FACTOR_RGB    = 0;
const int SHIFT_TO_SRC_FACTOR_ALPHA  = 4;
const int SHIFT_TO_DEST_FACTOR_RGB   = 8;
const int SHIFT_TO_DEST_FACTOR_ALPHA = 12;
const int SHIFT_TO_EQUATION_RGB      = 16;
const int SHIFT_TO_EQUATION_ALPHA    = 24;

static unsigned int CLEAR_BLEND_FUNC_MASK     = 0xFFFF0000; // Bottom 16 bits cleared
static unsigned int CLEAR_BLEND_EQUATION_MASK = 0x0000FFFF; // Top 16 bits cleared

/**
 * Utility to store one of the BlendFunc values.
 * @param[out] options A bitmask used to store the BlendFunc values.
 * @param[in] factor The BlendFunc value.
 * @param[in] bitshift Used to shift to the correct part of options.
 */
void StoreBlendFactor(unsigned int& options, BlendFactor::Type factor, int bitShift)
{
  switch(factor)
  {
    case BlendFactor::ZERO:
    {
      options |= (0u << bitShift);
      break;
    }

    case BlendFactor::ONE:
    {
      options |= (1u << bitShift);
      break;
    }

    case BlendFactor::SRC_COLOR:
    {
      options |= (2u << bitShift);
      break;
    }

    case BlendFactor::ONE_MINUS_SRC_COLOR:
    {
      options |= (3u << bitShift);
      break;
    }

    case BlendFactor::SRC_ALPHA:
    {
      options |= (4u << bitShift);
      break;
    }

    case BlendFactor::ONE_MINUS_SRC_ALPHA:
    {
      options |= (5u << bitShift);
      break;
    }

    case BlendFactor::DST_ALPHA:
    {
      options |= (6u << bitShift);
      break;
    }

    case BlendFactor::ONE_MINUS_DST_ALPHA:
    {
      options |= (7u << bitShift);
      break;
    }

    case BlendFactor::DST_COLOR:
    {
      options |= (8u << bitShift);
      break;
    }

    case BlendFactor::ONE_MINUS_DST_COLOR:
    {
      options |= (9u << bitShift);
      break;
    }

    case BlendFactor::SRC_ALPHA_SATURATE:
    {
      options |= (10u << bitShift);
      break;
    }

    case BlendFactor::CONSTANT_COLOR:
    {
      options |= (11u << bitShift);
      break;
    }

    case BlendFactor::ONE_MINUS_CONSTANT_COLOR:
    {
      options |= (12u << bitShift);
      break;
    }

    case BlendFactor::CONSTANT_ALPHA:
    {
      options |= (13u << bitShift);
      break;
    }

    case BlendFactor::ONE_MINUS_CONSTANT_ALPHA:
    {
      options |= (14u << bitShift);
      break;
    }
  }
}

/**
 * Utility to store one of the BlendEquation values.
 * @param[out] options A bitmask used to store the BlendEquation values.
 * @param[in] factor The BlendEquation value.
 * @param[in] bitshift Used to shift to the correct part of options.
 */
void StoreBlendEquation(unsigned int& options, DevelBlendEquation::Type factor, int bitShift)
{
  // Must be same order as BLENDING_EQUATIONS, below:
  enum
  {
    ADD_BITVAL = 0u,
    SUBTRACT_BITVAL,
    REVERSE_SUBTRACT_BITVAL,
    MIN_BITVAL,
    MAX_BITVAL,
    MULTIPLY_BITVAL,
    SCREEN_BITVAL,
    OVERLAY_BITVAL,
    DARKEN_BITVAL,
    LIGHTEN_BITVAL,
    COLOR_DODGE_BITVAL,
    COLOR_BURN_BITVAL,
    HARD_LIGHT_BITVAL,
    SOFT_LIGHT_BITVAL,
    DIFFERENCE_BITVAL,
    EXCLUSION_BITVAL,
    HUE_BITVAL,
    SATURATION_BITVAL,
    COLOR_BITVAL,
    LUMINOSITY_BITVAL
  };

  switch(factor)
  {
    case DevelBlendEquation::ADD:
    {
      options |= (ADD_BITVAL << bitShift);
      break;
    }

    case DevelBlendEquation::SUBTRACT:
    {
      options |= (SUBTRACT_BITVAL << bitShift);
      break;
    }

    case DevelBlendEquation::REVERSE_SUBTRACT:
    {
      options |= (REVERSE_SUBTRACT_BITVAL << bitShift);
      break;
    }

    case DevelBlendEquation::MIN:
    {
      options |= (MIN_BITVAL << bitShift);
      break;
    }

    case DevelBlendEquation::MAX:
    {
      options |= (MAX_BITVAL << bitShift);
      break;
    }

    case DevelBlendEquation::MULTIPLY:
    {
      options |= (MULTIPLY_BITVAL << bitShift);
      break;
    }

    case DevelBlendEquation::SCREEN:
    {
      options |= (SCREEN_BITVAL << bitShift);
      break;
    }

    case DevelBlendEquation::OVERLAY:
    {
      options |= (OVERLAY_BITVAL << bitShift);
      break;
    }

    case DevelBlendEquation::DARKEN:
    {
      options |= (DARKEN_BITVAL << bitShift);
      break;
    }

    case DevelBlendEquation::LIGHTEN:
    {
      options |= (LIGHTEN_BITVAL << bitShift);
      break;
    }

    case DevelBlendEquation::COLOR_DODGE:
    {
      options |= (COLOR_DODGE_BITVAL << bitShift);
      break;
    }

    case DevelBlendEquation::COLOR_BURN:
    {
      options |= (COLOR_BURN_BITVAL << bitShift);
      break;
    }

    case DevelBlendEquation::HARD_LIGHT:
    {
      options |= (HARD_LIGHT_BITVAL << bitShift);
      break;
    }

    case DevelBlendEquation::SOFT_LIGHT:
    {
      options |= (SOFT_LIGHT_BITVAL << bitShift);
      break;
    }

    case DevelBlendEquation::DIFFERENCE:
    {
      options |= (DIFFERENCE_BITVAL << bitShift);
      break;
    }

    case DevelBlendEquation::EXCLUSION:
    {
      options |= (EXCLUSION_BITVAL << bitShift);
      break;
    }

    case DevelBlendEquation::HUE:
    {
      options |= (HUE_BITVAL << bitShift);
      break;
    }

    case DevelBlendEquation::SATURATION:
    {
      options |= (SATURATION_BITVAL << bitShift);
      break;
    }

    case DevelBlendEquation::COLOR:
    {
      options |= (COLOR_BITVAL << bitShift);
      break;
    }

    case DevelBlendEquation::LUMINOSITY:
    {
      options |= (LUMINOSITY_BITVAL << bitShift);
      break;
    }
  }
}

const unsigned int BLENDING_FACTOR_COUNT                  = 15;
const unsigned int BLENDING_EQUATION_COUNT                = 20;
const unsigned int BLENDING_EQUATION_ADVANCED_INDEX_START = 5;
const unsigned int BLENDING_EQUATION_ADVANCED_INDEX_END   = 19;

BlendFactor::Type BLENDING_FACTORS[BLENDING_FACTOR_COUNT] =
  {
    BlendFactor::ZERO,
    BlendFactor::ONE,
    BlendFactor::SRC_COLOR,
    BlendFactor::ONE_MINUS_SRC_COLOR,
    BlendFactor::SRC_ALPHA,
    BlendFactor::ONE_MINUS_SRC_ALPHA,
    BlendFactor::DST_ALPHA,
    BlendFactor::ONE_MINUS_DST_ALPHA,
    BlendFactor::DST_COLOR,
    BlendFactor::ONE_MINUS_DST_COLOR,
    BlendFactor::SRC_ALPHA_SATURATE,
    BlendFactor::CONSTANT_COLOR,
    BlendFactor::ONE_MINUS_CONSTANT_COLOR,
    BlendFactor::CONSTANT_ALPHA,
    BlendFactor::ONE_MINUS_CONSTANT_ALPHA};

DevelBlendEquation::Type BLENDING_EQUATIONS[BLENDING_EQUATION_COUNT] =
  {
    DevelBlendEquation::ADD,
    DevelBlendEquation::SUBTRACT,
    DevelBlendEquation::REVERSE_SUBTRACT,
    DevelBlendEquation::MIN,
    DevelBlendEquation::MAX,
    DevelBlendEquation::MULTIPLY,
    DevelBlendEquation::SCREEN,
    DevelBlendEquation::OVERLAY,
    DevelBlendEquation::DARKEN,
    DevelBlendEquation::LIGHTEN,
    DevelBlendEquation::COLOR_DODGE,
    DevelBlendEquation::COLOR_BURN,
    DevelBlendEquation::HARD_LIGHT,
    DevelBlendEquation::SOFT_LIGHT,
    DevelBlendEquation::DIFFERENCE,
    DevelBlendEquation::EXCLUSION,
    DevelBlendEquation::HUE,
    DevelBlendEquation::SATURATION,
    DevelBlendEquation::COLOR,
    DevelBlendEquation::LUMINOSITY};

/**
 * Utility to retrieve one of the BlendFunc values.
 * @param[in] options A bitmask of blending values.
 * @param[in] mask The used to mask unwanted values.
 * @param[in] bitshift Used to shift to the correct part of options.
 * @return The blending factor.
 */
BlendFactor::Type RetrieveBlendFactor(unsigned int options, int mask, int bitShift)
{
  unsigned int index = options & mask;
  index              = index >> bitShift;

  DALI_ASSERT_DEBUG(index < BLENDING_FACTOR_COUNT);

  return BLENDING_FACTORS[index];
}

/**
 * Utility to retrieve one of the BlendEquation values.
 * @param[in] options A bitmask of blending values.
 * @param[in] mask The used to mask unwanted values.
 * @param[in] bitshift Used to shift to the correct part of options.
 * @return The blending equation.
 */
DevelBlendEquation::Type RetrieveBlendEquation(unsigned int options, int mask, int bitShift)
{
  unsigned int index = options & mask;
  index              = index >> bitShift;

  DALI_ASSERT_DEBUG(index < BLENDING_EQUATION_COUNT);

  return BLENDING_EQUATIONS[index];
}

} // unnamed namespace

namespace Dali
{
namespace Internal
{
BlendingOptions::BlendingOptions()
: mBitmask(0u),
  mBlendColor(nullptr)
{
  SetBlendFunc(BlendFactor::SRC_ALPHA, BlendFactor::ONE_MINUS_SRC_ALPHA, BlendFactor::ONE, BlendFactor::ONE_MINUS_SRC_ALPHA);

  SetBlendEquation(DevelBlendEquation::ADD, DevelBlendEquation::ADD);
}

BlendingOptions::~BlendingOptions() = default;

void BlendingOptions::SetBitmask(unsigned int bitmask)
{
  mBitmask = bitmask;
}

unsigned int BlendingOptions::GetBitmask() const
{
  return mBitmask;
}

void BlendingOptions::SetBlendFunc(BlendFactor::Type srcFactorRgb, BlendFactor::Type destFactorRgb, BlendFactor::Type srcFactorAlpha, BlendFactor::Type destFactorAlpha)
{
  mBitmask &= CLEAR_BLEND_FUNC_MASK; // Clear the BlendFunc values

  StoreBlendFactor(mBitmask, srcFactorRgb, SHIFT_TO_SRC_FACTOR_RGB);
  StoreBlendFactor(mBitmask, destFactorRgb, SHIFT_TO_DEST_FACTOR_RGB);
  StoreBlendFactor(mBitmask, srcFactorAlpha, SHIFT_TO_SRC_FACTOR_ALPHA);
  StoreBlendFactor(mBitmask, destFactorAlpha, SHIFT_TO_DEST_FACTOR_ALPHA);
}

BlendFactor::Type BlendingOptions::GetBlendSrcFactorRgb() const
{
  return RetrieveBlendFactor(mBitmask, MASK_SRC_FACTOR_RGB, SHIFT_TO_SRC_FACTOR_RGB);
}

BlendFactor::Type BlendingOptions::GetBlendDestFactorRgb() const
{
  return RetrieveBlendFactor(mBitmask, MASK_DEST_FACTOR_RGB, SHIFT_TO_DEST_FACTOR_RGB);
}

BlendFactor::Type BlendingOptions::GetBlendSrcFactorAlpha() const
{
  return RetrieveBlendFactor(mBitmask, MASK_SRC_FACTOR_ALPHA, SHIFT_TO_SRC_FACTOR_ALPHA);
}

BlendFactor::Type BlendingOptions::GetBlendDestFactorAlpha() const
{
  return RetrieveBlendFactor(mBitmask, MASK_DEST_FACTOR_ALPHA, SHIFT_TO_DEST_FACTOR_ALPHA);
}

void BlendingOptions::SetBlendEquation(DevelBlendEquation::Type equationRgb, DevelBlendEquation::Type equationAlpha)
{
  mBitmask &= CLEAR_BLEND_EQUATION_MASK; // Clear the BlendEquation values

  StoreBlendEquation(mBitmask, equationRgb, SHIFT_TO_EQUATION_RGB);
  StoreBlendEquation(mBitmask, equationAlpha, SHIFT_TO_EQUATION_ALPHA);
}

DevelBlendEquation::Type BlendingOptions::GetBlendEquationRgb() const
{
  return RetrieveBlendEquation(mBitmask, MASK_EQUATION_RGB, SHIFT_TO_EQUATION_RGB);
}

DevelBlendEquation::Type BlendingOptions::GetBlendEquationAlpha() const
{
  return RetrieveBlendEquation(mBitmask, MASK_EQUATION_ALPHA, SHIFT_TO_EQUATION_ALPHA);
}

void BlendingOptions::SetBlendColor(const Vector4& color)
{
  if(Color::TRANSPARENT == color)
  {
    mBlendColor = nullptr;
  }
  else
  {
    if(mBlendColor)
    {
      *mBlendColor = color;
    }
    else
    {
      // Lazy allocation when non-default is set
      mBlendColor = new Vector4(color);
    }
  }
}

const Vector4* BlendingOptions::GetBlendColor() const
{
  return mBlendColor.Get();
}

bool BlendingOptions::IsAdvancedBlendEquationApplied() const
{
  unsigned int indexRgb = mBitmask & MASK_EQUATION_RGB;
  indexRgb              = indexRgb >> SHIFT_TO_EQUATION_RGB;
  unsigned int indexA   = mBitmask & MASK_EQUATION_ALPHA;
  indexA                = indexA >> SHIFT_TO_EQUATION_ALPHA;

  return (((indexRgb >= BLENDING_EQUATION_ADVANCED_INDEX_START) && (indexRgb <= BLENDING_EQUATION_ADVANCED_INDEX_END)) ||
          ((indexA >= BLENDING_EQUATION_ADVANCED_INDEX_START) && (indexA <= BLENDING_EQUATION_ADVANCED_INDEX_END)));
}

bool BlendingOptions::IsAdvancedBlendEquationIncluded(unsigned int bitmask)
{
  unsigned int indexRgb = bitmask & MASK_EQUATION_RGB;
  indexRgb              = indexRgb >> SHIFT_TO_EQUATION_RGB;
  unsigned int indexA   = bitmask & MASK_EQUATION_ALPHA;
  indexA                = indexA >> SHIFT_TO_EQUATION_ALPHA;

  return (((indexRgb >= BLENDING_EQUATION_ADVANCED_INDEX_START) && (indexRgb <= BLENDING_EQUATION_ADVANCED_INDEX_END)) ||
          ((indexA >= BLENDING_EQUATION_ADVANCED_INDEX_START) && (indexA <= BLENDING_EQUATION_ADVANCED_INDEX_END)));
}

bool BlendingOptions::IsAdvancedBlendEquation(DevelBlendEquation::Type equation)
{
  switch(equation)
  {
    case DevelBlendEquation::MULTIPLY:
    case DevelBlendEquation::SCREEN:
    case DevelBlendEquation::OVERLAY:
    case DevelBlendEquation::DARKEN:
    case DevelBlendEquation::LIGHTEN:
    case DevelBlendEquation::COLOR_DODGE:
    case DevelBlendEquation::COLOR_BURN:
    case DevelBlendEquation::HARD_LIGHT:
    case DevelBlendEquation::SOFT_LIGHT:
    case DevelBlendEquation::DIFFERENCE:
    case DevelBlendEquation::EXCLUSION:
    case DevelBlendEquation::HUE:
    case DevelBlendEquation::SATURATION:
    case DevelBlendEquation::COLOR:
    case DevelBlendEquation::LUMINOSITY:
    {
      return true;
    }

    default:
    {
      return false;
    }
  }
}

} // namespace Internal

} // namespace Dali
