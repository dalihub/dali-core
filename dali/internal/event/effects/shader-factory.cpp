/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/effects/shader-factory.h>

// EXTERNAL INCLUDES
#include <sstream>

// INTERNAL INCLUDES
#include <dali/devel-api/common/hash.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/dali-core-version.h>

namespace
{
constexpr uint32_t MAXIMUM_STRING_SHADER_DATA_CACHE_CLEAN_THRESHOLD = 128u;
} // namespace

namespace Dali
{
namespace Internal
{

ShaderFactory::ShaderFactory() = default;

ShaderFactory::~ShaderFactory()
{
  // Let all the cached objects destroy themselves:
  for(auto&& listPair : mShaderStringCache)
  {
    for(auto&& shaderData : listPair.second)
    {
      shaderData->Unreference();
    }
  }
}

ShaderDataPtr ShaderFactory::Load(std::string_view vertexSource, std::string_view fragmentSource, const Dali::Shader::Hint::Value hints, uint32_t renderPassTag, std::string_view name, size_t& shaderHash)
{
  // Work out the filename for the binary that the glsl source will be compiled and linked to:
  shaderHash = CalculateHash(vertexSource, fragmentSource);

  ShaderDataPtr shaderData;

  // Check a cache of previously loaded shaders as normal string:
  if(mShaderStringCache.count(shaderHash) > 0u)
  {
    const auto& cacheList = mShaderStringCache[shaderHash];
    for(std::size_t i = 0, cacheSize = cacheList.Size(); i < cacheSize; ++i)
    {
      if(cacheList[i]->GetHints() == hints &&
         cacheList[i]->GetRenderPassTag() == renderPassTag)
      {
        shaderData = cacheList[i];

        DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "Mem cache hit on string shader. Hash : \"%zu\", Hint : %d, Tag : %u\n", shaderHash, static_cast<int>(hints), renderPassTag);
        break;
      }
    }
  }

  // If memory cache failed, return a source-only ShaderData:
  if(shaderData.Get() == nullptr)
  {
    // Allocate the structure that returns the loaded shader:
    shaderData = new ShaderData(vertexSource, fragmentSource, hints, renderPassTag, name);
    shaderData->SetHashValue(shaderHash);

    if(DALI_UNLIKELY(mTotalStringCachedShadersCount >= MAXIMUM_STRING_SHADER_DATA_CACHE_CLEAN_THRESHOLD))
    {
      // Reset string cache, to avoid memory leak problem.
      ResetStringShaderData();
    }

    auto& cacheList = mShaderStringCache[shaderHash]; ///< Get or create a new cache list.

    shaderData->Reference();
    cacheList.PushBack(shaderData.Get());

    ++mTotalStringCachedShadersCount;

    DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "CACHED SHADER FOR HASH: %u, HINT: %d, TAG: %u\n", shaderHash, static_cast<int>(shaderData->GetHints()), shaderData->GetRenderPassTag());
  }

  return shaderData;
}

void ShaderFactory::Remove(ShaderData& shaderData)
{
  const size_t shaderHash = shaderData.GetHashValue();

  if(mShaderStringCache.count(shaderHash) > 0u)
  {
    auto& cacheList = mShaderStringCache[shaderHash];
    for(auto iter = cacheList.Begin(); iter != cacheList.End(); ++iter)
    {
      if((*iter)->GetHints() == shaderData.GetHints() &&
         (*iter)->GetRenderPassTag() == shaderData.GetRenderPassTag())
      {
        DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "REMOVE CACHE FOR HASH: %u, HINT: %d, TAG: %u\n", shaderHash, static_cast<int>(shaderData.GetHints()), shaderData.GetRenderPassTag());
        // Reduce reference before erase
        (*iter)->Unreference();
        cacheList.Erase(iter);

        --mTotalStringCachedShadersCount;
        break;
      }
    }
    if(cacheList.Count() == 0)
    {
      mShaderStringCache.erase(shaderHash);
    }
  }
}

void ShaderFactory::ResetStringShaderData()
{
  DALI_LOG_RELEASE_INFO("Trigger StringShaderData GC. shader : [%u]\n", mTotalStringCachedShadersCount);
  for(auto&& iter : mShaderStringCache)
  {
    auto& cacheList = iter.second;
    for(auto&& shaderData : cacheList)
    {
      shaderData->Unreference();
    }
  }

  // Reset cache after unreference.
  mShaderStringCache.clear();

  mTotalStringCachedShadersCount = 0u;
  DALI_LOG_RELEASE_INFO("StringShaderData GC done\n");
}

} // namespace Internal

} // namespace Dali
