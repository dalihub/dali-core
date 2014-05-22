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
#include <dali/public-api/animation/alpha-functions.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>

namespace Dali
{

namespace AlphaFunctions
{

float Default(float progress)
{
  return progress; // Linear
}

float Linear(float progress)
{
  return progress;
}

float Reverse(float progress)
{
  return 1.0f - progress;
}

float Square(float progress)
{
  return progress * progress;
}

float EaseIn(float progress)
{
  return progress * progress * progress;
}

float EaseOut(float progress)
{
  progress -= 1.0f;

  return progress * progress * progress + 1.0f;
}

float EaseInOut(float progress)
{
  if (progress > 0.5f)
  {
    return EaseOut((progress - 0.5f)*2.0f) * 0.5f + 0.5f;
  }
  else
  {
    return EaseIn(progress * 2.0f) * 0.5f;
  }
}

float EaseInSine(float progress)
{
  return -1.0f * cosf(progress * Math::PI_2) + 1.0f;
}

float EaseOutSine(float progress)
{
  return sinf(progress * Math::PI_2);
}

float EaseInOutSine(float progress)
{
  return -0.5f * (cosf(Math::PI * progress) - 1.0f);
}

float EaseInSine33(float progress)
{
  float tmp = cosf(Math::PI_2 * 33.0f / 90.0f);
  return -1.0f * (cosf(progress * Math::PI_2 * 33.0f / 90.0f) - tmp) / (1.0f - tmp) + 1.0f;
}

float EaseOutSine33(float progress)
{
  float tmp = cosf(Math::PI_2 * 33.0f / 90.0f);
  return (cosf((1.0f - progress) * Math::PI_2 * 33.0f / 90.0f) - tmp) / (1.0f - tmp);
}

float EaseInOutSine33(float progress)
{
  float tmp = sinf(Math::PI_2 * 33.0f / 90.0f);
  return (sinf((progress * Math::PI - Math::PI_2) * 33.0f / 90.0f) + tmp) / (2.0f * tmp);
}

float EaseInOutSine50(float progress)
{
  float tmp = sinf(Math::PI_2 * 50.0f / 90.0f);
  return (sinf((progress * Math::PI - Math::PI_2) * 50.0f / 90.0f) + tmp) / (2.0f * tmp);
}

float EaseInOutSine60(float progress)
{
  float tmp = sinf(Math::PI_2 * 60.0f / 90.0f);
  return (sinf((progress * Math::PI - Math::PI_2) * 60.0f / 90.0f) + tmp) / (2.0f * tmp);
}

float EaseInOutSine70(float progress)
{
  float tmp = sinf(Math::PI_2 * 70.0f / 90.0f);
  return (sinf((progress * Math::PI - Math::PI_2) * 70.0f / 90.0f) + tmp) / (2.0f * tmp);
}

float EaseInOutSine80(float progress)
{
  float tmp = sinf(Math::PI_2 * 80.0f / 90.0f);
  return (sinf((progress * Math::PI - Math::PI_2) * 80.0f / 90.0f) + tmp) / (2.0f * tmp);
}

float EaseInOutSine90(float progress)
{
  return EaseInOutSine(progress);
}

float DoubleEaseInOutSine60(float progress)
{
  if (progress < 0.5f) {
    return EaseInOutSine60(progress * 2.0f) / 2.0f;
  } else {
    return EaseInOutSine60((progress - 0.5f) * 2.0f) / 2.0f + 0.5f;
  }
}

float EaseOutQuint50(float progress)
{
  return 1.0f - pow(1.0f - progress, 1.7f);
}

float EaseOutQuint80(float progress)
{
  return 1.0f - pow(1.0f - progress, 2.3f);
}

float Bounce(float progress)
{
  return sinf(progress * Math::PI);
}

float BounceBack(float progress)
{
  if( progress > 0.0f )
  {
    return (sinf(progress * 2.0f * Math::PI) * sinf(progress * Math::PI)) / (progress * Math::PI);
  }
  else
  {
    return 0;
  }
}

float EaseInBack(float progress)
{
  const float sqrt2 = 1.70158f;

  return  progress * progress * ( ( sqrt2 + 1.0f ) * progress - sqrt2 );
}

float EaseOutBack(float progress)
{
  const float sqrt2 = 1.70158f;
  progress -= 1.0f;

  return 1.0f + progress * progress * ( ( sqrt2 + 1.0f ) * progress + sqrt2 );
}

float EaseInOutBack(float progress)
{
  if (progress > 0.5f)
  {
    return EaseOutBack((progress - 0.5f)*2.0f) * 0.5f + 0.5f;
  }
  else
  {
    return EaseInBack(progress * 2.0f) * 0.5f;
  }
}

float Sin(float progress)
{
  return 0.5f - cosf(progress * 2.0f * Math::PI) * 0.5f;
}

float Sin2x(float progress)
{
  return 0.5f - cosf(progress * 4.0f * Math::PI) * 0.5f;
}

} // AlphaFunctions

} // namespace Dali
