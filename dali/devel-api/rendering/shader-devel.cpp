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

#include <dali/devel-api/rendering/shader-devel.h>
#include <dali/internal/event/rendering/shader-impl.h>

namespace Dali
{
namespace DevelShader
{

Dali::Shader New( std::vector<char>& vertexShader,
                  std::vector<char>& fragmentShader,
                  ShaderLanguage language,
                  const Property::Map& specializationConstants )
{
  auto impl = Dali::Internal::Shader::New( vertexShader, fragmentShader, language, specializationConstants );
  return Shader( impl.Get() );
}


}
}

