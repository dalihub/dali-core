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
#include <dali/internal/render/common/shared-uniform-buffer-view-container.h>

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

// INTERNAL INCLUDES
#include <dali/internal/render/renderers/render-uniform-block.h>
#include <dali/internal/render/renderers/uniform-buffer-manager.h>
#include <dali/internal/render/renderers/uniform-buffer-view.h>
#include <dali/internal/render/renderers/uniform-buffer.h>
#include <dali/internal/render/shaders/program.h>

namespace Dali::Internal
{
#if defined(DEBUG_ENABLED)
namespace
{
Debug::Filter* gLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_RENDER_MANAGER");
} // unnamed namespace
#endif

struct SharedUniformBufferViewContainer::Impl
{
public:
  Impl()
  : mSharedUniformBlockBufferViews()
  {
  }
  ~Impl() = default;

  struct ProgramUniformBlockPair
  {
    ProgramUniformBlockPair(const Render::UniformBlock::ProgramIndex programIndex, Render::UniformBlock* sharedUniformBlock, uint32_t blockSize)
    : programIndex(programIndex),
      sharedUniformBlock(sharedUniformBlock),
      blockSize(blockSize)
    {
    }

    ProgramUniformBlockPair() = default;

    bool operator==(const ProgramUniformBlockPair& rhs) const
    {
      return programIndex == rhs.programIndex && sharedUniformBlock == rhs.sharedUniformBlock;
    }

    struct ProgramUniformBlockPairHash
    {
      // Reference by : https://stackoverflow.com/a/21062236
      std::size_t operator()(ProgramUniformBlockPair const& key) const noexcept
      {
        constexpr std::size_t uniformBlockShift = Dali::Log<1 + sizeof(Render::UniformBlock)>::value;
        constexpr std::size_t zitterShift       = sizeof(std::size_t) * 4; // zitter shift to avoid hash collision

        return ((static_cast<std::size_t>(key.programIndex)) << zitterShift) ^
               (reinterpret_cast<std::size_t>(key.sharedUniformBlock) >> uniformBlockShift);
      }
    };

    const Render::UniformBlock::ProgramIndex programIndex{0u};
    Render::UniformBlock*                    sharedUniformBlock{nullptr};

    const uint32_t blockSize{0u}; ///< Size of block for given pair. We don't need to compare this value for check hash.
  };

  using UniformBufferViewContainer = std::unordered_map<ProgramUniformBlockPair, Dali::Graphics::UniquePtr<Render::UniformBufferView>, ProgramUniformBlockPair::ProgramUniformBlockPairHash>;
  UniformBufferViewContainer mSharedUniformBlockBufferViews{};
};

SharedUniformBufferViewContainer::SharedUniformBufferViewContainer()
: mImpl(new Impl())
{
}

SharedUniformBufferViewContainer::~SharedUniformBufferViewContainer() = default;

bool SharedUniformBufferViewContainer::RegisterSharedUniformBlockAndPrograms(const Program& program, Render::UniformBlock& sharedUniformBlock, uint32_t blockSize)
{
  Render::UniformBlock::ProgramIndex programIndex = sharedUniformBlock.GetProgramIndex(program);

  auto ret = mImpl->mSharedUniformBlockBufferViews.insert({Impl::ProgramUniformBlockPair(programIndex, &sharedUniformBlock, blockSize), nullptr});

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Registered : %zu, Program : %p, UBO : %p, blockSize : %u -- Registered : %d\n", mImpl->mSharedUniformBlockBufferViews.size(), &program, &sharedUniformBlock, blockSize, ret.second);

  return ret.second;
}

void SharedUniformBufferViewContainer::Initialize(BufferIndex renderBufferIndex, Render::UniformBufferManager& uniformBufferManager)
{
#if defined(DEBUG_ENABLED)
  uint32_t totalSize                   = 0u;
  uint32_t totalUniformBufferViewCount = 0u;
#endif

  static Render::UniformBufferView* sDummyView = nullptr;

  for(auto& item : mImpl->mSharedUniformBlockBufferViews)
  {
    const auto& programIndex       = item.first.programIndex;
    auto&       sharedUniformBlock = *item.first.sharedUniformBlock;
    uint32_t    blockSize          = item.first.blockSize;

    item.second = uniformBufferManager.CreateUniformBufferView(sDummyView, blockSize, false);

    if(DALI_UNLIKELY(!item.second))
    {
      DALI_LOG_ERROR("Fail to create shared uniform buffer view!\n");
      continue;
    }

#if defined(DEBUG_ENABLED)
    totalSize += blockSize;
    ++totalUniformBufferViewCount;
#endif

    auto& ubo = *(item.second.get());

    // Write to the buffer view here, by value at sharedUniformBlock.
    sharedUniformBlock.WriteUniforms(renderBufferIndex, programIndex, ubo);
  }
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Registered : %zu, SharedUniformBufferView count : %u, total block size:%u\n", mImpl->mSharedUniformBlockBufferViews.size(), totalUniformBufferViewCount, totalSize);
}

Render::UniformBufferView* SharedUniformBufferViewContainer::GetSharedUniformBlockBufferView(const Program& program, Render::UniformBlock& sharedUniformBlock) const
{
  Render::UniformBlock::ProgramIndex programIndex = sharedUniformBlock.GetProgramIndex(program);

  auto key  = Impl::ProgramUniformBlockPair(programIndex, &sharedUniformBlock, 0u);
  auto iter = mImpl->mSharedUniformBlockBufferViews.find(key);
  if(iter != mImpl->mSharedUniformBlockBufferViews.end())
  {
    return iter->second.get();
  }
  return nullptr;
}

void SharedUniformBufferViewContainer::Finalize()
{
  mImpl->mSharedUniformBlockBufferViews.clear();
  mImpl->mSharedUniformBlockBufferViews.rehash(0);
}

} // namespace Dali::Internal
