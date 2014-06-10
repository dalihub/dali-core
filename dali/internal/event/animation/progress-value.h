#ifndef __DALI_INTERNAL_PROGRESS_VALUE_H__
#define __DALI_INTERNAL_PROGRESS_VALUE_H__

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
#include <dali/public-api/math/quaternion.h>
#include <dali/public-api/math/vector3.h>
#include <dali/public-api/math/radian.h>
#include <dali/public-api/math/degree.h>

namespace Dali
{

namespace Internal
{

/**
 * Progress / value pair for animating channels (properties) with keyframes
 */
template <typename T>
class ProgressValue
{
public:
  ProgressValue (float progress, T value)
  : mProgress(progress),
    mValue (value)
  {
  }

  ~ProgressValue ()
  {
  }

  float GetProgress () const
  {
    return mProgress;
  }

  const T& GetValue () const
  {
    return mValue;
  }

public:
  float mProgress;   ///< Progress this value applies to animation channel
  T mValue;          ///< value this animation channel should take
};

typedef ProgressValue<Quaternion>                       ProgressQuaternion;
typedef std::vector<ProgressQuaternion>                 ProgressQuaternionContainer;

typedef ProgressValue<AngleAxis>                        ProgressAngleAxis;
typedef std::vector<ProgressQuaternion>                 ProgressQuaternionContainer;

typedef ProgressValue<bool>                             ProgressBoolean;
typedef std::vector<ProgressBoolean>                    ProgressBooleanContainer;

typedef ProgressValue<float>                            ProgressNumber;
typedef std::vector<ProgressNumber>                     ProgressNumberContainer;

typedef ProgressValue<Vector2>                          ProgressVector2;
typedef std::vector<ProgressVector2>                    ProgressVector2Container;

typedef ProgressValue<Vector3>                          ProgressVector3;
typedef std::vector<ProgressVector3>                    ProgressVector3Container;

typedef ProgressValue<Vector4>                          ProgressVector4;
typedef std::vector<ProgressVector4>                    ProgressVector4Container;

inline Quaternion Interpolate (ProgressQuaternion& a, ProgressQuaternion& b, float progress)
{
  return Quaternion::Slerp(a.GetValue(), b.GetValue(), progress);
}

inline AngleAxis Interpolate (ProgressAngleAxis& a, ProgressAngleAxis& b, float progress)
{
  AngleAxis av(a.GetValue());
  AngleAxis bv(b.GetValue());
  Quaternion q1(Radian(av.angle), av.axis);
  Quaternion q2(Radian(bv.angle), bv.axis);

  Quaternion iq = Quaternion::Slerp(q1, q2, progress);
  AngleAxis result;
  iq.ToAxisAngle(result.axis, result.angle);
  return result;
}


inline bool Interpolate (ProgressBoolean& a, ProgressBoolean& b, float progress)
{
  return progress < 0.5f ? a.GetValue() : b.GetValue();
}

inline float Interpolate (ProgressNumber& a, ProgressNumber& b, float progress)
{
  return (a.GetValue() + (b.GetValue() - a.GetValue()) * progress);
}

inline Vector2 Interpolate (ProgressVector2& a, ProgressVector2& b, float progress)
{
  return (a.GetValue() + (b.GetValue() - a.GetValue()) * progress);
}

inline Vector3 Interpolate (ProgressVector3& a, ProgressVector3& b, float progress)
{
  return (a.GetValue() + (b.GetValue() - a.GetValue()) * progress);
}

inline Vector4 Interpolate (ProgressVector4& a, ProgressVector4& b, float progress)
{
  return (a.GetValue() + (b.GetValue() - a.GetValue()) * progress);
}

} // namespace Internal

} // namespace Dali

#endif //__DALI_PROGRESS_VALUE_H__
