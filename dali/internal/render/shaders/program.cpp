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
#include <dali/internal/render/shaders/program.h>

// EXTERNAL INCLUDES
#include <cstring>
#include <iomanip>
#include <map>

// INTERNAL INCLUDES
#include <dali/devel-api/common/hash.h>
#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-program.h>
#include <dali/graphics-api/graphics-reflection.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/gl-defines.h>
#include <dali/internal/common/shader-data.h>
#include <dali/internal/render/common/performance-monitor.h>
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
const char* const gStdUniforms[Program::UNIFORM_TYPE_LAST] =
  {
    "uMvpMatrix",    // UNIFORM_MVP_MATRIX
    "uModelView",    // UNIFORM_MODELVIEW_MATRIX
    "uProjection",   // UNIFORM_PROJECTION_MATRIX
    "uModelMatrix",  // UNIFORM_MODEL_MATRIX,
    "uViewMatrix",   // UNIFORM_VIEW_MATRIX,
    "uNormalMatrix", // UNIFORM_NORMAL_MATRIX
    "uColor",        // UNIFORM_COLOR
    "sTexture",      // UNIFORM_SAMPLER
    "sTextureRect",  // UNIFORM_SAMPLER_RECT
    "sEffect",       // UNIFORM_EFFECT_SAMPLER
    "uSize"          // UNIFORM_SIZE
};

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
    CalculateHash(std::string("uColor"))};

} // namespace

// IMPLEMENTATION

Program* Program::New(ProgramCache& cache, Internal::ShaderDataPtr shaderData, Graphics::Controller& gfxController, Graphics::UniquePtr<Graphics::Program>&& gfxProgram, bool modifiesGeometry)
{
  uint32_t programId{0u};

  // Get program id and use it as hash for the cache
  // in order to maintain current functionality as long as needed
  gfxController.GetProgramParameter(*gfxProgram, 1, &programId);

  size_t shaderHash = programId;

  Program* program = cache.GetProgram(shaderHash);

  if(nullptr == program)
  {
    // program not found so create it
    program = new Program(cache, shaderData, gfxController, std::move(gfxProgram), modifiesGeometry);
    cache.AddProgram(shaderHash, program);
  }

  return program;
}

uint32_t Program::RegisterUniform(ConstString name)
{
  uint32_t index = 0;
  // find the value from cache
  for(; index < static_cast<uint32_t>(mUniformLocations.size()); ++index)
  {
    if(mUniformLocations[index].first == name)
    {
      // name found so return index
      return index;
    }
  }
  // if we get here, index is one past end so push back the new name
  mUniformLocations.push_back(std::make_pair(name, UNIFORM_NOT_QUERIED));
  return index;
}

bool Program::ModifiesGeometry()
{
  return mModifiesGeometry;
}

Program::Program(ProgramCache& cache, Internal::ShaderDataPtr shaderData, Graphics::Controller& controller, Graphics::UniquePtr<Graphics::Program>&& gfxProgram, bool modifiesGeometry)
: mCache(cache),
  mProjectionMatrix(nullptr),
  mViewMatrix(nullptr),
  mProgramId(0),
  mGfxProgram(std::move(gfxProgram)),
  mGfxController(controller),
  mProgramData(shaderData),
  mModifiesGeometry(modifiesGeometry)
{
  // reserve space for standard uniforms
  mUniformLocations.reserve(UNIFORM_TYPE_LAST);

  // reset built in uniform names in cache
  for(uint32_t i = 0; i < UNIFORM_TYPE_LAST; ++i)
  {
    RegisterUniform(ConstString(gStdUniforms[i]));
  }

  // reset values
  ResetUniformCache();

  // Get program id and use it as hash for the cache
  // in order to maintain current functionality as long as needed
  mGfxController.GetProgramParameter(*mGfxProgram, 1, &mProgramId);

  BuildReflection(controller.GetProgramReflection(*mGfxProgram.get()));
}

Program::~Program()
{
}

void Program::ResetUniformCache()
{
  // reset all gl uniform locations
  for(uint32_t i = 0; i < mUniformLocations.size(); ++i)
  {
    // reset gl program locations and names
    mUniformLocations[i].second = UNIFORM_NOT_QUERIED;
  }

  mSamplerUniformLocations.clear();

  // reset uniform caches
  mSizeUniformCache.x = mSizeUniformCache.y = mSizeUniformCache.z = 0.f;

  for(uint32_t i = 0; i < MAX_UNIFORM_CACHE_SIZE; ++i)
  {
    // GL initializes uniforms to 0
    mUniformCacheInt[i]       = 0;
    mUniformCacheFloat[i]     = 0.0f;
    mUniformCacheFloat2[i][0] = 0.0f;
    mUniformCacheFloat2[i][1] = 0.0f;
    mUniformCacheFloat4[i][0] = 0.0f;
    mUniformCacheFloat4[i][1] = 0.0f;
    mUniformCacheFloat4[i][2] = 0.0f;
    mUniformCacheFloat4[i][3] = 0.0f;
  }
}

void Program::BuildReflection(const Graphics::Reflection& graphicsReflection)
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
      auto hashValue = CalculateHash(item.name);
      mReflection.emplace_back(ReflectionUniformInfo{hashValue, false, item});

      // update buffer index
      mReflection.back().uniformInfo.bufferIndex = i;

      // Update default uniforms
      for(auto i = 0u; i < NUMBER_OF_DEFAULT_UNIFORMS; ++i)
      {
        if(hashValue == DEFAULT_UNIFORM_HASHTABLE[i])
        {
          mReflectionDefaultUniforms[i] = mReflection.back();
          break;
        }
      }
    }
  }

  // add samplers
  auto samplers = graphicsReflection.GetSamplers();
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
}

bool Program::GetUniform(const std::string& name, size_t hashedName, Graphics::UniformInfo& out) const
{
  if(mReflection.empty())
  {
    return false;
  }

  hashedName = !hashedName ? CalculateHash(name, '[') : hashedName;

  for(const ReflectionUniformInfo& item : mReflection)
  {
    if(item.hashValue == hashedName)
    {
      if(!item.hasCollision || item.uniformInfo.name == name)
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
