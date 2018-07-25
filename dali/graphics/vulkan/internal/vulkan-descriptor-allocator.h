#ifndef DALI_GRAPHICS_VULKAN_DESCRIPTOR_ALLOCATOR
#define DALI_GRAPHICS_VULKAN_DESCRIPTOR_ALLOCATOR

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

#include <dali/graphics/vulkan/internal/vulkan-types.h>
#include <dali/graphics/vulkan/internal/vulkan-descriptor-set.h>
#include <utility>
#include <vector>
#include <thread>
#include <mutex>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{

struct SignatureAndPools
{
  DescriptorSetLayoutSignature signature;
  std::vector< RefCountedDescriptorPool > pools;

  SignatureAndPools() = default;

  SignatureAndPools( DescriptorSetLayoutSignature signature_,
                     std::vector< RefCountedDescriptorPool > pools_)
  : signature( signature_ ),
    pools( std::move( pools_ ) )
  {}
};

struct SignatureAndPoolsPerThread
{
  std::thread::id threadId;
  std::vector< SignatureAndPools > signatureAndPools;

  SignatureAndPoolsPerThread() = default;

  SignatureAndPoolsPerThread( std::thread::id threadId_,
                              std::vector< SignatureAndPools > signatureAndPools_ )
  : threadId( threadId_ ),
    signatureAndPools( std::move( signatureAndPools_ ) )
  {}
};

class DescriptorSetAllocator final
{
public:

  explicit DescriptorSetAllocator( Graphics& graphics, uint32_t maxPoolSize );

  ~DescriptorSetAllocator() = default;

  std::vector< RefCountedDescriptorSet >
  AllocateDescriptorSets( const std::vector< DescriptorSetLayoutSignature >& signatures,
                          const std::vector< vk::DescriptorSetLayout >& descriptorSetLayouts );

  void PrintAllocationReport();

private:

private:
  std::vector< vk::DescriptorPoolSize >
  GetPoolSizesFromDescriptorSetLayoutSignature( const DescriptorSetLayoutSignature& signature );

  void ExtendPoolVectorForSignature( const DescriptorSetLayoutSignature& signature,
                                     std::vector< RefCountedDescriptorPool >& poolVector,
                                     uint32_t count );

private:
  Graphics* mGraphics;

  uint32_t mMaxPoolSize;

  std::mutex mMutex;

  std::vector< SignatureAndPoolsPerThread > mStorage;
};

} //namespace Vulkan

} //namespace Graphics

} //namespace Dali

#endif //DALI_GRAPHICS_VULKAN_DESCRIPTOR_ALLOCATOR
