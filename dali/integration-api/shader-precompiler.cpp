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
: mRawShaderList(),
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

void ShaderPrecompiler::GetPrecompileShaderList(std::vector<RawShaderData>& shaderList)
{
  // move shader list
  shaderList = mRawShaderList;
}

void ShaderPrecompiler::SavePrecomipleShaderList(std::vector<RawShaderData>& shaderList)
{
  mRawShaderList = shaderList;
  mPrecompiled = true;
  StopPrecompile();
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

void ShaderPrecompiler::WaitPrecompileList()
{
  ConditionalWait::ScopedLock lock(mConditionalWait);
  {
    Dali::Mutex::ScopedLock mutexLock(mMutex);
    if(!mNeedsSleep)
    {
      return;
    }
  }

  mConditionalWait.Wait(lock);
}

void ShaderPrecompiler::StopPrecompile()
{
  ConditionalWait::ScopedLock lock(mConditionalWait);
  Dali::Mutex::ScopedLock mutexLock(mMutex);
  mNeedsSleep = false;
  mConditionalWait.Notify(lock);
}


} // namespace Integration
} // namespace Dali
