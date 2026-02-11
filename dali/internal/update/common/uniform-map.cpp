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
 */

#include <algorithm>

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
  ++(mImpl->mChangeCounter);
}

void UniformMap::Add(UniformPropertyMapping newMap)
{
  if(!mImpl)
  {
    mImpl = std::make_unique<UniformMap::Impl>();
  }
  auto& uniformMaps = mImpl->mUniformMaps;

  auto iter = uniformMaps.lower_bound(newMap.uniformName);
  if(iter == uniformMaps.end() || ConstStringComparator()(newMap.uniformName, iter->first))
  {
    uniformMaps.insert(iter, {newMap.uniformName, newMap});
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
  if(!mImpl)
  {
    return;
  }
  auto& uniformMaps = mImpl->mUniformMaps;

  auto iter = uniformMaps.find(uniformName);
  if(iter != uniformMaps.end())
  {
    uniformMaps.erase(iter);
    MappingChanged();
  }
}

const PropertyInputImpl* UniformMap::Find(ConstString uniformName) const
{
  if(!mImpl)
  {
    return nullptr;
  }
  auto& uniformMaps = mImpl->mUniformMaps;

  auto iter = uniformMaps.find(uniformName);
  return (iter != uniformMaps.end()) ? iter->second.propertyPtr : nullptr;
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
