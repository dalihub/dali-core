/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/rendering/shader.h>           // Dali::Shader

// INTERNAL INCLUDES
#include <dali/internal/event/rendering/shader-impl.h>  // Dali::Internal::Shader

namespace Dali
{

Shader Shader::New( const std::string& vertexShader,
                    const std::string& fragmentShader,
                    Hint::Value hints )
{
  Internal::ShaderPtr renderer = Internal::Shader::New( vertexShader, fragmentShader, hints );
  return Shader( renderer.Get() );
}

Shader::Shader()
{
}

Shader::~Shader()
{
}

Shader::Shader( const Shader& handle )
: Handle( handle )
{
}

Shader Shader::DownCast( BaseHandle handle )
{
  return Shader( dynamic_cast<Dali::Internal::Shader*>(handle.GetObjectPtr()));
}

Shader& Shader::operator=( const Shader& handle )
{
  BaseHandle::operator=( handle );
  return *this;
}

Shader::Shader( Internal::Shader* pointer )
: Handle( pointer )
{
}

} // namespace Dali
