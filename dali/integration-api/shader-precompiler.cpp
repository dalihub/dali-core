/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
 */

// CLASS HEADER
#include <dali/integration-api/shader-precompiler.h>

// INTERNAL HEADER
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Integration
{

std::unique_ptr<ShaderPrecompiler> ShaderPrecompiler::mInstance = nullptr;
std::once_flag ShaderPrecompiler::mOnceFlag;

ShaderPrecompiler::ShaderPrecompiler()
: mRawShaderData(),
  mPrecompiled(false),
  mEnabled(false)
{
}

ShaderPrecompiler& ShaderPrecompiler::Get()
{
  std::call_once(mOnceFlag, []()
                 { mInstance.reset(new ShaderPrecompiler); });

  return *(mInstance.get());
}

void ShaderPrecompiler::GetPrecompileShaderList(RawShaderData& shaders)
{
  ConditionalWait::ScopedLock lock(mConditionalWait);
  if(!IsReady())
  {
    DALI_LOG_RELEASE_INFO("Precompiled shader list is not ready yet, need to wait \n");
    mConditionalWait.Wait(lock);
  }

  // move shader list
  shaders = mRawShaderData;
}

void ShaderPrecompiler::SavePrecomipleShaderList(RawShaderData& shaders)
{
  ConditionalWait::ScopedLock lock(mConditionalWait);
  mRawShaderData.vertexPrefix = shaders.vertexPrefix;
  mRawShaderData.fragmentPrefix = shaders.fragmentPrefix;
  mRawShaderData.vertexShader = shaders.vertexShader;
  mRawShaderData.fragmentShader = shaders.fragmentShader;
  mRawShaderData.shaderCount = shaders.shaderCount;

  mPrecompiled = true;
  mConditionalWait.Notify(lock);
}

bool ShaderPrecompiler::IsReady() const
{
  return mPrecompiled;
}

void ShaderPrecompiler::Enable()
{
  mEnabled = true;
}

bool ShaderPrecompiler::IsEnable()
{
  return mEnabled;
}


} // namespace Integration
} // namespace Dali
