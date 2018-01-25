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

#include <dali/graphics/vulkan/vulkan-descriptor-set.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>

namespace Dali
{
namespace Graphics
{
namespace Vulkan
{
struct DescriptorSetLayout::Impl
{
  Impl( Graphics& graphics, const vk::DescriptorSetLayoutCreateInfo& createInfo ) :
    mGraphics( graphics ),
    mCreateInfo( createInfo )
  {
  }

  ~Impl()
  {
    if(mLayout)
    {
      mGraphics.GetDevice().destroyDescriptorSetLayout( mLayout, mGraphics.GetAllocator() );
    }
  }

  bool Initialise()
  {
    mLayout = VkAssert( mGraphics.GetDevice().createDescriptorSetLayout( mCreateInfo, mGraphics.GetAllocator() ));
    if(mLayout)
    {
      return true;
    }
    return false;
  }

  Graphics&                         mGraphics;
  vk::DescriptorSetLayout           mLayout;
  vk::DescriptorSetLayoutCreateInfo mCreateInfo;
};

/**
 * Class: DescriptorSetLayout
 */

std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::New( Graphics& graphics, const vk::DescriptorSetLayoutCreateInfo& createInfo )
{
  auto retval = std::unique_ptr<DescriptorSetLayout>( new DescriptorSetLayout(graphics, createInfo ) );
  if( retval->mImpl->Initialise() )
  {
    return retval;
  }
  return nullptr;
}

DescriptorSetLayout::DescriptorSetLayout( Graphics& graphics, const vk::DescriptorSetLayoutCreateInfo& createInfo )
{
  mImpl = MakeUnique<DescriptorSetLayout::Impl>( graphics, createInfo );
}

} // Namespace Vulkan

} // Namespace Graphics

} // Namespace Dali
