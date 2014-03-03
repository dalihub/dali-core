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
#include <dali/public-api/animation/key-frames.h>

// INTERNAL INCLUDES
#include <dali/public-api/animation/alpha-functions.h>
#include <dali/public-api/math/degree.h>
#include <dali/public-api/math/radian.h>
#include <dali/internal/event/animation/key-frames-impl.h>

namespace Dali
{

KeyFrames KeyFrames::New()
{
  Internal::KeyFrames* internal = Internal::KeyFrames::New();
  return KeyFrames(internal);
}

KeyFrames KeyFrames::DownCast( BaseHandle handle )
{
  return KeyFrames( dynamic_cast<Dali::Internal::KeyFrames*>(handle.GetObjectPtr()) );
}

KeyFrames::KeyFrames()
{
}

KeyFrames::~KeyFrames()
{
}

Property::Type KeyFrames::GetType() const
{
  return GetImplementation(*this).GetType();
}

void KeyFrames::Add(float time, Property::Value value)
{
  Add(time, value, AlphaFunctions::Linear);
}

void KeyFrames::Add(float time, Property::Value value, AlphaFunction alpha)
{
  GetImplementation(*this).Add(time, value, alpha);
}

KeyFrames::KeyFrames(Internal::KeyFrames* internal)
  : BaseHandle(internal)
{
}



} // Dali
