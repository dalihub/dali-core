#ifndef __DALI_ANIMATOR_FUNCTIONS_H__
#define __DALI_ANIMATOR_FUNCTIONS_H__

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
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/quaternion.h>


namespace Dali DALI_IMPORT_API
{

namespace AnimatorFunctions
{

/**
 * @brief Function object that interpolates using a bounce shape.
 */
struct BounceFunc
{
  /**
   * @brief Constructor.
   */
  BounceFunc(float x, float y, float z);

  /**
   * @brief functor
   */
  Vector3 operator()(float alpha, const Vector3& current);

  Vector3 mDistance; ///< Distance to bounce
};

/**
 * @brief Function object that rotates about a random axis twice.
 */
struct TumbleFunc
{
  /**
   * @brief Factory method to create a functor with a random axis.
   */
  static TumbleFunc GetRandom();

  /**
   * @brief Class method to return a random float in the given range
   */
  static float Randomize(float f0, float f1);

  /**
   * @brief Constructor.
   *
   * @param[in] x The x component of the axis
   * @param[in] y The y component of the axis
   * @param[in] z The z component of the axis
   */
  TumbleFunc(float x, float y, float z);

  /**
   * @brief Functor to get the current rotation.
   * @param[in] alpha The alpha value (the output of an alpha function)
   * @param[in] current The current property value
   * @return The output rotation
   */
  Quaternion operator()(float alpha, const Quaternion& current);

  Vector4 tumbleAxis; ///< The axis about which to rotate
};

/**
 * @brief Animator functor that allows provide a timer as input to an animation.
 *
 * The functor returns: scale * ( numberOfLoops + progress )
 */
struct Timer
{
  /**
   * @brief Constructor
   *
   * @param[in] scale Factor by which to multiply progress.
   */
  Timer(float scale);

  /**
   * @brief Functor to return the time.
   * @param[in] progress The animation progress (0-1)
   * @param[in] current The current property value.
   * @return Time since start of animation
   */
  float operator()(float progress, const float& current);

private:
  unsigned int mLoopCounter; ///< Number of times this functor has seen looping progress
  float mPrevious;           ///< The last progress value
  float mScale;              ///< factor by which to multiply progress.
};

} // namespace AnimatorFunctions

} // namespace Dali


#endif // __DALI_ANIMATOR_FUNCTIONS_H__
