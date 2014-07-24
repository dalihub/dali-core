#ifndef __INTERNAL_TEXT_PARAMETERS_H__
#define __INTERNAL_TEXT_PARAMETERS_H__

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

// INTERNAL HEADERS
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{

namespace Internal
{


  // Number of bits for an index mask - increase if more attributes are added...
  const unsigned int TEXT_PARAMETER_BITS = 2u;

  // Set mask for this number of bits
  const unsigned int TEXT_PARAMETER_MASK = ~( -1 << TEXT_PARAMETER_BITS );

  // Shift values for attribute indices
  const unsigned int OUTLINE_INDEX_SHIFT = 0u;
  const unsigned int GLOW_INDEX_SHIFT = OUTLINE_INDEX_SHIFT + TEXT_PARAMETER_BITS;
  const unsigned int DROP_SHADOW_INDEX_SHIFT = GLOW_INDEX_SHIFT + TEXT_PARAMETER_BITS;
  const unsigned int GRADIENT_INDEX_SHIFT = DROP_SHADOW_INDEX_SHIFT + TEXT_PARAMETER_BITS;
  const unsigned int TEXT_PARAMETER_FLAGS = GRADIENT_INDEX_SHIFT + TEXT_PARAMETER_BITS;

  // Position in flags for attribute index
  const unsigned int OUTLINE_INDEX = 0u;                                                 // bits 0,1
  const unsigned int GLOW_INDEX = TEXT_PARAMETER_MASK << GLOW_INDEX_SHIFT;               // bits 2,3
  const unsigned int DROP_SHADOW_INDEX = TEXT_PARAMETER_MASK << DROP_SHADOW_INDEX_SHIFT; // bits 4,5
  const unsigned int GRADIENT_INDEX = TEXT_PARAMETER_MASK << GRADIENT_INDEX_SHIFT;       // bits 6,7

  // Flag positions for attributes ( gradient has two as parameters can be set independently )
  const unsigned int OUTLINE_EXISTS = 1u << TEXT_PARAMETER_FLAGS;                        // bit 8
  const unsigned int OUTLINE_ENABLED = 1u << ( TEXT_PARAMETER_FLAGS + 1u );              // bit 9
  const unsigned int GLOW_EXISTS = 1u << ( TEXT_PARAMETER_FLAGS + 2u );                  // bit 10
  const unsigned int GLOW_ENABLED = 1u << ( TEXT_PARAMETER_FLAGS + 3u );                 // bit 11
  const unsigned int DROP_SHADOW_EXISTS = 1u << ( TEXT_PARAMETER_FLAGS + 4u );           // bit 12
  const unsigned int DROP_SHADOW_ENABLED = 1u << ( TEXT_PARAMETER_FLAGS + 5u );          // bit 13
  const unsigned int GRADIENT_EXISTS = 1u << ( TEXT_PARAMETER_FLAGS + 6u );              // bit 14
  const unsigned int GRADIENT_ENABLED = 1u << ( TEXT_PARAMETER_FLAGS + 7u );             // bit 15
  const unsigned int ATTRIBUTE_END = GRADIENT_ENABLED;

/**
 * class TextParameters internal class to encapsulate (and allow on demand allocation) of
 * text effect parameters like, outline, glow and shadow
 */
class TextParameters
{
public:

  /**
   * @brief Outline attributes
   */
  struct OutlineAttributes
  {
    Vector4 mOutlineColor;
    Vector2 mOutlineThickness;
  };

  /**
   * @brief Glow attributes
   */
  struct GlowAttributes
  {
    Vector4 mGlowColor;
    float mGlowIntensity;
  };

  /**
   * @brief Drop Shadow attributes
   */
  struct DropShadowAttributes
  {
    Vector4 mDropShadowColor;
    Vector2 mDropShadowOffset;
    float   mDropShadowSize;
  };

  /**
   * @brief Gradient attributes
   */
  struct GradientAttributes
  {
    Vector4 mGradientColor;
    Vector2 mGradientStartPoint;
    Vector2 mGradientEndPoint;
  };

  /**
   * Constructor
   */
  TextParameters();

  /**
   * Destructor
   */
  ~TextParameters();

  /// @copydoc Dali::TextActor::SetOutline
  void SetOutline( bool enable, const Vector4& color, const Vector2& thickness );

  /// @copydoc Dali::TextActor::SetGlow
  void SetGlow( bool enable, const Vector4& color, float intensity );

  /// @copydoc Dali::TextActor::SetShadow
  void SetShadow( bool enable, const Vector4& color, const Vector2& offset, float size );

  /**
   * @brief Set Gradient parameters.
   * @param[in] color The gradient color (end-point color)
   * @param[in] start The relative position of the gradient start point.
   * @param[in] end   The relative position of the gradient end point.
   */
  void SetGradient( const Vector4& color, const Vector2& start, const Vector2& end );

  /**
   * @brief Get the Gradient Color
   *
   * @return Gradient Color
   */
  const Vector4& GetOutlineColor() const;

  /**
   * @brief Get Outline Thickness
   *
   * @return Outline Thickness
   */
  const Vector2& GetOutlineThickness() const;

  /**
   * @brief Get Glow Color
   *
   * @return Glow Color
   */
  const Vector4& GetGlowColor() const;

  /**
   * @brief Get Glow Intensity
   *
   * @return Glow Intensity
   */
  float GetGlowIntensity() const;

  /**
   * @brief Get Drop Shadow Color
   *
   * @return Drop Shadow Color
   */
  const Vector4& GetDropShadowColor() const;

  /**
   * @brief Get Drop Shadow Offset
   *
   * @return Drop Shadow Offset
   */
  const Vector2& GetDropShadowOffset() const;

  /**
   * @brief Get Drop Shadow Size
   *
   * @return Drop Shadow Size
   */
  float GetDropShadowSize() const;

  /**
   * @brief Get Gradient Color
   *
   * @return Gradient Color
   */
  const Vector4& GetGradientColor() const;

  /**
   * @brief Get Gradient Start Point
   *
   * @return Position of Gradient Start Point
   */
  const Vector2& GetGradientStartPoint() const;

  /**
   * @brief Get Gradient End Point
   *
   * @return Position of Gradient End Point
   */
  const Vector2& GetGradientEndPoint() const;

  /**
   * @brief Get if Outline is enabled
   *
   * @return true if enabled, false if not
   */
  bool IsOutlineEnabled() const
  {
    return ( ( mFlags & OUTLINE_ENABLED ) != 0 );
  }

  /**
   * @brief Get if Glow is enabled
   *
   * @return true if enabled, false if not
   */
  bool IsGlowEnabled() const
  {
    return ( ( mFlags & GLOW_ENABLED ) != 0 );
  }

  /**
   * @brief Get if Drop Shadow is enabled
   *
   * @return true if enabled, false if not
   */
  bool IsDropShadowEnabled() const
  {
    return ( ( mFlags & DROP_SHADOW_ENABLED ) != 0 );
  }

  /**
   * @brief Get if Gradient is enabled
   *
   * @return true if enabled, false if not
   */
  bool IsGradientEnabled() const
  {
    return ( ( mFlags & GRADIENT_ENABLED ) != 0 );
  }

private: // unimplemented copy constructor and assignment operator
  TextParameters( const TextParameters& copy );
  TextParameters& operator=(const TextParameters& rhs);

  Vector<char*> mParameters;    // container for any used attributes

#if ( ATTRIBUTE_END > 0x8000 )
  unsigned int mFlags;          // flags for used attributes, packed with position in container
#else
  unsigned short mFlags;        // might be rendered irrelevant by alignment / packing
#endif

}; // class TextParameters

} // namespace Internal

} // namespace Dali

#endif // __INTERNAL_TEXT_PARAMETERS_H__
