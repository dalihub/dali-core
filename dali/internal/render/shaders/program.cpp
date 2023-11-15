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
 *
 */

// CLASS HEADER
#include <dali/internal/render/shaders/program.h>

// EXTERNAL INCLUDES
#include <cstring>
#include <map>

// INTERNAL INCLUDES
#include <dali/devel-api/common/hash.h>
#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-program.h>
#include <dali/graphics-api/graphics-reflection.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/common/shader-data.h>
#include <dali/internal/render/common/performance-monitor.h>
#include <dali/internal/render/renderers/uniform-buffer-manager.h>
#include <dali/internal/render/shaders/program-cache.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/common/dali-vector.h>

namespace Dali
{
namespace Internal
{
// LOCAL STUFF
namespace
{
const unsigned int NUMBER_OF_DEFAULT_UNIFORMS = static_cast<unsigned int>(Program::DefaultUniformIndex::COUNT);

/**
 * List of all default uniforms used for quicker lookup
 */
size_t DEFAULT_UNIFORM_HASHTABLE[NUMBER_OF_DEFAULT_UNIFORMS] =
  {
    CalculateHash(std::string("uModelMatrix")),
    CalculateHash(std::string("uMvpMatrix")),
    CalculateHash(std::string("uViewMatrix")),
    CalculateHash(std::string("uModelView")),
    CalculateHash(std::string("uNormalMatrix")),
    CalculateHash(std::string("uProjection")),
    CalculateHash(std::string("uSize")),
    CalculateHash(std::string("uColor")),
    CalculateHash(std::string("uActorColor"))};

/**
 * Helper function to calculate the correct alignment of data for uniform buffers
 * @param dataSize size of uniform buffer
 * @return size of data aligned to given size
 */
inline uint32_t AlignSize(uint32_t dataSize, uint32_t alignSize)
{
  return ((dataSize / alignSize) + ((dataSize % alignSize) ? 1u : 0u)) * alignSize;
}

} // namespace

// IMPLEMENTATION

Program* Program::New(ProgramCache& cache, const Internal::ShaderDataPtr& shaderData, Graphics::Controller& gfxController)
{
  size_t shaderHash = shaderData->GetHashValue();

  Program* program = cache.GetProgram(shaderHash);

  if(nullptr == program)
  {
    // program not found so create it
    program = new Program(cache, shaderData, gfxController);

    DALI_LOG_INFO(Debug::Filter::gShader, Debug::Verbose, "Program::New() created a unique program:\n  VertexShader:\n%s\n\n  FragShader:\n%s\n", shaderData->GetVertexShader(), shaderData->GetFragmentShader());
    cache.AddProgram(shaderHash, program);
  }

  return program;
}

Program::Program(ProgramCache& cache, Internal::ShaderDataPtr shaderData, Graphics::Controller& controller)
: mCache(cache),
  mGfxProgram(nullptr),
  mGfxController(controller),
  mProgramData(std::move(shaderData))
{
}

Program::~Program() = default;

void Program::BuildReflection(const Graphics::Reflection& graphicsReflection, Render::UniformBufferManager& uniformBufferManager)
{
  mReflectionDefaultUniforms.clear();
  mReflectionDefaultUniforms.resize(NUMBER_OF_DEFAULT_UNIFORMS);

  auto uniformBlockCount = graphicsReflection.GetUniformBlockCount();

  // add uniform block fields
  for(auto i = 0u; i < uniformBlockCount; ++i)
  {
    Graphics::UniformBlockInfo uboInfo;
    graphicsReflection.GetUniformBlock(i, uboInfo);

    // for each member store data
    for(const auto& item : uboInfo.members)
    {
      // Add a hash for the whole name.
      //
      // If the name represents an array of basic types, it won't contain an index
      // operator "[",NN,"]".
      //
      // If the name represents an element in an array of structs, it will contain an
      // index operator, but should be hashed in full.
      auto hashValue = CalculateHash(item.name);
      mReflection.emplace_back(ReflectionUniformInfo{hashValue, false, item});

      // update buffer index
      mReflection.back().uniformInfo.bufferIndex = i;

      // Update default uniforms
      for(auto j = 0u; j < NUMBER_OF_DEFAULT_UNIFORMS; ++j)
      {
        if(hashValue == DEFAULT_UNIFORM_HASHTABLE[j])
        {
          mReflectionDefaultUniforms[j] = mReflection.back();
          break;
        }
      }
    }
  }

  // add samplers
  auto samplers = graphicsReflection.GetSamplers(); // Only holds first element of arrays without [].
  for(const auto& sampler : samplers)
  {
    mReflection.emplace_back(ReflectionUniformInfo{CalculateHash(sampler.name), false, sampler});
  }

  // check for potential collisions
  std::map<size_t, bool> hashTest;
  bool                   hasCollisions(false);
  for(auto&& item : mReflection)
  {
    if(hashTest.find(item.hashValue) == hashTest.end())
    {
      hashTest[item.hashValue] = false;
    }
    else
    {
      hashTest[item.hashValue] = true;
      hasCollisions            = true;
    }
  }

  // update collision flag for further use
  if(hasCollisions)
  {
    for(auto&& item : mReflection)
    {
      item.hasCollision = hashTest[item.hashValue];
    }
  }

  mUniformBlockMemoryRequirements.blockSize.resize(uniformBlockCount);
  mUniformBlockMemoryRequirements.blockSizeAligned.resize(uniformBlockCount);
  mUniformBlockMemoryRequirements.blockCount           = uniformBlockCount;
  mUniformBlockMemoryRequirements.totalSizeRequired    = 0u;
  mUniformBlockMemoryRequirements.totalCpuSizeRequired = 0u;
  mUniformBlockMemoryRequirements.totalGpuSizeRequired = 0u;

  for(auto i = 0u; i < uniformBlockCount; ++i)
  {
    Graphics::UniformBlockInfo uboInfo;
    graphicsReflection.GetUniformBlock(i, uboInfo);
    bool standaloneUniformBlock = (i == 0);

    auto     blockSize        = graphicsReflection.GetUniformBlockSize(i);
    uint32_t blockAlignment   = uniformBufferManager.GetUniformBlockAlignment(standaloneUniformBlock);
    auto     alignedBlockSize = AlignSize(blockSize, blockAlignment);

    mUniformBlockMemoryRequirements.blockSize[i]        = blockSize;
    mUniformBlockMemoryRequirements.blockSizeAligned[i] = alignedBlockSize;

    mUniformBlockMemoryRequirements.totalSizeRequired += alignedBlockSize;
    mUniformBlockMemoryRequirements.totalCpuSizeRequired += (standaloneUniformBlock) ? alignedBlockSize : 0;
    mUniformBlockMemoryRequirements.totalGpuSizeRequired += (standaloneUniformBlock) ? 0 : alignedBlockSize;
  }
}

void Program::SetGraphicsProgram(Graphics::UniquePtr<Graphics::Program>&& program, Render::UniformBufferManager& uniformBufferManager)
{
  mGfxProgram = std::move(program);
  BuildReflection(mGfxController.GetProgramReflection(*mGfxProgram.get()), uniformBufferManager);
}

bool Program::GetUniform(const std::string_view& name, Hash hashedName, Hash hashedNameNoArray, Graphics::UniformInfo& out) const
{
  if(mReflection.empty())
  {
    return false;
  }
  DALI_ASSERT_DEBUG(hashedName != 0 && "GetUniform() hash is not set");

  // If name contains a "]", but has nothing after, it's an element in an array,
  // The reflection doesn't contain such elements, it only contains the name without square brackets
  // Use the hash without array subscript.

  // If the name contains a "]" anywhere but the end, it's a structure element. The reflection
  // does contain such elements, so use normal hash.
  Hash             hash  = hashedName;
  std::string_view match = name;

  if(!name.empty() && name.back() == ']')
  {
    hash     = hashedNameNoArray;
    auto pos = name.rfind("[");
    match    = name.substr(0, pos - 1); // Remove subscript
  }

  for(const ReflectionUniformInfo& item : mReflection)
  {
    if(item.hashValue == hash)
    {
      if(!item.hasCollision || item.uniformInfo.name == match)
      {
        out = item.uniformInfo;
        return true;
      }
      else
      {
        return false;
      }
    }
  }
  return false;
}

const Graphics::UniformInfo* Program::GetDefaultUniform(DefaultUniformIndex defaultUniformIndex) const
{
  if(mReflectionDefaultUniforms.empty())
  {
    return nullptr;
  }

  const auto value = &mReflectionDefaultUniforms[static_cast<uint32_t>(defaultUniformIndex)];
  return &value->uniformInfo;
}

} // namespace Internal

} // namespace Dali
