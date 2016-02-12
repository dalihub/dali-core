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

// CLASS HEADER
#include <dali/internal/common/blending-options.h>

namespace // unnamed namespace
{

using namespace Dali;

const int MASK_SRC_FACTOR_RGB    = 0x0000000F;
const int MASK_SRC_FACTOR_ALPHA  = 0x000000F0;
const int MASK_DEST_FACTOR_RGB   = 0x00000F00;
const int MASK_DEST_FACTOR_ALPHA = 0x0000F000;
const int MASK_EQUATION_RGB      = 0x000F0000;
const int MASK_EQUATION_ALPHA    = 0x00F00000;

const int SHIFT_TO_SRC_FACTOR_RGB    =  0;
const int SHIFT_TO_SRC_FACTOR_ALPHA  =  4;
const int SHIFT_TO_DEST_FACTOR_RGB   =  8;
const int SHIFT_TO_DEST_FACTOR_ALPHA = 12;
const int SHIFT_TO_EQUATION_RGB      = 16;
const int SHIFT_TO_EQUATION_ALPHA    = 20;

static unsigned int CLEAR_BLEND_FUNC_MASK     = 0xFFFF0000; // Bottom 16 bits cleared
static unsigned int CLEAR_BLEND_EQUATION_MASK = 0xFF00FFFF; // 8 bits cleared

/**
 * Utility to store one of the BlendFunc values.
 * @param[out] options A bitmask used to store the BlendFunc values.
 * @param[in] factor The BlendFunc value.
 * @param[in] bitshift Used to shift to the correct part of options.
 */
void StoreBlendingFactor( unsigned int& options, BlendingFactor::Type factor, int bitShift )
{
  switch ( factor )
  {
    case BlendingFactor::ZERO:
      options |= ( 0u << bitShift );
      break;

    case BlendingFactor::ONE:
      options |= ( 1u << bitShift );
      break;

    case BlendingFactor::SRC_COLOR:
      options |= ( 2u << bitShift );
      break;

    case BlendingFactor::ONE_MINUS_SRC_COLOR:
      options |= ( 3u << bitShift );
      break;

    case BlendingFactor::SRC_ALPHA:
      options |= ( 4u << bitShift );
      break;

    case BlendingFactor::ONE_MINUS_SRC_ALPHA:
      options |= ( 5u << bitShift );
      break;

    case BlendingFactor::DST_ALPHA:
      options |= ( 6u << bitShift );
      break;

    case BlendingFactor::ONE_MINUS_DST_ALPHA:
      options |= ( 7u << bitShift );
      break;

    case BlendingFactor::DST_COLOR:
      options |= ( 8u << bitShift );
      break;

    case BlendingFactor::ONE_MINUS_DST_COLOR:
      options |= ( 9u << bitShift );
      break;

    case BlendingFactor::SRC_ALPHA_SATURATE:
      options |= ( 10u << bitShift );
      break;

    case BlendingFactor::CONSTANT_COLOR:
      options |= ( 11u << bitShift );
      break;

    case BlendingFactor::ONE_MINUS_CONSTANT_COLOR:
      options |= ( 12u << bitShift );
      break;

    case BlendingFactor::CONSTANT_ALPHA:
      options |= ( 13u << bitShift );
      break;

    case BlendingFactor::ONE_MINUS_CONSTANT_ALPHA:
      options |= ( 14u << bitShift );
      break;
  }
}

/**
 * Utility to store one of the BlendEquation values.
 * @param[out] options A bitmask used to store the BlendEquation values.
 * @param[in] factor The BlendEquation value.
 * @param[in] bitshift Used to shift to the correct part of options.
 */
void StoreBlendingEquation( unsigned int& options, BlendingEquation::Type factor, int bitShift )
{
  switch ( factor )
  {
    case BlendingEquation::ADD:
      options |= ( 0u << bitShift );
      break;

    case BlendingEquation::SUBTRACT:
      options |= ( 1u << bitShift );
      break;

    case BlendingEquation::REVERSE_SUBTRACT:
      options |= ( 2u << bitShift );
      break;
  }
}

const unsigned int BLENDING_FACTOR_COUNT   = 15;
const unsigned int BLENDING_EQUATION_COUNT = 3;

BlendingFactor::Type BLENDING_FACTORS[ BLENDING_FACTOR_COUNT ] =
  { BlendingFactor::ZERO,
    BlendingFactor::ONE,
    BlendingFactor::SRC_COLOR,
    BlendingFactor::ONE_MINUS_SRC_COLOR,
    BlendingFactor::SRC_ALPHA,
    BlendingFactor::ONE_MINUS_SRC_ALPHA,
    BlendingFactor::DST_ALPHA,
    BlendingFactor::ONE_MINUS_DST_ALPHA,
    BlendingFactor::DST_COLOR,
    BlendingFactor::ONE_MINUS_DST_COLOR,
    BlendingFactor::SRC_ALPHA_SATURATE,
    BlendingFactor::CONSTANT_COLOR,
    BlendingFactor::ONE_MINUS_CONSTANT_COLOR,
    BlendingFactor::CONSTANT_ALPHA,
    BlendingFactor::ONE_MINUS_CONSTANT_ALPHA };

BlendingEquation::Type BLENDING_EQUATIONS[ BLENDING_EQUATION_COUNT ] =
  { BlendingEquation::ADD,
    BlendingEquation::SUBTRACT,
    BlendingEquation::REVERSE_SUBTRACT };

/**
 * Utility to retrieve one of the BlendFunc values.
 * @param[in] options A bitmask of blending values.
 * @param[in] mask The used to mask unwanted values.
 * @param[in] bitshift Used to shift to the correct part of options.
 * @return The blending factor.
 */
BlendingFactor::Type RetrieveBlendingFactor( unsigned int options, int mask, int bitShift )
{
  unsigned int index = options & mask;
  index = index >> bitShift;

  DALI_ASSERT_DEBUG( index < BLENDING_FACTOR_COUNT );

  return BLENDING_FACTORS[ index ];
}

/**
 * Utility to retrieve one of the BlendEquation values.
 * @param[in] options A bitmask of blending values.
 * @param[in] mask The used to mask unwanted values.
 * @param[in] bitshift Used to shift to the correct part of options.
 * @return The blending equation.
 */
BlendingEquation::Type RetrieveBlendingEquation( unsigned int options, int mask, int bitShift )
{
  unsigned int index = options & mask;
  index = index >> bitShift;

  DALI_ASSERT_DEBUG( index < BLENDING_EQUATION_COUNT );

  return BLENDING_EQUATIONS[ index ];
}

} // unnamed namespace

