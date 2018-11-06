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
 */

#include <dali/graphics/vulkan/internal/vulkan-debug-allocator.h>
#include <mutex>
#include <map>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

#define DALI_VULKAN_DEBUG_ALLOCATOR
#ifdef DALI_VULKAN_DEBUG_ALLOCATOR

struct MemoryAllocationTag
{
  MemoryAllocationTag( const MemoryAllocationTag& rhs )
  {
    if( strlen(rhs.tagBuffer) )
    {
      strcpy( tagBuffer, rhs.tagBuffer );
    }
    parentPtr = rhs.parentPtr;
  }

  MemoryAllocationTag()
  {
    tagBuffer[0] = 0;
  }

  explicit MemoryAllocationTag( const char* tagString )
  {
    strcpy( tagBuffer, tagString );
  }

  char tagBuffer[256];
  void* parentPtr { nullptr };
};

namespace
{
struct AllocationTracker
{
  enum class ObjectType
  {
    IMAGE,
    IMAGE_VIEW,
    SAMPLER,
    FENCE,
    BUFFER,
    DEVICE_MEMORY,
    FRAMEBUFFER,
    RENDERPASS,
    UNDEFINED
  };

  AllocationTracker()
  {
    totalObjects = 0u;
    totalBytesAllocated = 0u;
    allocationId = 0u;
  }
  struct Allocation
  {
    void* ptr;
    size_t size;
    size_t alignment;
    size_t allocationId;
    bool reallocated;
    uint32_t freed;
    ObjectType objectType;
    VkSystemAllocationScope allocationScope;
    std::unique_ptr<MemoryAllocationTag> allocationTag;

    // used by stats
    uint32_t totalSizeWithSuballocations;
    uint32_t totalSuballocationCount;
  };

  size_t totalObjects;
  size_t totalBytesAllocated;
  size_t allocationId;
  std::recursive_mutex allocMutex{};

  void* valloc( size_t size, size_t align, VkSystemAllocationScope scope, MemoryAllocationTag* debugTag )
  {
    void* retval = malloc( size );
    auto allocation = Allocation();
    allocation.ptr = retval;
    allocation.size = size;

    allocation.alignment = align;
    allocation.allocationScope = scope;
    allocation.reallocated = false;
    if( debugTag )
    {
      allocation.allocationTag.reset( new MemoryAllocationTag( *debugTag ) );
      if( !debugTag->parentPtr )
      {
        // set parent allocation pointer
        debugTag->parentPtr = allocation.ptr;
      }
    }

    std::lock_guard<std::recursive_mutex> lock( allocMutex );
    totalObjects++;
    totalBytesAllocated += size;
    allocation.allocationId = ++allocationId;
    allocationData[retval] = std::move( allocation );
    return retval;
  }

  void* vrealloc( void* original, size_t size, size_t align, VkSystemAllocationScope scope, MemoryAllocationTag* debugTag  )
  {
    void* retval = ::realloc( original, size );
    auto allocation = Allocation();

    allocation.ptr = retval;
    allocation.size = size;
    allocation.alignment = align;
    allocation.allocationScope = scope;
    allocation.reallocated = true;
    if( debugTag )
    {
      allocation.allocationTag.reset( new MemoryAllocationTag( *debugTag ) );
      if( !debugTag->parentPtr )
      {
        // set parent allocation pointer
        debugTag->parentPtr = allocation.ptr;
      }
    }

    std::lock_guard<std::recursive_mutex> lock( allocMutex );
    allocation.allocationId = ++allocationId;
    if( original )
    {
      // find original
      auto it = allocationData.find( original );
      if( it != allocationData.end() )
      {
        totalBytesAllocated -= (*it).second.size;
        totalObjects--;
        if( original != retval )
        {
          allocationData.erase( it );
        }
      }
    }
    totalBytesAllocated += size;
    totalObjects++;
    allocationData[retval] = std::move( allocation );
    return retval;
  }

  void vfree( void* ptr )
  {
    if( !ptr )
    {
      //printf("[MEMORY] Warning! Freeing NULL pointer!\n");
      return;
    }
    std::lock_guard<std::recursive_mutex> lock( allocMutex );
    auto it = allocationData.find( ptr );
    if( it != allocationData.end() )
    {
      if( it->second.freed )
      {
        printf("[MEMORY] Object id = %d, ptr = %p has been freed %d times!\n",
               int(it->second.allocationId), it->second.ptr, int(it->second.freed+1));

      }
      else
      {
        free( (*it).first );
        totalObjects--;
        totalBytesAllocated -= (*it).second.size;
        allocationData.erase( it );
      }
      ++it->second.freed;
    }
    else
    {
      // trying to kill unallocated stuff!!!
      puts("Object not in the register!");
    }
  }

  void updateSubAllocations( Allocation& allocation )
  {
    allocation.totalSizeWithSuballocations = uint32_t( allocation.size );
    allocation.totalSuballocationCount = 1u;

    // find suballocations
    for( auto& k : allocationData )
    {
      if( k.second.allocationTag && k.second.allocationTag->parentPtr == allocation.ptr )
      {
        allocation.totalSizeWithSuballocations += uint32_t(k.second.size);
        allocation.totalSuballocationCount++;
      }
    }
  }


