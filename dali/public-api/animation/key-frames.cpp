/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/animation/key-frames.h>

// INTERNAL INCLUDES
#include <dali/internal/event/animation/key-frames-impl.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/math/degree.h>
#include <dali/public-api/math/radian.h>

namespace Dali
{
KeyFrames KeyFrames::New()
{
  Internal::KeyFrames* internal = Internal::KeyFrames::New();
  return KeyFrames(internal);
}

KeyFrames KeyFrames::DownCast(BaseHandle handle)
{
  return KeyFrames(dynamic_cast<Dali::Internal::KeyFrames*>(handle.GetObjectPtr()));
}

KeyFrames::KeyFrames() = default;

KeyFrames::~KeyFrames() = default;

KeyFrames::KeyFrames(const KeyFrames& handle) = default;

KeyFrames& KeyFrames::operator=(const KeyFrames& rhs) = default;

KeyFrames::KeyFrames(KeyFrames&& rhs) = default;

KeyFrames& KeyFrames::operator=(KeyFrames&& rhs) = default;

Property::Type KeyFrames::GetType() const
{
  return GetImplementation(*this).GetType();
}

void KeyFrames::Add(float time, Property::Value value)
{
  Add(time, std::move(value), AlphaFunction::DEFAULT);
}

void KeyFrames::Add(float time, Property::Value value, AlphaFunction alpha)
{
  GetImplementation(*this).Add(time, value, alpha);
}

KeyFrames::KeyFrames(Internal::KeyFrames* internal)
: BaseHandle(internal)
{
}

} // namespace Dali
