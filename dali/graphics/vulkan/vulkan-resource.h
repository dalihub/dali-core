#ifndef DALI_VULKAN_RESOURCE_H
#define DALI_VULKAN_RESOURCE_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/graphics/vulkan/vulkan-types.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
class ResourceManager;
/*
template <class ResourceType>
class ResourceHandle
{
public:

  ResourceType* operator->()
  {
    return mResourceManager.GetResource<ResourceType>( mResourceId );
  }

private:
  std::vector<uint32_t>   mResourcesConnected; // all resources using this resource
  std::atomic_uint        mResourceCount;
  ResourceManager&        mResourceManager;
  uint32_t                mResourceId;
};


// creates placeholder resource
Shader = resourceManager->CreateResource<Shader>(); // allocates empty resource space to be used by placement new upon initialisation


//
*/
//resourceManager->CreateImage()

} // Namespace Vulkan

} // Namespace Graphics

} // Namespace Dali

#endif //DALI_VULKAN_RESOURCE_H
