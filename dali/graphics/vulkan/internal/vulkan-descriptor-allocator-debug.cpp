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

#include <dali/graphics/vulkan/internal/vulkan-descriptor-allocator.h>
#include <dali/graphics/vulkan/internal/vulkan-debug.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

void PrintAllocationReport( DescriptorSetAllocator& allocator )
{
#if defined(DEBUG_ENABLED)
  std::lock_guard< std::mutex > lock{ allocator.mMutex };

  DALI_LOG_STREAM( gVulkanFilter, Debug::General, "DESCRIPTOR SET ALLOCATOR: ALLOCATION REPORT ----------------" )
  for( const auto& entry : allocator.mStorage )
  {
    DALI_LOG_STREAM( gVulkanFilter, Debug::General, "ThreadId: " << entry.threadId << std::endl );

    for( const auto& pair : entry.signatureAndPools )
    {
      DALI_LOG_STREAM( gVulkanFilter, Debug::General, "===> Signature: " << pair.signature );
      auto valuesAndTypes = pair.signature.Decode();

      for( const auto& tuple : valuesAndTypes )
      {
        const auto& type = std::get< DescriptorType >( tuple );
        const auto& value = std::get< uint64_t >( tuple );
        auto typeString = std::string{};

        switch( type )
        {
          case DescriptorType::STORAGE_IMAGE:
            typeString = "STORAGE_IMAGE";
            break;
          case DescriptorType::SAMPLER:
            typeString = "SAMPLER";
            break;
          case DescriptorType::SAMPLED_IMAGE:
            typeString = "SAMPLED_IMAGE";
            break;
          case DescriptorType::COMBINED_IMAGE_SAMPLER:
            typeString = "COMBINED_IMAGE_SAMPLER";
            break;
          case DescriptorType::UNIFORM_TEXEL_BUFFER:
            typeString = "UNIFORM_TEXEL_BUFFER";
            break;
          case DescriptorType::STORAGE_TEXEL_BUFFER:
            typeString = "STORAGE_TEXEL_BUFFER";
            break;
          case DescriptorType::UNIFORM_BUFFER:
            typeString = "UNIFORM_BUFFER";
            break;
          case DescriptorType::STORAGE_BUFFER:
            typeString = "STORAGE_BUFFER";
            break;
          case DescriptorType::DYNAMIC_UNIFORM_BUFFER:
            typeString = "DYNAMIC_UNIFORM_BUFFER";
            break;
          case DescriptorType::DYNAMIC_STORAGE_BUFFER:
            typeString = "DYNAMIC_STORAGE_BUFFER";
            break;
          case DescriptorType::INPUT_ATTACHMENT:
            typeString = "INPUT_ATTACHMENT";
            break;
        }

        DALI_LOG_INFO( gVulkanFilter, Debug::General, "\t\tValue: %ld\tType: %s\n", value, typeString.c_str() );
      }

      DALI_LOG_STREAM( gVulkanFilter, Debug::General, "" );
      DALI_LOG_INFO( gVulkanFilter, Debug::General, "\t\tMax Pool Size: %d\n", allocator.mMaxPoolSize );
      DALI_LOG_INFO( gVulkanFilter, Debug::General, "\t\tPool Count: %ld\n", pair.pools.size() );
      for( const auto& pool : pair.pools )
      {
        DALI_LOG_INFO( gVulkanFilter, Debug::General, "\t\t\tPool: %p => Available allocations: %d\n",
                       static_cast< void* >( pool->GetVkHandle() ),
                       pool->GetAvailableAllocations() );
      }

      DALI_LOG_STREAM( gVulkanFilter, Debug::General, "" );
    }
  }
  DALI_LOG_STREAM( gVulkanFilter, Debug::General, "----------------------------------------------------------------" )
#endif
}

} // namespace Vulkan
} // namespace Graphics
} // namespace Dali
