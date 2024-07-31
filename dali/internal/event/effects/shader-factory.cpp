/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
const char* VERSION_SEPARATOR = "-";
const char* SHADER_SUFFIX     = ".dali-bin";
} // namespace

namespace Dali
{
namespace Internal
{
namespace
{
/**
 * @brief Generates a filename for a shader binary based on the hash value passed in.
 * @param[in] shaderHash A hash over shader sources.
 * @param[out] filename A string to overwrite with the filename.
 */
void shaderBinaryFilename(size_t shaderHash, std::string& filename)
{
  std::stringstream binaryShaderFilenameBuilder(std::ios_base::out);
  binaryShaderFilenameBuilder << CORE_MAJOR_VERSION << VERSION_SEPARATOR << CORE_MINOR_VERSION << VERSION_SEPARATOR << CORE_MICRO_VERSION << VERSION_SEPARATOR
                              << shaderHash
                              << SHADER_SUFFIX;
  filename = binaryShaderFilenameBuilder.str();
}

} // namespace

ShaderFactory::ShaderFactory() = default;

ShaderFactory::~ShaderFactory()
{
  // Let all the cached objects destroy themselves:
  for(auto&& listPair : mShaderBinaryCache)
  {
    for(auto&& shaderData : listPair.second)
    {
      shaderData->Unreference();
    }
  }
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

  /// Check a cache of previously loaded shaders:
  if(mShaderBinaryCache.count(shaderHash) > 0u)
  {
    const auto& cacheList = mShaderBinaryCache[shaderHash];
    for(std::size_t i = 0, cacheSize = cacheList.Count(); i < cacheSize; ++i)
    {
      if(cacheList[i]->GetHints() == hints &&
         cacheList[i]->GetRenderPassTag() == renderPassTag)
      {
        shaderData = cacheList[i];

#ifdef DEBUG_ENABLED
        if(Debug::Filter::gShader->IsEnabledFor(Debug::General))
        {
          std::string binaryShaderFilename;
          shaderBinaryFilename(shaderHash, binaryShaderFilename);

          DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "Mem cache hit on path: \"%s\", Hint : %d, Tag : %u\n", binaryShaderFilename.c_str(), static_cast<int>(hints), renderPassTag);
        }
#endif
        break;
      }
    }
  }

  /// Check a cache of previously loaded shaders as normal string:
  if(shaderData.Get() == nullptr && mShaderStringCache.count(shaderHash) > 0u)
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

  // If memory cache failed check the file system for a binary or return a source-only ShaderData:
  if(shaderData.Get() == nullptr)
  {
    std::string binaryShaderFilename;
    shaderBinaryFilename(shaderHash, binaryShaderFilename);

    // Allocate the structure that returns the loaded shader:
    shaderData = new ShaderData(vertexSource, fragmentSource, hints, renderPassTag, name);
    shaderData->SetHashValue(shaderHash);
    shaderData->GetBuffer().Clear();

    // Try to load the binary (this will fail if the shader source has never been compiled before):
    ThreadLocalStorage&               tls                 = ThreadLocalStorage::Get();
    Integration::PlatformAbstraction& platformAbstraction = tls.GetPlatformAbstraction();
    const bool                        loaded              = platformAbstraction.LoadShaderBinaryFile(binaryShaderFilename, shaderData->GetBuffer());

    MemoryCacheInsert(*shaderData, loaded);

    DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, loaded ? "loaded on path: \"%s\"\n" : "failed to load on path: \"%s\"\n", binaryShaderFilename.c_str());
  }

  return shaderData;
}

void ShaderFactory::SaveBinary(Internal::ShaderDataPtr shaderData)
{
  // Save the binary to the file system:
  std::string binaryShaderFilename;
  shaderBinaryFilename(shaderData->GetHashValue(), binaryShaderFilename);

  ThreadLocalStorage&               tls                 = ThreadLocalStorage::Get();
  Integration::PlatformAbstraction& platformAbstraction = tls.GetPlatformAbstraction();
  const bool                        saved               = platformAbstraction.SaveShaderBinaryFile(binaryShaderFilename, &shaderData->GetBuffer()[0], static_cast<unsigned int>(shaderData->GetBufferSize())); // don't expect buffer larger than unsigned int

  // Save the binary into to memory cache:
  MemoryCacheInsert(*shaderData, saved);

  DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, saved ? "Saved to file: %s\n" : "Save to file failed: %s\n", binaryShaderFilename.c_str());
}

void ShaderFactory::MemoryCacheInsert(ShaderData& shaderData, const bool isBinaryCached)
{
  const size_t shaderHash = shaderData.GetHashValue();

  // Save the binary into to memory cache:
  if(isBinaryCached)
  {
    DALI_ASSERT_DEBUG(shaderData.GetBufferSize() > 0);

    // Remove shaderdata from string cache if it exists:
    RemoveStringShaderData(shaderData);

    auto& cacheList = mShaderBinaryCache[shaderHash]; ///< Get or create a new cache list.

    cacheList.Reserve(cacheList.Size() + 1); // Make sure the push won't throw after we inc the ref count.
    shaderData.Reference();
    cacheList.PushBack(&shaderData);
    DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "CACHED BINARY FOR HASH: %u, HINT: %d, TAG: %u\n", shaderHash, static_cast<int>(shaderData.GetHints()), shaderData.GetRenderPassTag());
  }
  else
  {
    auto& cacheList = mShaderStringCache[shaderHash]; ///< Get or create a new cache list.

    // Ignore shaderdata with string if it already exists:
    for(auto iter = cacheList.Begin(); iter != cacheList.End(); ++iter)
    {
      if((*iter)->GetHints() == shaderData.GetHints() &&
         (*iter)->GetRenderPassTag() == shaderData.GetRenderPassTag())
      {
        DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "ALREADY CACHED NON-BINARY CACHE FOR HASH: %u, HINT: %d, TAG: %u\n", shaderHash, static_cast<int>(shaderData.GetHints()), shaderData.GetRenderPassTag());
        return;
      }
    }
    shaderData.Reference();
    cacheList.PushBack(&shaderData);
    DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "CACHED NON-BINARY SHADER FOR HASH: %u, HINT: %d, TAG: %u\n", shaderHash, static_cast<int>(shaderData.GetHints()), shaderData.GetRenderPassTag());
  }
}

void ShaderFactory::RemoveStringShaderData(ShaderData& shaderData)
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
        DALI_LOG_INFO(Debug::Filter::gShader, Debug::General, "REMOVE NON-BINARY CACHE FOR HASH: %u, HINT: %d, TAG: %u\n", shaderHash, static_cast<int>(shaderData.GetHints()), shaderData.GetRenderPassTag());
        // Reduce reference before erase
        (*iter)->Unreference();
        cacheList.Erase(iter);
        break;
      }
    }
    if(cacheList.Count() == 0)
    {
      mShaderStringCache.erase(shaderHash);
    }
  }
}

} // namespace Internal

} // namespace Dali