namespace Dali
{

namespace Internal
{

BlendingOptions::BlendingOptions()
: mBitmask( 0u ),
  mBlendColor( NULL )
{
  SetBlendFunc( DEFAULT_BLENDING_SRC_FACTOR_RGB,   DEFAULT_BLENDING_DEST_FACTOR_RGB,
                DEFAULT_BLENDING_SRC_FACTOR_ALPHA, DEFAULT_BLENDING_DEST_FACTOR_ALPHA );

  SetBlendEquation( DEFAULT_BLENDING_EQUATION_RGB, DEFAULT_BLENDING_EQUATION_ALPHA );
}

BlendingOptions::~BlendingOptions()
{
  delete mBlendColor;
}

void BlendingOptions::SetBitmask( unsigned int bitmask )
{
  mBitmask = bitmask;
}

unsigned int BlendingOptions::GetBitmask() const
{
  return mBitmask;
}

void BlendingOptions::SetBlendFunc( BlendingFactor::Type srcFactorRgb,   BlendingFactor::Type destFactorRgb,
                                    BlendingFactor::Type srcFactorAlpha, BlendingFactor::Type destFactorAlpha )
{
  mBitmask &= CLEAR_BLEND_FUNC_MASK; // Clear the BlendFunc values

  StoreBlendingFactor( mBitmask, srcFactorRgb,    SHIFT_TO_SRC_FACTOR_RGB );
  StoreBlendingFactor( mBitmask, destFactorRgb,   SHIFT_TO_DEST_FACTOR_RGB );
  StoreBlendingFactor( mBitmask, srcFactorAlpha,  SHIFT_TO_SRC_FACTOR_ALPHA );
  StoreBlendingFactor( mBitmask, destFactorAlpha, SHIFT_TO_DEST_FACTOR_ALPHA );
}

BlendingFactor::Type BlendingOptions::GetBlendSrcFactorRgb() const
{
  return RetrieveBlendingFactor( mBitmask, MASK_SRC_FACTOR_RGB,  SHIFT_TO_SRC_FACTOR_RGB );
}

BlendingFactor::Type BlendingOptions::GetBlendDestFactorRgb() const
{
  return RetrieveBlendingFactor( mBitmask, MASK_DEST_FACTOR_RGB, SHIFT_TO_DEST_FACTOR_RGB );
}

BlendingFactor::Type BlendingOptions::GetBlendSrcFactorAlpha() const
{
  return RetrieveBlendingFactor( mBitmask, MASK_SRC_FACTOR_ALPHA,  SHIFT_TO_SRC_FACTOR_ALPHA );
}

BlendingFactor::Type BlendingOptions::GetBlendDestFactorAlpha() const
{
  return RetrieveBlendingFactor( mBitmask, MASK_DEST_FACTOR_ALPHA, SHIFT_TO_DEST_FACTOR_ALPHA );
}

void BlendingOptions::SetBlendEquation( BlendingEquation::Type equationRgb, BlendingEquation::Type equationAlpha )
{
  mBitmask &= CLEAR_BLEND_EQUATION_MASK; // Clear the BlendEquation values

  StoreBlendingEquation( mBitmask, equationRgb,   SHIFT_TO_EQUATION_RGB );
  StoreBlendingEquation( mBitmask, equationAlpha, SHIFT_TO_EQUATION_ALPHA );
}

BlendingEquation::Type BlendingOptions::GetBlendEquationRgb() const
{
  return RetrieveBlendingEquation( mBitmask, MASK_EQUATION_RGB, SHIFT_TO_EQUATION_RGB );
}

BlendingEquation::Type BlendingOptions::GetBlendEquationAlpha() const
{
  return RetrieveBlendingEquation( mBitmask, MASK_EQUATION_ALPHA, SHIFT_TO_EQUATION_ALPHA );
}

void BlendingOptions::SetBlendColor( const Vector4& color )
{
  if( Vector4::ZERO == color )
  {
    if( mBlendColor )
    {
      // Discard unnecessary vector
      delete mBlendColor;
      mBlendColor = NULL;
    }
    return;
  }

  if( mBlendColor )
  {
    *mBlendColor = color;
  }
  else
  {
    // Lazy allocation when non-default is set
    mBlendColor = new Vector4( color );
  }
}

const Vector4* BlendingOptions::GetBlendColor() const
{
  return mBlendColor;
}

} // namespace Internal

} // namespace Dali
