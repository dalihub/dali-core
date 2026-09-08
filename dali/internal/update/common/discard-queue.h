#ifndef DALI_INTERNAL_DISCARD_QUEUE_H
#define DALI_INTERNAL_DISCARD_QUEUE_H

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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-namespace.h>

namespace DALI_NAMESPACE
{
namespace Internal
{
namespace SceneGraph
{
template<class Type, class TypeContainer>
class DiscardQueue
{
public:
  DiscardQueue() = default;

  void Add(Type object)
  {
    mDiscardQueue.PushBack(object);
  }

  void Clear()
  {
    mDiscardQueue.Clear();
#if defined(LOW_SPEC_MEMORY_MANAGEMENT_ENABLED)
    mDiscardQueue.ShrinkToFit();
#endif
  }

  bool IsEmpty() const
  {
    return mDiscardQueue.IsEmpty();
  }

private:
  TypeContainer mDiscardQueue;
};

} // namespace SceneGraph

} // namespace Internal

} //namespace DALI_NAMESPACE

#endif // DALI_INTERNAL_DISCARD_QUEUE_H