  void printStatus()
  {
    std::lock_guard<std::recursive_mutex> lock( allocMutex );
    puts("Memory stats:");
    printf("Total allocations: %d\n", int( totalObjects ));
    printf("Total allocations: %d\n", int( totalBytesAllocated ));

    size_t objectCount { 0u };
    size_t objectMemory { 0u };
    size_t commandCount { 0u };
    size_t commandMemory { 0u };

    size_t imageCount { 0u };
    size_t imageViewCount { 0u };
    size_t bufferCount { 0u };
    size_t samplerCount { 0u };
    size_t framebufferCount { 0u };
    size_t renderPassCount { 0u };
    size_t deviceMemoryCount { 0u };
    size_t shaderModuleCount { 0u };
    size_t pipelineCount { 0u };
    size_t pipelineLayoutCount { 0u };
    size_t dsLayoutCount { 0u };
    size_t dsPoolCount { 0u };
    size_t dsPoolMemory { 0u };

    std::vector<Allocation*> descriptorPoolAllocations;
    for( auto& item : allocationData )
    {
      if( !item.second.freed )
      {
        if( item.second.allocationScope == VK_SYSTEM_ALLOCATION_SCOPE_OBJECT
          && item.second.allocationTag
          && !item.second.allocationTag->parentPtr )
        {
          objectCount++;
          objectMemory += item.second.size;

          std::string str( item.second.allocationTag->tagBuffer );
          if(str == "IMAGEVIEW")
            imageViewCount++;
          else if(str == "IMAGE")
            imageCount++;
          else if(str == "SAMPLER")
            samplerCount++;
          else if(str == "SAMPLER")
            bufferCount++;
          else if(str == "DEVICEMEMORY")
            deviceMemoryCount++;
          else if(str == "SHADERMODULE")
            shaderModuleCount++;
          else if(str == "DESCRIPTORPOOL")
          {
            dsPoolCount++;
            dsPoolMemory += item.second.size;
            descriptorPoolAllocations.emplace_back(&item.second);
          }
          else if(str == "PIPELINE")
            pipelineCount++;
          else if(str == "PIPELINELAYOUT")
            pipelineLayoutCount++;
          else if(str == "DESCRIPTORSETLAYOUT")
            dsLayoutCount++;

          updateSubAllocations( item.second );
        }
        else if( item.second.allocationScope == VK_SYSTEM_ALLOCATION_SCOPE_COMMAND )
        {
          commandCount++;
          commandMemory += item.second.size;
        }
      }
    }

    printf("  Objects : count = %d, size = %d\n", int(objectCount), int(objectMemory) );
    printf("      IMAGE        = %d\n", int(imageCount) );
    printf("      IMAGEVIEW    = %d\n", int(imageViewCount) );
    printf("      BUFFER       = %d\n", int(bufferCount) );
    printf("      SAMPLER      = %d\n", int(samplerCount) );
    printf("      FRAMEBUFFER  = %d\n", int(framebufferCount) );
    printf("      RENDERPASS   = %d\n", int(renderPassCount) );
    printf("      DEVICEMEM    = %d\n", int(deviceMemoryCount) );
    printf("      SHADERMODULE = %d\n", int(shaderModuleCount) );
    printf("      PIPELINE     = %d\n", int(pipelineCount ));
    printf("      PIPELINE_LAYOUT = %d\n", int(pipelineLayoutCount));
    printf("      DESCRIPTOR_SET_LAYOUT = %d\n", int(dsLayoutCount ));
    printf("      DESCRIPTOR_POOL = %d, size = %d\n", int(dsPoolCount), int(dsPoolMemory));

    for(const auto& dspool : descriptorPoolAllocations )
    {
      printf("         Pool: %p, size = %d, allSize = %d, sub = %d\n",
             dspool->ptr, int(dspool->size),
             int(dspool->totalSizeWithSuballocations),
             int(dspool->totalSuballocationCount) );
    }

    //printf("      COMMAND_POOL = %lu\n", 0);
    printf("  Commands: count = %d, size = %d\n", int(commandCount), int(commandMemory) );

    puts("");
  }

  std::map<void*, Allocation> allocationData;

} gAllocationTracker;

void* vkAllocationFunction(
  void*                                       pUserData,
  size_t                                      size,
  size_t                                      alignment,
  VkSystemAllocationScope                     allocationScope)
{
  auto retval = gAllocationTracker.valloc( size, alignment, allocationScope,
                                           pUserData ? reinterpret_cast<MemoryAllocationTag*>(pUserData) : nullptr );

  return retval;
}

void* vkReallocationFunction(
  void*                                       pUserData,
  void*                                       pOriginal,
  size_t                                      size,
  size_t                                      alignment,
  VkSystemAllocationScope                     allocationScope)
{
  auto retval = gAllocationTracker.vrealloc( pOriginal, size, alignment, allocationScope,
                                             pUserData ? reinterpret_cast<MemoryAllocationTag*>(pUserData) : nullptr );
  return retval;
}

void vkFreeFunction(
  void*                                       pUserData,
  void*                                       pMemory)
{
  gAllocationTracker.vfree( pMemory );
}

vk::AllocationCallbacks gDebugAllocationCallbacks( nullptr, vkAllocationFunction, vkReallocationFunction, vkFreeFunction );
}



#endif

const vk::AllocationCallbacks* GetDebugAllocator()
{
#ifdef DALI_VULKAN_DEBUG_ALLOCATOR
  return &gDebugAllocationCallbacks;
#else
  return nullptr;
#endif
}

void PrintAllocationStatus()
{
  gAllocationTracker.printStatus();
}

MemoryAllocationTag* CreateMemoryAllocationTag( const char* tagString )
{
  if(!tagString)
  {
    return nullptr;
  }
  return new MemoryAllocationTag( tagString );
}

}
}
}
