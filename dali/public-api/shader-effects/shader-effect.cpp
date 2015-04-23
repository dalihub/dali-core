/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/shader-effects/shader-effect.h>

// INTERNAL INCLUDES
#include <dali/internal/event/effects/shader-effect-impl.h>
#include <dali/internal/event/animation/constraint-impl.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{
const float ShaderEffect::DEFAULT_GRID_DENSITY = 40;

ShaderEffect::Extension::Extension()
{
}

ShaderEffect::Extension::~Extension()
{
}

ShaderEffect::ShaderEffect()
{
}

ShaderEffect::ShaderEffect(Internal::ShaderEffect* internal)
: Handle(internal)
{
}

ShaderEffect::~ShaderEffect()
{
}

ShaderEffect::ShaderEffect(const ShaderEffect& handle)
: Handle(handle)
{
}

ShaderEffect& ShaderEffect::operator=(const ShaderEffect& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

ShaderEffect ShaderEffect::New( const std::string& vertexShader, const std::string& fragmentShader, GeometryType type, GeometryHints hints)
{
  Internal::ShaderEffectPtr internal = Internal::ShaderEffect::New( hints );

  internal->SetPrograms( type, vertexShader, fragmentShader );

  return ShaderEffect( internal.Get() );
}

ShaderEffect ShaderEffect::NewWithPrefix( const std::string& vertexShaderPrefix,
                                          const std::string& vertexShader,
                                          const std::string& fragmentShaderPrefix,
                                          const std::string& fragmentShader,
                                          GeometryType type,
                                          GeometryHints hints)
{
  Internal::ShaderEffectPtr internal = Internal::ShaderEffect::New( hints );

  internal->SetPrograms( type, vertexShaderPrefix, fragmentShaderPrefix, vertexShader, fragmentShader );

  return ShaderEffect( internal.Get() );
}

ShaderEffect ShaderEffect::DownCast( BaseHandle handle )
{
  return ShaderEffect( dynamic_cast<Dali::Internal::ShaderEffect*>(handle.GetObjectPtr()) );
}

void ShaderEffect::SetEffectImage( Image image )
{
  GetImplementation(*this).SetEffectImage( image );
}

void ShaderEffect::SetUniform( const std::string& name, float value, UniformCoordinateType uniformCoordinateType )
{
  GetImplementation(*this).SetUniform( name, value, uniformCoordinateType );
}

void ShaderEffect::SetUniform( const std::string& name, Vector2 value, UniformCoordinateType uniformCoordinateType )
{
  GetImplementation(*this).SetUniform( name, value, uniformCoordinateType );
}

void ShaderEffect::SetUniform( const std::string& name, Vector3 value, UniformCoordinateType uniformCoordinateType )
{
  GetImplementation(*this).SetUniform( name, value, uniformCoordinateType );
}

void ShaderEffect::SetUniform( const std::string& name, Vector4 value, UniformCoordinateType uniformCoordinateType )
{
  GetImplementation(*this).SetUniform( name, value, uniformCoordinateType );
}

void ShaderEffect::SetUniform( const std::string& name, const Matrix& value, UniformCoordinateType uniformCoordinateType )
{
  GetImplementation(*this).SetUniform( name, value, uniformCoordinateType );
}

void ShaderEffect::SetUniform( const std::string& name, const Matrix3& value, UniformCoordinateType uniformCoordinateType )
{
  GetImplementation(*this).SetUniform( name, value, uniformCoordinateType );
}

void ShaderEffect::AttachExtension( ShaderEffect::Extension *object )
{
  GetImplementation(*this).AttachExtension( object );
}

ShaderEffect::Extension& ShaderEffect::GetExtension()
{
  return GetImplementation(*this).GetExtension();
}

const ShaderEffect::Extension& ShaderEffect::GetExtension() const
{
  return GetImplementation(*this).GetExtension();
}

} // namespace Dali
