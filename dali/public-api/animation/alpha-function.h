#ifndef __DALI_ALPHA_FUNCTION_H__
#define __DALI_ALPHA_FUNCTION_H__

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
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/compile-time-math.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{

typedef float (*AlphaFunctionPrototype)(float progress); ///< Prototype of an alpha function

  /*
   * @brief Alpha functions are used in animations to specify the rate of change of the animation parameter over time.
   * Understanding an animation as a parametric function over time, the alpha function is applied to the parameter of
   * the animation before computing the final animation value.
   */
class DALI_IMPORT_API AlphaFunction
{
public:

  /**
   * Built-in alpha functions
   */
  enum BuiltinFunction
  {
    DEFAULT,            ///< Linear
    LINEAR,             ///< No transformation
    REVERSE,            ///< Reverse linear

    EASE_IN_SQUARE,     ///< Speeds up and comes to a sudden stop (Square)
    EASE_OUT_SQUARE,    ///< Sudden start and slows to a gradual stop (Square)

    EASE_IN,            ///< Speeds up and comes to a sudden stop (Cubic)
    EASE_OUT,           ///< Sudden start and slows to a gradual stop (Cubic)
    EASE_IN_OUT,        ///< Speeds up and slows to a gradual stop (Cubic)

    EASE_IN_SINE,       ///< Speeds up and comes to a sudden stop (sinusoidal)
    EASE_OUT_SINE,      ///< Sudden start and slows to a gradual stop (sinusoidal)
    EASE_IN_OUT_SINE,   ///< Speeds up and slows to a gradual stop (sinusoidal)

    BOUNCE,             ///< Sudden start, loses momentum and returns to start position
    SIN,                ///< Single revolution
    EASE_OUT_BACK,      ///< Sudden start, exceed end position and return to a gradual stop

    COUNT
  };

  /**
   * All possible functioning modes for the alpha function
   */
  enum Mode
  {
    BUILTIN_FUNCTION,  //< The user has specified a built-in function
    CUSTOM_FUNCTION,   //< The user has provided a custom function
    BEZIER             //< The user has provided the control points of a bezier curve
  };

  /**
   * @brief Default constructor.
   * Creates an alpha function object with the default built-in alpha function
   * @return The alpha function
   */
  AlphaFunction();

  /**
   * @brief Constructor.
   * Creates an alpha function object with the built-in alpha function passed as a parameter
   * to the constructor
   * @param[in] function One of the built-in alpha functions
   * @return The alpha function
   */
  AlphaFunction( BuiltinFunction function);

  /**
   * @brief Constructor.
   * Creates an alpha function object using a pointer to an alpha function passed as a paramter
   * to the constructor
   * @param[in] function A pointer to an alpha function
   * @return The alpha function
   */
  AlphaFunction( AlphaFunctionPrototype function);

  /**
   * @brief Constructor.
   * Creates a bezier alpha function. The bezier will have the first point at (0,0) and
   * the end point at (1,1).
   * @note The x components of the control points will be clamped to the range [0,1] to prevent
   * non monotonic curves.
   * @param[in] controlPoint0 A Vector2 which will be used as the first control point of the curve
   * @param[in] controlPoint1 A Vector2 which will be used as the second control point of the curve
   * @return The alpha function
   */
  AlphaFunction( const Dali::Vector2& controlPoint0, const Dali::Vector2& controlPoint1 );

  /**
   * @brief Return the control points of the alpha function
   * @return Vector4 containing the two control points of the curve.
   * (xy for the first point and zw for the second)
   */
  Vector4 GetBezierControlPoints() const;

  /**
   * @brief Returns the pointer to the custom function
   * @return A pointer to a custom alpha function or 0 if not defined
   */
  AlphaFunctionPrototype GetCustomFunction() const;

  /**
   * @brief Returns the built0in function used by the alpha function
   * @return One of the built-in alpha functions. In case no built-in function
   * has been specified, it will return AlphaFunction::DEfAULT
   */
  BuiltinFunction GetBuiltinFunction() const;

  /**
   * @brief Returns the functioning mode of the alpha function
   * @return The functioning mode of the alpha function
   */
  Mode GetMode() const;

private:

  Vector4                 mBezierControlPoints;   //< Control points for the bezier alpha function
  AlphaFunctionPrototype  mCustom;                //< Pointer to an alpha function
  BuiltinFunction         mBuiltin : Log<COUNT>::value+1; //< Enum indicating the built-in alpha function
  Mode                    mMode    : 2;                   //< Enum indicating the functioning mode of the AlphaFunction
};

} // namespace Dali

#endif // __DALI_ALPHA_FUNCTION_H__
