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
    ProgramUniformBlockPair(const Render::UniformBlock::ProgramIndex programIndex, Render::UniformBlock* sharedUniformBlock, uint32_t blockSize, uint32_t blockSizeAligned)
    : programIndex(programIndex),
      sharedUniformBlock(sharedUniformBlock),
      blockSize(blockSize),
      blockSizeAligned(blockSizeAligned)
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
    const uint32_t blockSizeAligned{0u};
  };

  using UniformBufferViewContainer = std::unordered_map<ProgramUniformBlockPair, Dali::Graphics::UniquePtr<Render::UniformBufferView>, ProgramUniformBlockPair::ProgramUniformBlockPairHash>;
  UniformBufferViewContainer mSharedUniformBlockBufferViews{};

  uint32_t mTotalAlignedBlockSize{0u};

  Render::UniformBlock::ProgramIndex mLatestQueryIndex{0u};
  Render::UniformBlock*              mLatestQueryBlock{nullptr};
  Render::UniformBufferView*         mLatestResult{nullptr};
};

SharedUniformBufferViewContainer::SharedUniformBufferViewContainer()
: mImpl(new Impl())
{
}

SharedUniformBufferViewContainer::~SharedUniformBufferViewContainer() = default;

bool SharedUniformBufferViewContainer::RegisterSharedUniformBlockAndPrograms(const Program& program, Render::UniformBlock& sharedUniformBlock, uint32_t blockSize, uint32_t blockSizeAligned)
{
  Render::UniformBlock::ProgramIndex programIndex = sharedUniformBlock.GetProgramIndex(program);

  auto ret = mImpl->mSharedUniformBlockBufferViews.insert({Impl::ProgramUniformBlockPair(programIndex, &sharedUniformBlock, blockSize, blockSizeAligned), nullptr});

  if(ret.second)
  {
    mImpl->mTotalAlignedBlockSize += blockSizeAligned;
  }

  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Registered : %zu, Program : %p, UBO : %p, blockSize : %u %u -- Registered : %d. Total GPU size : %u\n", mImpl->mSharedUniformBlockBufferViews.size(), &program, &sharedUniformBlock, blockSize, blockSizeAligned, ret.second, mImpl->mTotalAlignedBlockSize);

  return ret.second;
}

uint32_t SharedUniformBufferViewContainer::GetTotalAlignedBlockSize() const
{
  return mImpl->mTotalAlignedBlockSize;
}

void SharedUniformBufferViewContainer::Initialize(Render::UniformBufferManager& uniformBufferManager)
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

    // To skip hash table search for single-container cases.
    if(mImpl->mLatestQueryBlock == nullptr)
    {
      mImpl->mLatestQueryIndex = programIndex;
      mImpl->mLatestQueryBlock = &sharedUniformBlock;
      mImpl->mLatestResult     = item.second.get();
    }

    auto& ubo = *(item.second.get());

    // Write to the buffer view here, by value at sharedUniformBlock.
    sharedUniformBlock.WriteUniforms(programIndex, ubo);
  }
  DALI_LOG_INFO(gLogFilter, Debug::Verbose, "Registered : %zu, SharedUniformBufferView count : %u, total block size:%u %u\n", mImpl->mSharedUniformBlockBufferViews.size(), totalUniformBufferViewCount, totalSize, mImpl->mTotalAlignedBlockSize);
}

Render::UniformBufferView* SharedUniformBufferViewContainer::GetSharedUniformBlockBufferView(const Program& program, Render::UniformBlock& sharedUniformBlock) const
{
  Render::UniformBlock::ProgramIndex programIndex = sharedUniformBlock.GetProgramIndex(program);

  if(mImpl->mLatestQueryIndex == programIndex && mImpl->mLatestQueryBlock == &sharedUniformBlock)
  {
    return mImpl->mLatestResult;
  }

  mImpl->mLatestQueryIndex = programIndex;
  mImpl->mLatestQueryBlock = &sharedUniformBlock;

  auto key = Impl::ProgramUniformBlockPair(mImpl->mLatestQueryIndex, mImpl->mLatestQueryBlock, 0u, 0u);

  auto iter = mImpl->mSharedUniformBlockBufferViews.find(key);
  if(iter != mImpl->mSharedUniformBlockBufferViews.end())
  {
    return mImpl->mLatestResult = iter->second.get();
  }
  return mImpl->mLatestResult = nullptr;
}

void SharedUniformBufferViewContainer::Finalize()
{
  if(mImpl->mSharedUniformBlockBufferViews.size() > 1u)
  {
    mImpl->mSharedUniformBlockBufferViews.clear();
    mImpl->mSharedUniformBlockBufferViews.rehash(0);
  }
  else
  {
    mImpl->mSharedUniformBlockBufferViews.clear();
  }

  // Reset only mLatestQueryBlock is enough.
  mImpl->mLatestQueryBlock = nullptr;

  mImpl->mTotalAlignedBlockSize = 0u;
}

} // namespace Dali::Internal
