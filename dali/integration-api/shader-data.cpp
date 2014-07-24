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
#include <dali/integration-api/shader-data.h>

namespace Dali
{

namespace Integration
{

ShaderData::ShaderData(const std::string& vertexSource, const std::string& fragmentSource)
: mShaderHash(0),
  vertexShader(vertexSource),
  fragmentShader(fragmentSource)
{
}

ShaderData::~ShaderData()
{
  buffer.clear();
}

bool ShaderData::HasBinary() const
{
  return 0 != buffer.size();
}

void ShaderData::AllocateBuffer(const unsigned int size)
{
  if( size > buffer.size() )
  {
    buffer.resize( size );
  }
}

} //namespace Integration

} //namespace Dali
