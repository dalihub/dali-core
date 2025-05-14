#ifndef DALI_ALPHA_FUNCTION_H
#define DALI_ALPHA_FUNCTION_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <cstdint> // uint8_t

// INTERNAL INCLUDES
#include <dali/public-api/animation/spring-data.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/math/compile-time-math.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>

namespace Dali
{
/**
 * @addtogroup dali_core_animation
 * @{
 */

using AlphaFunctionPrototype = float (*)(float); ///< Prototype of an alpha function @SINCE_1_0.0

/**
   * @brief Alpha functions are used in animations to specify the rate of change of the animation parameter over time.
   *
   * Understanding an animation as a parametric function over time, the alpha function is applied to the parameter of
   * the animation before computing the final animation value.
   * @SINCE_1_0.0
   */
class DALI_CORE_API AlphaFunction
{
public:
  /**
   * @brief Enumeration for built-in alpha functions.
   * @SINCE_1_0.0
   */
  enum BuiltinFunction : uint8_t
  {
    DEFAULT,          ///< Linear @SINCE_1_0.0
    LINEAR,           ///< No transformation @SINCE_1_0.0
    REVERSE,          ///< Reverse linear @SINCE_1_0.0
    EASE_IN_SQUARE,   ///< Speeds up and comes to a sudden stop (Square) @SINCE_1_0.0
    EASE_OUT_SQUARE,  ///< Sudden start and slows to a gradual stop (Square) @SINCE_1_0.0
    EASE_IN,          ///< Speeds up and comes to a sudden stop (Cubic) @SINCE_1_0.0
    EASE_OUT,         ///< Sudden start and slows to a gradual stop (Cubic) @SINCE_1_0.0
    EASE_IN_OUT,      ///< Speeds up and slows to a gradual stop (Cubic) @SINCE_1_0.0
    EASE_IN_SINE,     ///< Speeds up and comes to a sudden stop (sinusoidal) @SINCE_1_0.0
    EASE_OUT_SINE,    ///< Sudden start and slows to a gradual stop (sinusoidal) @SINCE_1_0.0
    EASE_IN_OUT_SINE, ///< Speeds up and slows to a gradual stop (sinusoidal) @SINCE_1_0.0
    BOUNCE,           ///< Sudden start, loses momentum and returns to start position @SINCE_1_0.0
    SIN,              ///< Single revolution @SINCE_1_0.0
    EASE_OUT_BACK,    ///< Sudden start, exceed end position and return to a gradual stop @SINCE_1_0.0
    COUNT
  };

  /**
   * @brief Enumeration for all possible functioning modes for the alpha function.
   * @SINCE_1_0.0
   */
  enum Mode : uint8_t
  {
    BUILTIN_FUNCTION, ///< The user has specified a built-in function @SINCE_1_0.0
    CUSTOM_FUNCTION,  ///< The user has provided a custom function @SINCE_1_0.0
    BEZIER,           ///< The user has provided the control points of a bezier curve @SINCE_1_0.0
    SPRING,           ///< The user has provided the spring type @SINCE_2_4.17
    CUSTOM_SPRING     ///< The user has provided the spring data @SINCE_2_4.17
  };

  /**
   * @brief Enumeration for predefined spring animation types.
   * This presets are based on typical spring behavior tuned for different motion effects.
   * @SINCE_2_4.17
   */
  enum SpringType : uint8_t
  {
    /**
     * @brief Gentle spring. slower and smoother motion with less oscillation.
     * @SINCE_2_4.17
     */
    GENTLE,

    /**
     * @brief Quick spring, Fast settling animation with minimal overshoot.
     * @SINCE_2_4.17
     */
    QUICK,

    /**
     * @brief Bouncy spring. Highly elastic and oscillatory animation.
     * @SINCE_2_4.17
     */
    BOUNCY,

    /**
     * @brief Slow spring. Smooth and relaxed motion with longer settling.
     * @SINCE_2_4.17
     */
    SLOW
  };

  /**
   * @brief Default constructor.
   * Creates an alpha function object with the default built-in alpha function.
   * @SINCE_1_0.0
   */
  AlphaFunction();

  /**
   * @brief Constructor.
   * Creates an alpha function object with the built-in alpha function passed as a parameter
   * to the constructor.
   * @SINCE_1_0.0
   * @param[in] function One of the built-in alpha functions
   */
  AlphaFunction(BuiltinFunction function);

  /**
   * @brief Constructor.
   * Creates an alpha function object using a pointer to an alpha function passed as a parameter
   * to the constructor.
   * @SINCE_1_0.0
   * @param[in] function A pointer to an alpha function
   */
  AlphaFunction(AlphaFunctionPrototype function);

  /**
   * @brief Constructor.
   *
   * Creates a bezier alpha function. The bezier will have the first point at (0,0) and
   * the end point at (1,1).
   * @SINCE_1_0.0
   * @param[in] controlPoint0 A Vector2 which will be used as the first control point of the curve
   * @param[in] controlPoint1 A Vector2 which will be used as the second control point of the curve
   * @note The x components of the control points will be clamped to the range [0,1] to prevent
   * non monotonic curves.
   */
  AlphaFunction(const Dali::Vector2& controlPoint0, const Dali::Vector2& controlPoint1);

  /**
   * @brief Constructor for spring-based AlphaFunction using a predefined SpringType.
   *
   * @SINCE_2_4.17
   * @param[in] springType The spring preset type to use (e.g., GENTLE, QUICK, etc.).
   */
  AlphaFunction(SpringType springType);

  /**
   * @brief Constructor for spring-based AlphaFunction using custom spring parameters.
   *
   * This allows creating a spring easing function with fully customizable physics behavior.
   * @SINCE_2_4.17
   * @param[in] springData The custom spring configuration (stiffness, damping, mass)
   * @note Since this AlphaFunction follows a physics-based motion model, it does not guarantee
   * that the value will exactly reach 1 at the end of the animation duration.
   * To ensure the animation ends at 1, you may need to adjust the duration according to the spring configuration.
   */
  AlphaFunction(const Dali::SpringData& springData);

  /**
   * @brief Returns the control points of the alpha function.
   * @SINCE_1_0.0
   * @return Vector4 containing the two control points of the curve
   * (xy for the first point and zw for the second)
   */
  Vector4 GetBezierControlPoints() const;

  /**
   * @brief Returns the pointer to the custom function.
   * @SINCE_1_0.0
   * @return A pointer to a custom alpha function, or @c 0 if not defined
   */
  AlphaFunctionPrototype GetCustomFunction() const;

  /**
   * @brief Returns the built-in function used by the alpha function.
   * @SINCE_1_0.0
   * @return One of the built-in alpha functions. In case no built-in function
   * has been specified, it will return AlphaFunction::DEFAULT
   */
  BuiltinFunction GetBuiltinFunction() const;

  /**
   * @brief Returns the functioning mode of the alpha function
   * @SINCE_1_0.0
   * @return The functioning mode of the alpha function
   */
  Mode GetMode() const;

  /**
   * @brief Returns SpringData of spring animation
   * @SINCE_2_4.17
   * @return The SpringData of spring animation
   */
  const Dali::SpringData& GetSpringData() const;

private:
  Mode             mMode;    //< Enum indicating the functioning mode of the AlphaFunction
  BuiltinFunction  mBuiltin; //< Enum indicating the built-in alpha function

  union
  {
    Vector4                mBezierControlPoints; //< Control points for the bezier alpha function
    AlphaFunctionPrototype mCustom;              //< Pointer to an alpha function
    Dali::SpringData       mSpringData;
  };
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_ALPHA_FUNCTION_H
