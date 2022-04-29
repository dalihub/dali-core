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

// CLASS HEADER
#include <dali/internal/update/common/collected-uniform-map.h>

// INTERNAL HEADERS
#include <dali/devel-api/common/hash.h>
#include <dali/internal/render/data-providers/uniform-map-data-provider.h>

// EXTERNAL HEADERS
#include <algorithm>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
void CollectedUniformMap::AddMappings(const UniformMap& uniformMap)
{
  // Iterate thru uniformMap.
  // Any maps that aren't in localMap should be added in a single step

  // keep a static vector to avoid temporary heap allocation.
  // As this function gets called only from update thread we don't have to
  // make it thread safe (so no need to keep a thread_local variable).
  static Dali::Vector<UniformPropertyMapping> newUniformMappings;

  newUniformMappings.Clear();

  for(UniformMap::SizeType i = 0, iCount = uniformMap.Count(); i < iCount; ++i)
  {
    bool found = false;

    for(UniformMap::SizeType j = 0, jCount = mUniformMap.Count(); j < jCount; ++j)
    {
      if(mUniformMap[j].uniformName == uniformMap[i].uniformName)
      {
        found = true;
        break;
      }
    }
    if(!found)
    {
      newUniformMappings.PushBack(uniformMap[i]);
    }
  }

  if(newUniformMappings.Count() > 0)
  {
    mUniformMap.Reserve(mUniformMap.Count() + newUniformMappings.Count());

    for(UniformMap::SizeType i = 0, iCount = newUniformMappings.Count(); i < iCount; ++i)
    {
      mUniformMap.PushBack(newUniformMappings[i]);
    }
  }
}

} // namespace SceneGraph
} // namespace Internal
} // namespace Dali
