/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/shaders/render-shader.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/common/image-sampler.h>
#include <dali/internal/render/common/render-debug.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/shaders/program.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
Shader::Shader(Dali::Shader::Hint::Value& hints)
: mHints(hints)
{
}

Shader::~Shader()
{
}

void Shader::SetShaderData(ShaderDataPtr shaderData)
{
  DALI_LOG_TRACE_METHOD_FMT(Debug::Filter::gShader, "%d\n", shaderData->GetHashValue());

  mShaderData = shaderData;
}

ShaderDataPtr Shader::GetShaderData() const
{
  return mShaderData;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
