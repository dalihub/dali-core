#ifndef __DALI_ANIMATOR_FUNCTIONS_H__
#define __DALI_ANIMATOR_FUNCTIONS_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/quaternion.h>

namespace Dali DALI_IMPORT_API
{

namespace AnimatorFunctions
{

struct BounceFunc
{
  BounceFunc(float x, float y, float z);

  Vector3 operator()(float alpha, const Vector3& current);

  Vector3 mDistance;
};

struct TumbleFunc
{
  static TumbleFunc GetRandom();

  static float Randomize(float f0, float f1);

  TumbleFunc(float x, float y, float z);

  Quaternion operator()(float alpha, const Quaternion& current);

  Vector4 tumbleAxis;
};

/**
 * Animator functor that allows provide a timer as input to an animation.
 * The functor returns: scale * ( numberOfLoops + progress )
 */
struct Timer
{
  Timer(float scale);

  float operator()(float progress, const float& current);

private:
  unsigned int mLoopCounter;
  float mPrevious;
  float mScale;
};

} // namespace AnimatorFunctions

} // namespace Dali

#endif // __DALI_ANIMATOR_FUNCTIONS_H__
