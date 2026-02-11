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
 */

// CLASS HEADER
#include <dali/internal/update/common/uniform-map.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
void UniformMap::MappingChanged()
{
  ++mChangeCounter;
}

void UniformMap::Add(UniformPropertyMapping newMap)
{
  auto iter = mUniformMaps.lower_bound(newMap.uniformName);
  if(iter == mUniformMaps.end() || ConstStringComparator()(newMap.uniformName, iter->first))
  {
    mUniformMaps.insert(iter, {newMap.uniformName, newMap});
  }
  else
  {
    // element already present, update the property pointer in place
    iter->second.propertyPtr = newMap.propertyPtr;
  }
  MappingChanged();
}

void UniformMap::Remove(ConstString uniformName)
{
  auto iter = mUniformMaps.find(uniformName);
  if(iter != mUniformMaps.end())
  {
    mUniformMaps.erase(iter);
    MappingChanged();
  }
}

const PropertyInputImpl* UniformMap::Find(ConstString uniformName) const
{
  auto iter = mUniformMaps.find(uniformName);
  return (iter != mUniformMaps.end()) ? iter->second.propertyPtr : nullptr;
}

UniformMap::SizeType UniformMap::Count() const
{
  return static_cast<UniformMap::SizeType>(mUniformMaps.size());
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
