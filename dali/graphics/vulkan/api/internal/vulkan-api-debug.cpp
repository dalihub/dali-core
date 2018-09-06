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

#if defined(DEBUG_ENABLED)

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-api-render-command.h>
#include <dali/graphics/vulkan/api/vulkan-api-buffer.h>
#include <dali/graphics/vulkan/api/vulkan-api-texture.h>
#include <dali/graphics/vulkan/internal/vulkan-buffer.h>
#include <dali/graphics/vulkan/internal/vulkan-image.h>
#include <dali/graphics/vulkan/internal/vulkan-image-view.h>
#include <dali/graphics/vulkan/internal/vulkan-sampler.h>
#include <dali/graphics/vulkan/internal/vulkan-gpu-memory-handle.h>
#include <dali/graphics/vulkan/internal/vulkan-gpu-memory-manager.h>
#include <dali/graphics/vulkan/internal/vulkan-gpu-memory-allocator.h>

// EXTERNAL INCLUDES
#include <sstream>

namespace Dali
{
namespace Graphics
{
namespace API
{

std::string APItoVK( const Buffer* buffer)
{
  std::stringstream ss;
  if(!buffer)
  {
    ss << "{ nullptr }" << std::endl;
    return ss.str();
  }
  const auto& vbuffer = static_cast<const VulkanAPI::Buffer&>(*buffer);
  auto vbufferref = vbuffer.GetBufferRef();
  auto vkBuffer = vbufferref->GetVkHandle();

  ss << "VulkanAPI::Buffer(" << &vbuffer << ") {" << std::endl;
  ss << "         vk::Buffer = " << static_cast<VkBuffer>(vkBuffer) << std::endl;
  ss << "      }";
  return ss.str();
}

std::string APItoVK( const Texture* texture)
{
  std::stringstream ss;
  if(!texture)
  {
    ss << "{ nullptr }" << std::endl;
    return ss.str();
  }
  const auto& apiTexture = static_cast<const VulkanAPI::Texture&>(*texture);

  auto imageRef = apiTexture.GetImageRef();
  auto imageViewRef = apiTexture.GetImageViewRef();
  auto samplerRef = apiTexture.GetSamplerRef();

  ss << "VulkanAPI::Texture(" << &apiTexture << ") {" << std::endl;
  ss << "         vk::Image = " << static_cast<VkImage>(imageRef->GetVkHandle()) << std::endl;
  ss << "         vk::ImageView = " << static_cast<VkImageView>( imageViewRef->GetVkHandle() ) << std::endl;
  ss << "         vk::Sampler = " << static_cast<VkSampler>(samplerRef->GetVkHandle()) << std::endl;
  //@ todo: add gpu memory block info too
  ss << "      }";
  return ss.str();
}

std::ostream& operator<<(std::ostream& ss, const RenderCommand::UniformBufferBinding& object)
{
  ss << "    UniformBufferBinding = " << &object << " { " << std::endl;
  ss << "      buffer = " << APItoVK(object.buffer) << std::endl;
  ss << "      dataSize = " << object.dataSize << std::endl;
  ss << "      offset = " << object.offset << std::endl;
  ss << "      binding = " << object.binding << std::endl;
  ss << "    }" << std::endl;
  return ss;
}

std::ostream& operator<<(std::ostream& ss, const RenderCommand::TextureBinding& object)
{
  ss << "    TextureBinding = " << &object << " { " << std::endl;
  ss << "      texture = " << APItoVK(object.texture) << std::endl;
  ss << "      sampler = " << object.sampler << std::endl;
  ss << "      binding = " << object.binding << std::endl;
  ss << "    }" << std::endl;
  return ss;
}

std::ostream& operator<<(std::ostream& ss, const RenderCommand::SamplerBinding& object)
{
  ss << "    SamplerBinding = " << &object << " { " << std::endl;
  ss << "      sampler = " << object.sampler << std::endl;
  ss << "      binding = " << object.binding << std::endl;
  ss << "    }" << std::endl;
  return ss;
}

std::ostream& operator<<(std::ostream& ss, const RenderCommand::IndexBufferBinding& object)
{
  ss << "    IndexBufferBinding = { " << std::endl;
  ss << "      buffer = " << APItoVK( object.buffer ) << std::endl;
  ss << "      offset = " << object.offset << std::endl;
  ss << "      type = " << std::function<std::string()>([&object]() -> std::string {
    switch(object.type)
    {
      case RenderCommand::IndexType::INDEX_TYPE_UINT16:
        return std::string("INDEX_TYPE_UINT16");
      case RenderCommand::IndexType::INDEX_TYPE_UINT32:
        return std::string("INDEX_TYPE_UINT32");
    }
    return "";
  })() << std::endl;
  ss << "    }" << std::endl;
  return ss;
}

std::ostream& operator<<(std::ostream& ss, const RenderCommand::RenderTargetBinding& object)
{
  ss << "    RenderTargetBinding = " << &object << " { " << std::endl;
  ss << "      framebuffer = " << object.framebuffer << std::endl;
  ss << "      clearColors = " << std::function<std::string()>([&object]() -> std::string {
    if( object.clearColors.empty() )
    {
      return " = nullptr\n";
    }
    std::stringstream s;
    for( auto i = 0u; i < object.clearColors.size(); ++i )
    {
      s << "       [" << i << "] = \n" <<
        "               clearColors.r = " << object.clearColors[i].r << "\n" <<
        "               clearColors.g = " << object.clearColors[i].g << "\n" <<
        "               clearColors.b = " << object.clearColors[i].b << "\n" <<
        "               clearColors.a = " << object.clearColors[i].a << "\n";
    }
    return s.str();
  })() << std::endl;
  ss << "      dsClearColor = [ d = " << object.depthStencilClearColor.depth <<
                             ", s = " << object.depthStencilClearColor.stencil << "]\n";
  ss << "    }" << std::endl;
  return ss;
}

std::ostream& operator<<(std::ostream& ss, const RenderCommand::DrawCommand& object)
{
  ss << "    DrawCommand = " << &object << " { " << std::endl;
  ss << "      drawType = " << std::function<std::string()>([&object]() -> std::string {
    switch(object.drawType)
    {
      case RenderCommand::DrawType::INDEXED_DRAW:
        return std::string("INDEXED_DRAW");
      case RenderCommand::DrawType::VERTEX_DRAW:
        return std::string("VERTEX_DRAW");
      case RenderCommand::DrawType::UNDEFINED_DRAW:
        return std::string("UNDEFINED_DRAW");
    }
    return "";
  })() << std::endl;
  ss << "      union {\n";
  ss << "         firstVertex = " << object.firstVertex << std::endl;
  ss << "         firstIndex = " << object.firstIndex << std::endl;;
  ss << "      }\n";
  ss << "      union {\n";
  ss << "         vertexCount = " << object.vertexCount << std::endl;
  ss << "         indicesCount = " << object.indicesCount << std::endl;;
  ss << "      }\n";
  ss << "      firstInstance = " << object.firstInstance << std::endl;
  ss << "      instanceCount = " << object.instanceCount << std::endl;
  ss << "    }" << std::endl;
  return ss;
}

std::ostream& operator<<(std::ostream& ss, const RenderCommand::PushConstantsBinding& object)
{
  ss << "    PushConstantsBinding = " << &object << " { " << std::endl;
  ss << "      data = " << object.data << std::endl;
  ss << "      size = " << object.size << std::endl;
  ss << "      binding = " << object.binding << std::endl;
  ss << "    }" << std::endl;
  return ss;
}

std::ostream& operator<<(std::ostream& ss, const RenderCommand& object)
{
  ss << "RenderCommand {\n";
  ss << "  mDebugString = " << object.mDebugString << std::endl;
  ss << "  mDebugObject = " << object.mDebugObject << std::endl;
  ss << "  mVertexBufferBindings = " << std::endl;
  for( auto i = 0u; i < object.mVertexBufferBindings.size(); ++i )
  {
    ss << "      " << APItoVK(object.mVertexBufferBindings[i]) << std::endl;
  }
  ss << "  }\n" << std::endl;

  if(object.mUniformBufferBindings)
  {
    ss << "  mUniformBufferBindings = {" << std::endl;
    for( auto i = 0u; i < object.mUniformBufferBindings->size(); ++i )
    {
      ss << (*object.mUniformBufferBindings)[i] << std::endl;
    }
    ss << "  }\n" << std::endl;
  }

  ss << "  mTextureBindings {" << std::endl;
  for( auto i = 0u; i < object.mTextureBindings.size(); ++i )
  {
    ss << object.mTextureBindings[i] << std::endl;
  }
  ss << "  }\n" << std::endl;

  ss << "  mSamplerBindings {" << std::endl;
  for( auto i = 0u; i < object.mSamplerBindings.size(); ++i )
  {
    ss << object.mSamplerBindings[i] << std::endl;
  }
  ss << "  }\n" << std::endl;

  ss << "  mPushConstantsBindings {" << std::endl;
  for( auto i = 0u; i < object.mPushConstantsBindings.size(); ++i )
  {
    ss << object.mPushConstantsBindings[i] << std::endl;
  }
  ss << "  }\n" << std::endl;

  ss << "  mIndexBufferBinding {" << std::endl;
  ss << object.mIndexBufferBinding << std::endl;
  ss << "  }\n" << std::endl;

  ss << "  mDrawCommand {" << std::endl;
  ss << object.mDrawCommand << std::endl;
  ss << "  }" << std::endl;
  ss << "}" << std::endl;

  return ss;
}

}
}
}

#endif
