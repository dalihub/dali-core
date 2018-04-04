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

#include <dali/graphics/vulkan/api/vulkan-api-buffer-factory.h>
#include <dali/graphics/vulkan/api/vulkan-api-buffer.h>
#include <dali/graphics/vulkan/vulkan-graphics.h>
#include <dali/graphics/vulkan/vulkan-buffer.h>
#include <dali/graphics/vulkan/vulkan-graphics-controller.h>
namespace Dali
{
namespace Graphics
{
namespace VulkanAPI
{
BufferFactory::BufferFactory( Vulkan::Controller& controller )
: mController( controller ), mGraphics(controller.GetGraphics() )
{

}

BufferFactory& BufferFactory::SetUsage( API::Buffer::UsageHint usage )
{
  mUsageHints = usage;
  switch( usage )
  {
    case API::Buffer::UsageHint::ATTRIBUTES:
    {
      mUsage = vk::BufferUsageFlagBits::eVertexBuffer;
      break;
    }
    case API::Buffer::UsageHint::FRAME_CONSTANTS:
    {
      mUsage = vk::BufferUsageFlagBits::eUniformBuffer;
      break;
    }
    case API::Buffer::UsageHint::INDEX_BUFFER:
    {
      mUsage = vk::BufferUsageFlagBits::eIndexBuffer;
      break;
    }
    case API::Buffer::UsageHint::INSTANCE:
    {
      mUsage = vk::BufferUsageFlagBits::eVertexBuffer;
      break;
    }
    case API::Buffer::UsageHint::PRIMITIVE_UNIFORMS:
    {
      mUsage = vk::BufferUsageFlagBits::eUniformBuffer;
      break;
    }
  }
  return *this;
}

BufferFactory& BufferFactory::SetSize( uint32_t size )
{
  mSize = size;
  return *this;
}

std::unique_ptr<API::Buffer> BufferFactory::Create() const
{
  auto retval = std::make_unique<VulkanAPI::Buffer>( mController, mUsage, mUsageHints, mSize );
  if( retval->Initialise() )
  {
    return std::unique_ptr<API::Buffer>{ retval.release() };
  }
  return nullptr;
}



} // namespace VulkanAPI
} // namespace Graphics
} // namespace Dali