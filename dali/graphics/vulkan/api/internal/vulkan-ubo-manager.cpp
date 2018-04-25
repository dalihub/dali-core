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

#include <dali/graphics/vulkan/api/internal/vulkan-ubo-manager.h>
#include <dali/graphics/vulkan/api/internal/vulkan-ubo-pool.h>

#include <unordered_map>

namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{


struct UboManager::Impl
{
  explicit Impl( UboManager& uboManager, Controller& controller )
  : mUboManager( uboManager ),
    mController( controller )
  {
    Initialise();
  }

  ~Impl() = default;

  void Initialise()
  {
    // populate pools
    mPools.emplace( 64, std::make_unique<UboPool>( mController, 64, 64 ) );
    mPools.emplace( 128, std::make_unique<UboPool>( mController, 128, 64 ) );
    mPools.emplace( 256, std::make_unique<UboPool>( mController, 256, 64 ) );
    mPools.emplace( 512, std::make_unique<UboPool>( mController, 512, 64 ) );
    mPools.emplace( 1024, std::make_unique<UboPool>( mController, 1024, 64 ) );
  }

  /**
   * Allocates memory within pool space choosing proper allocation size
   * @param uboSize
   * @return
   */
  std::unique_ptr<Ubo> Allocate( uint32_t uboSize )
  {
    UboPool* uboPool { nullptr };
    for( auto&& pool : mPools )
    {
      if( uboSize < pool.first )
      {
        uboPool = &*pool.second;
        break;
      }
    }
    if(!uboPool)
    {
      return nullptr;
    }

    return uboPool->Allocate();
  }


  /**
   * Maps all UBO buffers for writing
   * @note this should happen when frame begins
   */
  void MapAllBuffers()
  {

  }

  /**
   * Unmaps all buffers and flushes memory
   * @note this should happen when frame ends
   */
  void UnmapAllBuffers()
  {

  }


  UboManager& mUboManager;
  Controller& mController;

  std::unordered_map<uint32_t, std::unique_ptr<UboPool>> mPools;
};

//

UboManager::UboManager(Controller& controller)
{
  mImpl = std::make_unique<Impl>( *this, controller );
}

UboManager::~UboManager() = default;

std::unique_ptr<Ubo> UboManager::Allocate( uint32_t size )
{
  return mImpl->Allocate( size );
}


}
}
}

