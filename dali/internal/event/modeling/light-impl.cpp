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
#include <dali/internal/event/modeling/light-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h>

namespace
{
const float DEFAULT_FALLOFF_START = 0.0f;
const float DEFAULT_FALLOFF_END   = 10000.0f;
const float DEFAULT_SPOT_CONE     = 90.0f;
}

namespace Dali
{

namespace Internal
{

Light::Light(const std::string& name)
: mName(name),
  mType(POINT),
  mFallOff(DEFAULT_FALLOFF_START, DEFAULT_FALLOFF_END),
  mSpotAngle(DEFAULT_SPOT_CONE, DEFAULT_SPOT_CONE),
  mAmbientColor(Color::BLACK),
  mDiffuseColor(Color::WHITE),
  mSpecularColor(Color::WHITE),
  mDirection(Vector3::NEGATIVE_ZAXIS)
{
}

/**
 * Copy Constructor
 */
Light::Light(const Light& rhs)
: mName(rhs.mName),
  mType(rhs.mType),
  mFallOff(rhs.mFallOff),
  mSpotAngle(rhs.mSpotAngle),
  mAmbientColor(rhs.mAmbientColor),
  mDiffuseColor(rhs.mDiffuseColor),
  mSpecularColor(rhs.mSpecularColor),
  mDirection(rhs.mDirection)
{
}

/**
 * Assignment operator
 */
Light& Light::operator=(const Light& rhs)
{
  if( this != &rhs)
  {
    mName           = rhs.mName;
    mType           = rhs.mType;
    mFallOff        = rhs.mFallOff;
    mSpotAngle      = rhs.mSpotAngle;
    mAmbientColor   = rhs.mAmbientColor;
    mDiffuseColor   = rhs.mDiffuseColor;
    mSpecularColor  = rhs.mSpecularColor;
    mDirection      = rhs.mDirection;
  }

  return *this;
}

Light::~Light()
{
}

} // namespace Internal

} // namespace Dali
