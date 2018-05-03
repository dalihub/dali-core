#ifndef DALI_SHADER_DEVEL_H
#define DALI_SHADER_DEVEL_H

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

#include <dali/public-api/rendering/shader.h>
#include <vector>

namespace Dali
{
namespace DevelShader
{

enum class ShaderLanguage
{
  AUTO_DETECT,
  SPIRV_1_0,
  GLSL_100_ES,
  GLSL_300_ES,
  GLSL_450,
  HLSL
};

/**
 * Creates shader with binary or text source
 * @param vertexShader
 * @param fragmentShader
 * @param language
 * @param specializationConstants
 * @return
 */
DALI_IMPORT_API Dali::Shader New( std::vector<char>& vertexShader, std::vector<char>& fragmentShader, ShaderLanguage language, const Property::Map& specializationConstants );

template<class T>
DALI_IMPORT_API Dali::Shader New( std::vector<T>& vertexShader, std::vector<T>& fragmentShader, ShaderLanguage language, const Property::Map& specializationConstants )
{
  auto vsh = std::vector<char>();
  auto fsh = std::vector<char>();
  vsh.insert( vsh.begin(), reinterpret_cast<char*>(&vertexShader[0]), reinterpret_cast<char*>(&vertexShader[0])+vertexShader.size()*sizeof(T) );
  fsh.insert( fsh.begin(), reinterpret_cast<char*>(&fragmentShader[0]), reinterpret_cast<char*>(&fragmentShader[0])+fragmentShader.size()*sizeof(T) );
  return New( vsh, fsh, language, specializationConstants );
}

template<class T>
DALI_IMPORT_API Dali::Shader New( std::vector<T>&& vertexShader, std::vector<T>&& fragmentShader, ShaderLanguage language, const Property::Map& specializationConstants )
{
  auto vsh = std::vector<char>();
  auto fsh = std::vector<char>();
  vsh.insert( vsh.begin(), reinterpret_cast<char*>(&vertexShader[0]), reinterpret_cast<char*>(&vertexShader[0])+vertexShader.size()*sizeof(T) );
  fsh.insert( fsh.begin(), reinterpret_cast<char*>(&fragmentShader[0]), reinterpret_cast<char*>(&fragmentShader[0])+fragmentShader.size()*sizeof(T) );
  return New( vsh, fsh, language, specializationConstants );
}


} // DevelShader

} // Dali

#endif // DALI_SHADER_DEVEL_H
