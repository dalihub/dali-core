#ifndef __INTERNAL_TEXT_PARAMETERS_H__
#define __INTERNAL_TEXT_PARAMETERS_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// INTERNAL HEADERS
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{

namespace Internal
{

/**
 * class TextParameters internal class to encapsulate (and allow on demand allocation) of
 * text effect parameters like, outline, glow and shadow
 */
class TextParameters
{
public:
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
  void SetGlow( bool enable, const Vector4& color, const float intensity);

  /// @copydoc Dali::TextActor::SetShadow
  void SetShadow(bool enable, const Vector4& color, const Vector2& offset, const float size);

  /**
   * Set Gradient parameters.
   * @param[in] color The gradient color (end-point color)
   * @param[in] start The relative position of the gradient start point.
   * @param[in] end   The relative position of the gradient end point.
   */
  void SetGradient( const Vector4& color, const Vector2& start, const Vector2& end);

private: // unimplemented copy constructor and assignment operator
  TextParameters( const TextParameters& copy );
  TextParameters& operator=(const TextParameters& rhs);

public: // Attributes
  bool    mOutlineEnabled : 1;
  bool    mGlowEnabled : 1;
  bool    mDropShadowEnabled : 1;
  bool    mGradientEnabled : 1;
  Vector4 mOutlineColor;
  Vector2 mOutline;
  Vector4 mGlowColor;
  float   mGlow;
  Vector4 mDropShadowColor;
  Vector2 mDropShadow;
  float   mDropShadowSize;
  Vector4 mGradientColor;
  Vector2 mGradientStartPoint;
  Vector2 mGradientEndPoint;
}; // class TextParameters

} // namespace Internal

} // namespace Dali

#endif // __INTERNAL_TEXT_PARAMETERS_H__
