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
#include <dali/internal/render/shaders/render-shader.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/common/image-sampler.h>
#include <dali/internal/render/common/render-debug.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/renderers/render-uniform-block.h>
#include <dali/internal/render/shaders/program.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
namespace
{
static constexpr uint32_t DEFAULT_RENDER_PASS_TAG = 0u;

enum DirtyUpdateFlags
{
  NOT_CHECKED = 0,
  CHECKED     = 0x80,

  UPDATED_FLAG_SHIFT = 0,

  IS_UPDATED_MASK = 1 << UPDATED_FLAG_SHIFT,
};
} // namespace

Shader::~Shader()
{
}

void Shader::UpdateShaderData(ShaderDataPtr shaderData)
{
  if(shaderData->GetRenderPassTag() == DEFAULT_RENDER_PASS_TAG)
  {
    mDefaultShaderData = std::move(shaderData);
    return;
  }

  DALI_LOG_TRACE_METHOD_FMT(Debug::Filter::gShader, "%d\n", shaderData->GetHashValue());
  std::vector<Internal::ShaderDataPtr>::iterator shaderDataIterator = std::find_if(mShaderDataList.begin(), mShaderDataList.end(), [&shaderData](const Internal::ShaderDataPtr& shaderDataItem)
  { return shaderDataItem->GetRenderPassTag() == shaderData->GetRenderPassTag(); });
  if(shaderDataIterator != mShaderDataList.end())
  {
    *shaderDataIterator = std::move(shaderData);
  }
  else
  {
    mShaderDataList.push_back(std::move(shaderData));
  }
}

const ShaderDataPtr& Shader::GetShaderData(uint32_t renderPassTag) const
{
  if(renderPassTag != DEFAULT_RENDER_PASS_TAG)
  {
    for(const auto& shaderData : mShaderDataList)
    {
      if(shaderData->GetRenderPassTag() == renderPassTag)
      {
        return shaderData;
      }
    }
  }

  return mDefaultShaderData;
}

void Shader::ConnectUniformBlock(Render::UniformBlock* uniformBlock)
{
  if(uniformBlock != nullptr)
  {
    // We may end up with: hashmap per shader, whereas, we could just store hash per ub
    // and relatively quickly search vec of hashes, which will take less space if ubs
    // are shared between shaders.

    auto uniformBlockNameHash = uniformBlock->GetHash();
    DALI_ASSERT_DEBUG(mBlocks.find(uniformBlockNameHash) == mBlocks.end() && "Duplicated name of uniform connected!");
    mBlockNamesHash ^= uniformBlockNameHash;
    mBlocks.insert(std::make_pair(uniformBlockNameHash, uniformBlock));
  }
}

void Shader::DisconnectUniformBlock(Render::UniformBlock* uniformBlock)
{
  if(uniformBlock != nullptr)
  {
    auto uniformBlockNameHash = uniformBlock->GetHash();
    DALI_ASSERT_DEBUG(mBlocks.find(uniformBlockNameHash) != mBlocks.end() && "Unconnected uniform disconnect!");
    mBlockNamesHash ^= uniformBlockNameHash;
    mBlocks.erase(uniformBlockNameHash);
  }
}

const Shader::UniformBlockContainer& Shader::GetConnectedUniformBlocks() const
{
  return mBlocks;
}

std::size_t Shader::GetSharedUniformNamesHash() const
{
  return mBlockNamesHash;
}

bool Shader::IsUpdated() const
{
  // We should check Whether
  // 1. Shadoer itself's property changed
  // 2. One of connected UniformBlock's property changed.
  CheckUpdated();
  return mDirtyUpdated & IS_UPDATED_MASK;
}

void Shader::CheckUpdated() const
{
  if(mDirtyUpdated == NOT_CHECKED)
  {
    mDirtyUpdated |= CHECKED;
    bool updated = Updated();
    if(!updated)
    {
      for(const auto& uniformBlockPair : mBlocks)
      {
        const auto* uniformBlock = uniformBlockPair.second;
        if(uniformBlock->Updated())
        {
          updated = true;
          break;
        }
      }
    }
    mDirtyUpdated |= (updated) << UPDATED_FLAG_SHIFT;
  }
}

void Shader::ResetUpdated()
{
  PropertyOwner::SetUpdated(false);
  mDirtyUpdated = NOT_CHECKED;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
