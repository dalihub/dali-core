/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// FILE HEADER
#include "memory-pool-relayout-container.h"

namespace Dali
{
namespace Internal
{
MemoryPoolRelayoutContainer::MemoryPoolRelayoutContainer(MemoryPoolObjectAllocator<RelayoutInfo>& objectAllocator)
: mAllocator(objectAllocator)
{
  mDummyRelayoutInfo.reset(new RelayoutInfo());
}

MemoryPoolRelayoutContainer::~MemoryPoolRelayoutContainer() = default;

void MemoryPoolRelayoutContainer::Add(const Dali::Actor& actor, const Vector2& size)
{
  // Store actor into dummy info.
  // It will be used for check comparision.
  // TODO : Can't we remove this handle copying, to avoid useless IntrusivePtr ref count?
  mDummyRelayoutInfo->actor = actor;

  if(mRelayoutInfos.Find(mDummyRelayoutInfo.get()) == mRelayoutInfos.End())
  {
    void*         ptr  = mAllocator.AllocateRaw();
    RelayoutInfo* info = new(ptr) RelayoutInfo();

    info->actor = std::move(mDummyRelayoutInfo->actor);
    info->size  = size;

    mRelayoutInfos.PushBack(info);
  }
  else
  {
    // Reset empty handle for deference.
    mDummyRelayoutInfo->actor = Dali::Actor();
  }
}

void MemoryPoolRelayoutContainer::PopBack(Dali::Actor& actorOut, Vector2& sizeOut)
{
  DALI_ASSERT_ALWAYS(mRelayoutInfos.Count() > 0 && "Try to pop from empty relayout container!");

  RelayoutInfoContainer::ConstIterator back = mRelayoutInfos.End();
  back--;
  RelayoutInfo* info = *back;

  mRelayoutInfos.Erase(back);

  actorOut = std::move(info->actor);
  sizeOut  = std::move(info->size);

  // Need to be destroyed after mRelayoutInfos erased.
  mAllocator.Destroy(info);
}

size_t MemoryPoolRelayoutContainer::Size() const
{
  return mRelayoutInfos.Count();
}

void MemoryPoolRelayoutContainer::Reserve(size_t capacity)
{
  mRelayoutInfos.Reserve(capacity);
}

void MemoryPoolRelayoutContainer::Clear()
{
  for(auto& info : mRelayoutInfos)
  {
    mAllocator.Destroy(info);
  }

  mRelayoutInfos.Clear();
}

} // namespace Internal

} // namespace Dali
