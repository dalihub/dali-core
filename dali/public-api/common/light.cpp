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
#include <dali/public-api/common/light.h>

// INTERNAL INCLUDES
#include <dali/internal/event/modeling/light-impl.h>

namespace Dali
{

Light Light::New(const std::string& name)
{
  Internal::Light* internal = new Internal::Light(name);

  return Light(internal);
}

Light Light::DownCast( BaseHandle handle )
{
  return Light( dynamic_cast<Dali::Internal::Light*>(handle.GetObjectPtr()) );
}

Light::Light(Internal::Light* internal)
: BaseHandle(internal)
{
}

Light::~Light()
{
}

Light::Light(const Light& handle)
: BaseHandle(handle)
{
}

Light& Light::operator=(const Light& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

void Light::SetName(const std::string& name)
{
  GetImplementation(*this).SetName(name);
}

const std::string& Light::GetName() const
{
  return GetImplementation(*this).GetName();
}

void Light::SetType(LightType type)
{
  GetImplementation(*this).SetType(type);
}

LightType Light::GetType() const
{
  return GetImplementation(*this).GetType();
}

void Light::SetFallOff(const Vector2& fallOff)
{
  GetImplementation(*this).SetFallOff(fallOff);
}

const Vector2& Light::GetFallOff() const
{
  return GetImplementation(*this).GetFallOff();
}

void Light::SetSpotAngle(const Vector2& angle)
{
  GetImplementation(*this).SetSpotAngle(angle);
}

const Vector2& Light::GetSpotAngle() const
{
  return GetImplementation(*this).GetSpotAngle();
}

void Light::SetAmbientColor(const Vector3& color)
{
  GetImplementation(*this).SetAmbientColor(color);
}

const Vector3& Light::GetAmbientColor() const
{
  return GetImplementation(*this).GetAmbientColor();
}

void Light::SetDiffuseColor(const Vector3& color)
{
  GetImplementation(*this).SetDiffuseColor(color);
}

const Vector3& Light::GetDiffuseColor() const
{
  return GetImplementation(*this).GetDiffuseColor();
}

void Light::SetSpecularColor(const Vector3& color)
{
  GetImplementation(*this).SetSpecularColor(color);
}

const Vector3& Light::GetSpecularColor() const
{
  return GetImplementation(*this).GetSpecularColor();
}

void Light::SetDirection(const Vector3& direction)
{
  GetImplementation(*this).SetDirection(direction);
}

const Vector3& Light::GetDirection() const
{
  return GetImplementation(*this).GetDirection();
}

} // namespace Dali
