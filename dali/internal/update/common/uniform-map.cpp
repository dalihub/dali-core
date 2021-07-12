/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
void UniformMap::AddObserver(Observer& observer)
{
  bool foundObserver = false;
  for(ObserversIter iter = mObservers.Begin(); iter != mObservers.End(); ++iter)
  {
    if(*iter == &observer)
    {
      foundObserver = true;
      break;
    }
  }
  if(!foundObserver)
  {
    mObservers.PushBack(&observer);
  }
}

void UniformMap::RemoveObserver(Observer& observer)
{
  for(ObserversIter iter = mObservers.Begin(); iter != mObservers.End(); ++iter)
  {
    if(*iter == &observer)
    {
      mObservers.Erase(iter);
      return;
    }
  }
}

void UniformMap::MappingChanged()
{
  ++mChangeCounter;
  for(ObserversIter iter = mObservers.Begin(); iter != mObservers.End(); ++iter)
  {
    Observer* observer = (*iter);
    observer->UniformMappingsChanged(*this);
  }
}

void UniformMap::Add(UniformPropertyMapping newMap)
{
  auto iter = std::find_if(mUniformMaps.Begin(),
                           mUniformMaps.End(),
                           [&](auto& element) { return element.uniformName == newMap.uniformName; });

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
                           [&](auto& element) { return element.uniformName == uniformName; });

  if(iter != mUniformMaps.End())
  {
    mUniformMaps.Erase(iter);
    MappingChanged();
  }
}

const PropertyInputImpl* UniformMap::Find(ConstString uniformName)
{
  auto iter = std::find_if(mUniformMaps.Begin(),
                           mUniformMaps.End(),
                           [&](auto& element) { return element.uniformName == uniformName; });

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
