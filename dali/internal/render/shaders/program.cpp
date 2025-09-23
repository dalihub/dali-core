/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/renderers/render-uniform-block.h>
#include <dali/internal/render/renderers/uniform-buffer-manager.h>
#include <dali/internal/render/renderers/uniform-buffer-view.h>
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
    CalculateHash(std::string_view("uModelMatrix")),
    CalculateHash(std::string_view("uMvpMatrix")),
    CalculateHash(std::string_view("uViewMatrix")),
    CalculateHash(std::string_view("uModelView")),
    CalculateHash(std::string_view("uNormalMatrix")),
    CalculateHash(std::string_view("uProjection")),
    CalculateHash(std::string_view("uScale")),
    CalculateHash(std::string_view("uSize")),
    CalculateHash(std::string_view("uColor")),
    CalculateHash(std::string_view("uActorColor"))};

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

Program* Program::New(ProgramCache& cache, const Internal::ShaderDataPtr& shaderData, std::size_t sharedUniformNamesHash, Graphics::Controller& gfxController)
{
  size_t shaderHash = shaderData->GetHashValue() ^ sharedUniformNamesHash;

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
  mLifecycleObservers(),
  mGfxController(controller),
  mProgramData(std::move(shaderData)),
  mObserverNotifying(false)
{
}

Program::~Program()
{
  mObserverNotifying = true;
  for(auto&& iter : mLifecycleObservers)
  {
    auto* observer = iter.first;
    observer->ProgramDestroyed(this);
  }
  mLifecycleObservers.clear();

  // Note : We don't need to restore mObserverNotifying to false as we are in delete the object.
  // If someone call AddObserver / RemoveObserver after this, assert.
}

void Program::BuildRequirements(
  const Graphics::Reflection&                      graphicsReflection,
  Render::UniformBufferManager&                    uniformBufferManager,
  const SceneGraph::Shader::UniformBlockContainer& sharedUniformBlockContainer)
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
      auto hashValue = CalculateHash(std::string_view(item.name.data(), item.name.size()));
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
    mReflection.emplace_back(ReflectionUniformInfo{CalculateHash(std::string_view(sampler.name.data(), sampler.name.size())), false, sampler});
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
  mUniformBlockMemoryRequirements.sharedBlock.resize(uniformBlockCount);

  mUniformBlockMemoryRequirements.blockCount            = uniformBlockCount;
  mUniformBlockMemoryRequirements.totalSizeRequired     = 0u;
  mUniformBlockMemoryRequirements.totalCpuSizeRequired  = 0u;
  mUniformBlockMemoryRequirements.totalGpuSizeRequired  = 0u;
  mUniformBlockMemoryRequirements.sharedGpuSizeRequired = 0u;

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

    bool sharedUniformUsed = false;

    // Compare and find block name in sharedUniformBlockContainer
    if(!standaloneUniformBlock)
    {
      auto uboNameHash = Dali::CalculateHash(std::string_view(uboInfo.name));
      auto iter        = sharedUniformBlockContainer.find(uboNameHash);
      if(iter != sharedUniformBlockContainer.end())
      {
        mUniformBlockMemoryRequirements.sharedBlock[i] = iter->second;
        mUniformBlockMemoryRequirements.totalSizeRequired += alignedBlockSize;
        mUniformBlockMemoryRequirements.sharedGpuSizeRequired += alignedBlockSize;

        sharedUniformUsed = true;
      }
    }

    if(!sharedUniformUsed)
    {
      mUniformBlockMemoryRequirements.sharedBlock[i] = nullptr;

      mUniformBlockMemoryRequirements.totalSizeRequired += alignedBlockSize;
      mUniformBlockMemoryRequirements.totalCpuSizeRequired += (standaloneUniformBlock) ? alignedBlockSize : 0;
      mUniformBlockMemoryRequirements.totalGpuSizeRequired += (standaloneUniformBlock) ? 0 : alignedBlockSize;
    }
  }
}

void Program::SetGraphicsProgram(
  Graphics::UniquePtr<Graphics::Program>&&         program,
  Render::UniformBufferManager&                    uniformBufferManager,
  const SceneGraph::Shader::UniformBlockContainer& uniformBlockContainer)
{
  mGfxProgram = std::move(program);
  BuildRequirements(mGfxController.GetProgramReflection(*mGfxProgram.get()), uniformBufferManager, uniformBlockContainer);
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

  int arrayIndex = 0;

  if(!name.empty() && name.back() == ']')
  {
    auto pos = name.rfind("[");
    if(pos != std::string::npos)
    {
      hash       = hashedNameNoArray;
      match      = name.substr(0, pos); // Remove subscript
      arrayIndex = atoi(&name[pos + 1]);
    }
  }

  for(const ReflectionUniformInfo& item : mReflection)
  {
    if(item.hashValue == hash)
    {
      if(!item.hasCollision || item.uniformInfo.name == match)
      {
        out = item.uniformInfo;

        // Array out of bounds
        if(item.uniformInfo.elementCount > 0 && arrayIndex >= int(item.uniformInfo.elementCount))
        {
          DALI_LOG_ERROR("Uniform %s, array index out of bound [%d >= %d]!\n",
                         item.uniformInfo.name.c_str(),
                         int(arrayIndex),
                         int(item.uniformInfo.elementCount));
          return false;
        }
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
