#ifndef __DALI_ALPHA_FUNCTIONS_H__
#define __DALI_ALPHA_FUNCTIONS_H__

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
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/constants.h>

namespace Dali
{

typedef float (*AlphaFunction)(float progress); ///< Definition of an alpha function

/**
 * @brief Namespace containing a set of alpha functions.
 */
namespace AlphaFunctions
{

inline float Linear( float progress )
{
  return progress;
}

inline float Default( float progress )
{
  return Linear( progress );
}

inline float Reverse( float progress )    ///< Reverse linear
{
  return 1.0f - progress;
}

inline float Square( float progress )     ///< Square (x^2)
{
  return progress * progress;
}

inline float EaseIn( float progress )     ///< Speeds up and comes to a sudden stop
{
  return progress * progress * progress;
}

inline float EaseOut( float progress )    ///< Sudden start and slows to a gradual stop
{
  progress -= 1.0f;

  return progress * progress * progress + 1.0f;
}

inline float EaseInOut( float progress )  ///< Speeds up and slows to a gradual stop
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

inline float EaseInSine( float progress )    ///< Speeds up and comes to a sudden stop
{
  return -1.0f * cosf(progress * Math::PI_2) + 1.0f;
}

inline float EaseOutSine( float progress )   ///< Sudden start and slows to a gradual stop
{
  return sinf(progress * Math::PI_2);
}

inline float EaseInOutSine( float progress ) ///< Speeds up and slows to a gradual stop
{
  return -0.5f * (cosf(Math::PI * progress) - 1.0f);
}

inline float EaseInSine33( float progress )  ///< Speeds up and comes to a sudden stop
{
  float tmp = cosf(Math::PI_2 * 33.0f / 90.0f);
  return -1.0f * (cosf(progress * Math::PI_2 * 33.0f / 90.0f) - tmp) / (1.0f - tmp) + 1.0f;
}

inline float EaseOutSine33( float progress ) ///< Sudden start and slows to a gradual stop
{
  float tmp = cosf(Math::PI_2 * 33.0f / 90.0f);
  return (cosf((1.0f - progress) * Math::PI_2 * 33.0f / 90.0f) - tmp) / (1.0f - tmp);
}

inline float EaseInOutSine33( float progress ) ///< Speeds up and slows to a gradual stop
{
  float tmp = sinf(Math::PI_2 * 33.0f / 90.0f);
  return (sinf((progress * Math::PI - Math::PI_2) * 33.0f / 90.0f) + tmp) / (2.0f * tmp);
}

inline float EaseInOutSine50( float progress ) ///< Speeds up and slows to a gradual stop
{
  float tmp = sinf(Math::PI_2 * 50.0f / 90.0f);
  return (sinf((progress * Math::PI - Math::PI_2) * 50.0f / 90.0f) + tmp) / (2.0f * tmp);
}

inline float EaseInOutSine60( float progress ) ///< Speeds up and slows to a gradual stop
{
  float tmp = sinf(Math::PI_2 * 60.0f / 90.0f);
  return (sinf((progress * Math::PI - Math::PI_2) * 60.0f / 90.0f) + tmp) / (2.0f * tmp);
}

inline float EaseInOutSine70( float progress ) ///< Speeds up and slows to a gradual stop
{
  float tmp = sinf(Math::PI_2 * 70.0f / 90.0f);
  return (sinf((progress * Math::PI - Math::PI_2) * 70.0f / 90.0f) + tmp) / (2.0f * tmp);
}

inline float EaseInOutSine80( float progress ) ///< Speeds up and slows to a gradual stop
{
  float tmp = sinf(Math::PI_2 * 80.0f / 90.0f);
  return (sinf((progress * Math::PI - Math::PI_2) * 80.0f / 90.0f) + tmp) / (2.0f * tmp);
}

inline float EaseInOutSine90( float progress ) ///< Speeds up and slows to a gradual stop
{
  return EaseInOutSine(progress);
}

inline float DoubleEaseInOutSine60( float progress ) ///< Speeds up and slows to a gradual stop, then speeds up again and slows to a gradual stop
{
  if (progress < 0.5f)
  {
    return EaseInOutSine60(progress * 2.0f) / 2.0f;
  }
  else
  {
    return EaseInOutSine60((progress - 0.5f) * 2.0f) / 2.0f + 0.5f;
  }
}

inline float EaseOutQuint50( float progress ) ///< Sudden start and slows to a gradual stop
{
  return 1.0f - powf(1.0f - progress, 1.7f);
}

inline float EaseOutQuint80( float progress ) ///< Sudden start and slows to a gradual stop
{
  return 1.0f - powf(1.0f - progress, 2.3f);
}

inline float Bounce( float progress ) ///< Sudden start, loses momentum and returns to start position
{
  return sinf(progress * Math::PI);
}

inline float BounceBack( float progress ) ///< Sudden start, loses momentum and returns to exceed start position and gradual stop at start position
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

inline float EaseInBack( float progress ) ///< Slow start, exceed start position and quickly reach destination
{
  const float sqrt2 = 1.70158f;

  return  progress * progress * ( ( sqrt2 + 1.0f ) * progress - sqrt2 );
}

inline float EaseOutBack( float progress ) ///< Sudden start, exceed end position and return to a gradual stop
{
  const float sqrt2 = 1.70158f;
  progress -= 1.0f;

  return 1.0f + progress * progress * ( ( sqrt2 + 1.0f ) * progress + sqrt2 );
}

inline float EaseInOutBack( float progress ) ///< Slow start, exceed start position, fast middle, exceed end position and return to a gradual stop
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

inline float Sin( float progress ) ///< Single revolution
{
  return 0.5f - cosf(progress * 2.0f * Math::PI) * 0.5f;
}

inline float Sin2x( float progress ) ///< Two revolutions
{
  return 0.5f - cosf(progress * 4.0f * Math::PI) * 0.5f;
}

} // namespace AlphaFunctions

} // namespace Dali

#endif // __DALI_ALPHA_FUNCTIONS_H__
