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
#include <dali/public-api/animation/animator-functions.h>

namespace Dali
{

namespace AnimatorFunctions
{

BounceFunc::BounceFunc(float x, float y, float z)
: mDistance(Vector3(x, y, z))
{
}

Vector3 BounceFunc::operator()(float alpha, const Vector3& current)
{
  if (alpha>0.001f && alpha<1.0f)
  {
    const float flip = 0.5f - cosf(alpha * Math::PI * 2.0f) * 0.5f;

    Vector3 newTranslation(current);

    newTranslation += mDistance * flip;

    return newTranslation;
  }

  return current;
}

TumbleFunc TumbleFunc::GetRandom()
{
  return TumbleFunc(Randomize(0.5f, 1.0f), Randomize(0.0f, 0.5f), Randomize(0.0f, 0.6f));
}

float TumbleFunc::Randomize(float f0, float f1)
{
  return f0 + (rand() & 0xfff) * (f1-f0) / 4096.0f;
}

TumbleFunc::TumbleFunc(float x, float y, float z)
: tumbleAxis(Vector4(x,y,z,0))
{
  tumbleAxis.Normalize();
}

Quaternion TumbleFunc::operator()(float alpha, const Quaternion& current)
{
  if (alpha>0.001f && alpha<1.0f)
  {
    Quaternion tumbleRotation(Quaternion::FromAxisAngle(tumbleAxis, alpha * Math::PI * 2.0f));

    return tumbleRotation * current;
  }

  return current;
}

Timer::Timer(float scale)
: mLoopCounter( 0u ),
  mPrevious( 0.f ),
  mScale( scale )
{
}

float Timer::operator()(float progress, const float& current)
{
  if ( progress < mPrevious )
  {
    ++mLoopCounter;
  }
  mPrevious = progress;
  return mScale * (progress + mLoopCounter);
}

} // namespace AnimatorFunctions

} // namespace Dali
