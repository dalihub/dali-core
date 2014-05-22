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

// CLASS HEADER
#include <dali/public-api/math/degree.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>
#include <dali/public-api/math/radian.h>

namespace
{
const float ONE80_OVER_PI = 180.0f/Dali::Math::PI;
}

namespace Dali
{

Degree::Degree( float value )
: mValue( value )
{
}

Degree::Degree( const Radian& radian )
: mValue( radian * ONE80_OVER_PI )
{
}

bool Degree::operator==( const Degree& rhs ) const
{
  return fabsf( mValue - rhs.mValue ) < GetRangedEpsilon( mValue, rhs.mValue );
}

bool Degree::operator!=( const Degree& rhs ) const
{
  return !(this->operator==(rhs));
}

bool Degree::operator<( const Degree& rhs ) const
{
  return mValue < rhs.mValue;
}

Degree& Degree::operator=( const float value )
{
  mValue = value;
  return *this;
}

Degree& Degree::operator=( const Radian& rhs )
{
  mValue = rhs * ONE80_OVER_PI;
  return *this;
}

Degree::operator const float&() const
{
  return mValue;
}

Degree::operator float&()
{
  return mValue;
}

} // namespace Dali
