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

// EXTERNAL INCLUDES
#include <algorithm>

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
  auto iter = std::find_if(mUniformMaps.Begin(),
                           mUniformMaps.End(),
                           [&](auto& element)
  { return element.uniformName == newMap.uniformName; });

  if(iter != mUniformMaps.End())
  {
    // Mapping already exists - update it.
    (*iter).propertyPtr = newMap.propertyPtr;
  }
  else
  {
    // add the new map.
    mUniformMaps.PushBack(newMap);
  }

  MappingChanged();
}

void UniformMap::Remove(ConstString uniformName)
{
  auto iter = std::find_if(mUniformMaps.Begin(),
                           mUniformMaps.End(),
                           [&](auto& element)
  { return element.uniformName == uniformName; });

  if(iter != mUniformMaps.End())
  {
    mUniformMaps.Erase(iter);
    MappingChanged();
  }
}

const PropertyInputImpl* UniformMap::Find(ConstString uniformName) const
{
  auto iter = std::find_if(mUniformMaps.Begin(),
                           mUniformMaps.End(),
                           [&](auto& element)
  { return element.uniformName == uniformName; });

  return (iter != mUniformMaps.End()) ? (*iter).propertyPtr : nullptr;
}

UniformMap::SizeType UniformMap::Count() const
{
  return static_cast<UniformMap::SizeType>(mUniformMaps.Count());
}

const UniformPropertyMapping& UniformMap::operator[](UniformMap::SizeType index) const
{
  return mUniformMaps[index];
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
