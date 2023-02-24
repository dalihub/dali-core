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

bool MemoryPoolRelayoutContainer::Contains(const Dali::Actor& actor)
{
  // Store actor into dummy info.
  // It will be used for check comparision.
  mDummyRelayoutInfo->actor = actor;

  bool ret = (mRelayoutInfos.Find(mDummyRelayoutInfo.get()) != mRelayoutInfos.End());

  // Reset empty handle for deference.
  mDummyRelayoutInfo->actor = Dali::Actor();
  return ret;
}

void MemoryPoolRelayoutContainer::Add(const Dali::Actor& actor, const Vector2& size)
{
  if(!Contains(actor))
  {
    void*         ptr  = mAllocator.AllocateRaw();
    RelayoutInfo* info = new(ptr) RelayoutInfo();
    info->actor        = actor;
    info->size         = size;

    mRelayoutInfos.PushBack(info);
  }
}

void MemoryPoolRelayoutContainer::PopBack()
{
  if(mRelayoutInfos.Count() > 0)
  {
    RelayoutInfoContainer::Iterator back = mRelayoutInfos.End();
    back--;
    RelayoutInfo* info = *back;
    mRelayoutInfos.Erase(back);

    // Need to be destroyed after mRelayoutInfos erased.
    mAllocator.Destroy(info);
  }
}

void MemoryPoolRelayoutContainer::GetBack(Dali::Actor& actorOut, Vector2& sizeOut) const
{
  if(mRelayoutInfos.Count() > 0)
  {
    RelayoutInfoContainer::ConstIterator back = mRelayoutInfos.End();
    back--;
    RelayoutInfo* info = *back;
    actorOut           = info->actor;
    sizeOut            = info->size;
  }
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
