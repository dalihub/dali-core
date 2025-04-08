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
#include <dali/internal/render/shaders/program-controller.h>

// INTERNAL INCLUDES
#include <dali/internal/common/shader-saver.h>
#include <dali/internal/render/shaders/program.h>

namespace Dali
{
namespace Internal
{
namespace
{
constexpr uint32_t MAXIMUM_COLLECTING_ITEM_COUNTS_PER_GC_CALL = 5u;
static_assert(1u <= MAXIMUM_COLLECTING_ITEM_COUNTS_PER_GC_CALL); /// Should delete at least 1 item.
} // namespace

ProgramController::ProgramController(Graphics::Controller& graphicsController)
: mGraphicsController(graphicsController),
  mProgramCacheAdded(false)
{
  mProgramCache.Reserve(32);

  mClearCacheIterator = mProgramCache.Begin();
}

ProgramController::~ProgramController() = default;

void ProgramController::ResetUsedFlag()
{
  for(auto&& item : mProgramCache)
  {
    item->ClearUsedFlag();
  }
  mClearCacheIterator = mProgramCache.Begin();
}

bool ProgramController::ClearUnusedCacheIncrementally(bool fullCollect, bool forceClearAll)
{
  if(mProgramCacheAdded)
  {
    // Clear from begin again if cache container changed.
    mClearCacheIterator = mProgramCache.Begin();

    // Reset flag
    mProgramCacheAdded = false;
  }

  uint32_t checkedCount = 0u;
  // Check only limited items. (Since this loop give overhead for rendering.)
  // TODO : Could we check running time here, instead of check counter?
  for(; mClearCacheIterator != mProgramCache.End() && (fullCollect || ++checkedCount <= MAXIMUM_COLLECTING_ITEM_COUNTS_PER_GC_CALL);)
  {
    if(forceClearAll || !((*mClearCacheIterator)->IsUsed()))
    {
      mClearCacheIterator = mProgramCache.Erase(mClearCacheIterator);
    }
    else
    {
      ++mClearCacheIterator;
    }
  }

  return mClearCacheIterator != mProgramCache.End();
}

Program* ProgramController::GetProgram(size_t shaderHash)
{
  Program*              program = nullptr;
  const ProgramIterator end     = mProgramCache.End();
  for(ProgramIterator iter = mProgramCache.Begin(); iter != end; ++iter)
  {
    size_t hash = (*iter)->GetHash();
    if(shaderHash == hash)
    {
      (*iter)->MarkAsUsed();
      program = (*iter)->GetProgram();
      break;
    }
  }
  return program;
}

void ProgramController::AddProgram(size_t shaderHash, Program* program)
{
  // we expect unique hash values so it is caller's job to guarantee that
  // AddProgram is only called after program checks that GetProgram returns NULL
  mProgramCache.PushBack(new ProgramPair(program, shaderHash));

  mProgramCacheAdded = true;
}

} // namespace Internal

} // namespace Dali
