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
#include <dali/public-api/math/radian.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/degree.h>

namespace
{
const float PI_OVER_180 = Dali::Math::PI/180.0f;
}

namespace Dali
{

Radian::Radian( float value )
: mValue( value )
{
}

Radian::Radian( const Degree& degree )
: mValue( degree * PI_OVER_180 )
{
}

bool Radian::operator==( const Radian& rhs ) const
{
  return fabsf( mValue - rhs.mValue ) < GetRangedEpsilon( mValue, rhs.mValue );
}

bool Radian::operator!=( const Radian& rhs ) const
{
  return !(this->operator==(rhs));
}

bool Radian::operator<( const Radian& rhs ) const
{
  return mValue < rhs.mValue;
}

Radian& Radian::operator=( const float value )
{
  mValue = value;
  return *this;
}

Radian& Radian::operator=( const Degree& rhs )
{
  mValue = rhs * PI_OVER_180;
  return *this;
}

Radian::operator const float&() const
{
  return mValue;
}

Radian::operator float&()
{
  return mValue;
}

} // namespace Dali
