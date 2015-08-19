#ifndef __DALI_INTERNAL_PROGRESS_VALUE_H__
#define __DALI_INTERNAL_PROGRESS_VALUE_H__

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
typedef std::vector<AngleAxis>                          ProgressAngleAxisContainer;

typedef ProgressValue<bool>                             ProgressBoolean;
typedef std::vector<ProgressBoolean>                    ProgressBooleanContainer;

typedef ProgressValue<int>                              ProgressInteger;
typedef std::vector<ProgressInteger>                    ProgressIntegerContainer;

typedef ProgressValue<float>                            ProgressNumber;
typedef std::vector<ProgressNumber>                     ProgressNumberContainer;

typedef ProgressValue<Vector2>                          ProgressVector2;
typedef std::vector<ProgressVector2>                    ProgressVector2Container;

typedef ProgressValue<Vector3>                          ProgressVector3;
typedef std::vector<ProgressVector3>                    ProgressVector3Container;

typedef ProgressValue<Vector4>                          ProgressVector4;
typedef std::vector<ProgressVector4>                    ProgressVector4Container;

inline void Interpolate (Quaternion& result, const Quaternion& a, const Quaternion& b, float progress)
{
  result = Quaternion::Slerp(a, b, progress);
}

inline void Interpolate (AngleAxis& result, const AngleAxis& a, const AngleAxis& b, float progress)
{
  Quaternion q1(a.angle, a.axis);
  Quaternion q2(b.angle, b.axis);

  Quaternion iq = Quaternion::Slerp(q1, q2, progress);
  iq.ToAxisAngle(result.axis, result.angle);
}


inline void Interpolate (bool& result, bool a, bool b, float progress)
{
  result = progress < 0.5f ? a : b;
}

inline void Interpolate (int& result, int a, int b, float progress)
{
  result = static_cast<int>(a + (b - a) * progress + 0.5f);
}

inline void Interpolate (unsigned int& result, unsigned int a, unsigned int b, float progress)
{
  result = static_cast<unsigned int>(a + (b - a) * progress + 0.5f);
}

inline void Interpolate (float& result, float a, float b, float progress)
{
  result = a + (b-a) * progress;
}

inline void Interpolate (Vector2& result, const Vector2& a,  const Vector2& b, float progress)
{
  result = a + (b-a) * progress;
}

inline void Interpolate (Vector3& result, const Vector3& a, const Vector3& b, float progress)
{
  result = a + (b-a) * progress;
}

inline void Interpolate (Vector4& result, const Vector4& a, const Vector4& b, float progress)
{
  result = a + (b-a) * progress;
}

/* Cubic Interpolation (Catmull-Rom spline) between values p1 and p2. p0 and p3 are prev and next values
 * and are used as control points to calculate tangent of the curve at interpolation points.
 *
 * f(t) = a3*t^3 + a2*t^2 + a1*t + a0
 * Restrictions: f(0)=p1   f(1)=p2   f'(0)=(p2-p0)*0.5   f'(1)=(p3-p1)*0.5
 */

inline void CubicInterpolate( int& result, int p0, int p1, int p2, int p3, float progress )
{
  float a3 = p3*0.5f - p2*1.5f + p1*1.5f - p0*0.5f;
  float a2 = p0 - p1*2.5f + p2*2.0f - p3*0.5f;
  float a1 = (p2-p0)*0.5f;

  result = static_cast<int>( a3*progress*progress*progress + a2*progress*progress + a1*progress + p1 + 0.5f );
}

inline void CubicInterpolate( unsigned int& result, unsigned int p0, unsigned int p1, unsigned int p2, unsigned int p3, float progress )
{
  float a3 = p3*0.5f - p2*1.5f + p1*1.5f - p0*0.5f;
  float a2 = p0 - p1*2.5f + p2*2.0f - p3*0.5f;
  float a1 = (p2-p0)*0.5f;

  result = static_cast<unsigned int>( a3*progress*progress*progress + a2*progress*progress + a1*progress + p1 + 0.5f );
}

inline void CubicInterpolate( float& result, float p0, float p1, float  p2, float  p3, float progress )
{
  float a3 = p3*0.5f - p2*1.5f + p1*1.5f - p0*0.5f;
  float a2 = p0 - p1*2.5f + p2*2.0f - p3*0.5f;
  float a1 = (p2-p0)*0.5f;

  result = a3*progress*progress*progress + a2*progress*progress + a1*progress + p1;
}

inline void CubicInterpolate( Vector2& result, const Vector2& p0, const Vector2& p1, const Vector2&  p2, const Vector2&  p3, float progress )
{
  Vector2 a3 = p3*0.5f - p2*1.5f + p1*1.5f - p0*0.5f;
  Vector2 a2 = p0 - p1*2.5f + p2*2.0f - p3*0.5f;
  Vector2 a1 = (p2-p0)*0.5f;

  result = a3*progress*progress*progress + a2*progress*progress + a1*progress + p1;
}

inline void CubicInterpolate( Vector3& result, const Vector3& p0, const Vector3& p1, const Vector3&  p2, const Vector3&  p3, float progress )
{
  Vector3 a3 = p3*0.5f - p2*1.5f + p1*1.5f - p0*0.5f;
  Vector3 a2 = p0 - p1*2.5f + p2*2.0f - p3*0.5f;
  Vector3 a1 = (p2-p0)*0.5f;

  result = a3*progress*progress*progress + a2*progress*progress + a1*progress + p1;
}

inline void CubicInterpolate( Vector4& result, const Vector4& p0, const Vector4& p1, const Vector4&  p2, const Vector4&  p3, float progress )
{
  Vector4 a3 = p3*0.5f - p2*1.5f + p1*1.5f - p0*0.5f;
  Vector4 a2 = p0 - p1*2.5f + p2*2.0f - p3*0.5f;
  Vector4 a1 = (p2-p0)*0.5f;

  result = a3*progress*progress*progress + a2*progress*progress + a1*progress + p1;
}

inline void CubicInterpolate( bool& result, bool p0, bool p1, bool  p2, bool  p3, float progress )
{
  Interpolate( result, p1, p2, progress);
}

inline void CubicInterpolate( Quaternion& result, const Quaternion& p0, const Quaternion& p1, const Quaternion& p2, const Quaternion& p3, float progress )
{
  Interpolate( result, p1, p2, progress);
}

inline void CubicInterpolate( AngleAxis& result, const AngleAxis& p0, const AngleAxis& p1, const AngleAxis& p2, const AngleAxis& p3, float progress )
{
  Interpolate( result, p1, p2, progress);
}

} // namespace Internal

} // namespace Dali

#endif //__DALI_PROGRESS_VALUE_H__
