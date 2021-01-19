/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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

#include "test-graphics-controller.h"

#include "test-graphics-buffer.h"
#include "test-graphics-command-buffer.h"
#include "test-graphics-sampler.h"
#include "test-graphics-texture.h"

#include <dali/integration-api/gl-defines.h>
#include <iostream>
#include <sstream>

namespace Dali
{
std::ostream& operator<<(std::ostream& o, const Graphics::BufferCreateInfo& bufferCreateInfo)
{
  return o << "usage:" << std::hex << bufferCreateInfo.usage << ", size:" << std::dec << bufferCreateInfo.size;
}

std::ostream& operator<<(std::ostream& o, const Graphics::CommandBufferCreateInfo& commandBufferCreateInfo)
{
  return o << "level:" << (commandBufferCreateInfo.level == Graphics::CommandBufferLevel::PRIMARY ? "PRIMARY" : "SECONDARY")
           << ", fixedCapacity:" << std::dec << commandBufferCreateInfo.fixedCapacity;
}

std::ostream& operator<<(std::ostream& o, const Graphics::TextureType& textureType)
{
  switch(textureType)
  {
    case Graphics::TextureType::TEXTURE_2D:
      o << "TEXTURE_2D";
      break;
    case Graphics::TextureType::TEXTURE_3D:
      o << "TEXTURE_3D";
      break;
    case Graphics::TextureType::TEXTURE_CUBEMAP:
      o << "TEXTURE_CUBEMAP";
      break;
  }
  return o;
}

std::ostream& operator<<(std::ostream& o, const Graphics::Extent2D extent)
{
  o << "width:" << extent.width << ", height:" << extent.height;
  return o;
}

std::ostream& operator<<(std::ostream& o, const Graphics::TextureCreateInfo& createInfo)
{
  o << "textureType:" << createInfo.textureType
    << " size:" << createInfo.size
    << " format:" << static_cast<uint32_t>(createInfo.format)
    << " mipMapFlag:" << createInfo.mipMapFlag
    << " layout:" << (createInfo.layout == Graphics::TextureLayout::LINEAR ? "LINEAR" : "OPTIMAL")
    << " usageFlags:" << std::hex << createInfo.usageFlags
    << " data:" << std::hex << createInfo.data
    << " dataSize:" << std::dec << createInfo.dataSize
    << " nativeImagePtr:" << std::hex << createInfo.nativeImagePtr;
  return o;
}

std::ostream& operator<<(std::ostream& o, Graphics::SamplerAddressMode addressMode)
{
  switch(addressMode)
  {
    case Graphics::SamplerAddressMode::REPEAT:
      o << "REPEAT";
      break;
    case Graphics::SamplerAddressMode::MIRRORED_REPEAT:
      o << "MIRRORED_REPEAT";
      break;
    case Graphics::SamplerAddressMode::CLAMP_TO_EDGE:
      o << "CLAMP_TO_EDGE";
      break;
    case Graphics::SamplerAddressMode::CLAMP_TO_BORDER:
      o << "CLAMP_TO_BORDER";
      break;
    case Graphics::SamplerAddressMode::MIRROR_CLAMP_TO_EDGE:
      o << "MIRROR_CLAMP_TO_EDGE";
      break;
  }
  return o;
}

std::ostream& operator<<(std::ostream& o, Graphics::SamplerFilter filterMode)
{
  switch(filterMode)
  {
    case Graphics::SamplerFilter::LINEAR:
      o << "LINEAR";
      break;
    case Graphics::SamplerFilter::NEAREST:
      o << "NEAREST";
      break;
  }
  return o;
}

std::ostream& operator<<(std::ostream& o, Graphics::SamplerMipmapMode mipmapMode)
{
  switch(mipmapMode)
  {
    case Graphics::SamplerMipmapMode::NONE:
      o << "NONE";
      break;
    case Graphics::SamplerMipmapMode::LINEAR:
      o << "LINEAR";
      break;
    case Graphics::SamplerMipmapMode::NEAREST:
      o << "NEAREST";
      break;
  }
  return o;
}

std::ostream& operator<<(std::ostream& o, const Graphics::SamplerCreateInfo& createInfo)
{
  o << "minFilter:" << createInfo.minFilter
    << " magFilter:" << createInfo.magFilter
    << " wrapModeU:" << createInfo.addressModeU
    << " wrapModeV:" << createInfo.addressModeV
    << " wrapModeW:" << createInfo.addressModeW
    << " mipMapMode:" << createInfo.mipMapMode;
  return o;
}

TestGraphicsController::TestGraphicsController()
{
  mCallStack.Enable(true);
  mCallStack.EnableLogging(true);
  mCommandBufferCallStack.Enable(true);
  mCommandBufferCallStack.EnableLogging(true);
  auto& trace = mGlAbstraction.GetTextureTrace();
  trace.Enable(true);
  trace.EnableLogging(true);
}

void TestGraphicsController::SubmitCommandBuffers(const Graphics::SubmitInfo& submitInfo)
{
  std::ostringstream          out;
  TraceCallStack::NamedParams namedParams;
  out << "cmdBuffer[" << submitInfo.cmdBuffer.size() << "], flags:" << std::hex << submitInfo.flags;
  namedParams["submitInfo"] = out.str();

  mCallStack.PushCall("SubmitCommandBuffers", "", namedParams);

  for(auto& commandBuffer : submitInfo.cmdBuffer)
  {
    for(auto& binding : (static_cast<TestGraphicsCommandBuffer*>(commandBuffer))->mTextureBindings)
    {
      if(binding.texture)
      {
        auto texture = const_cast<TestGraphicsTexture*>(static_cast<const TestGraphicsTexture*>(binding.texture));

        texture->Bind(binding.binding);

        if(binding.sampler)
        {
          auto sampler = const_cast<TestGraphicsSampler*>(static_cast<const TestGraphicsSampler*>(binding.sampler));
          if(sampler)
          {
            sampler->Apply(texture->GetTarget());
          }
        }

        texture->Prepare(); // Ensure native texture is ready
      }
    }
  }
}

/**
 * @brief Presents render target
 * @param renderTarget render target to present
 */
void TestGraphicsController::PresentRenderTarget(Graphics::RenderTarget* renderTarget)
{
  std::ostringstream          out;
  TraceCallStack::NamedParams namedParams;
  out << std::hex << renderTarget;
  namedParams["renderTarget"] = out.str();
  mCallStack.PushCall("PresentRenderTarget", "", namedParams);
}

/**
 * @brief Waits until the GPU is idle
 */
void TestGraphicsController::WaitIdle()
{
  mCallStack.PushCall("WaitIdle", "");
}

/**
 * @brief Lifecycle pause event
 */
void TestGraphicsController::Pause()
{
  mCallStack.PushCall("Pause", "");
}

/**
 * @brief Lifecycle resume event
 */
void TestGraphicsController::Resume()
{
  mCallStack.PushCall("Resume", "");
}

void TestGraphicsController::UpdateTextures(const std::vector<Graphics::TextureUpdateInfo>&       updateInfoList,
                                            const std::vector<Graphics::TextureUpdateSourceInfo>& sourceList)
{
  std::ostringstream          out;
  TraceCallStack::NamedParams namedParams;
  out << "[" << updateInfoList.size() << "]:";
  namedParams["updateInfoList"] = out.str();
  out.str("");
  out << "[" << sourceList.size() << "]:";
  namedParams["sourceList"] = out.str();

  mCallStack.PushCall("UpdateTextures", "", namedParams);

  // Call either TexImage2D or TexSubImage2D
  for(unsigned int i = 0; i < updateInfoList.size(); ++i)
  {
    auto& updateInfo = updateInfoList[i];
    auto& source     = sourceList[i];

    auto texture = static_cast<TestGraphicsTexture*>(updateInfo.dstTexture);
    texture->Bind(0); // Use first texture unit during resource update
    texture->Update(updateInfo, source);
  }
}

bool TestGraphicsController::EnableDepthStencilBuffer(bool enableDepth, bool enableStencil)
{
  TraceCallStack::NamedParams namedParams;
  namedParams["enableDepth"]   = enableDepth ? "T" : "F";
  namedParams["enableStencil"] = enableStencil ? "T" : "F";
  mCallStack.PushCall("EnableDepthStencilBuffer", "", namedParams);
  return false;
}

void TestGraphicsController::RunGarbageCollector(size_t numberOfDiscardedRenderers)
{
  std::ostringstream out;
  out << numberOfDiscardedRenderers;
  TraceCallStack::NamedParams namedParams;
  namedParams["numberOfDiscardedrenderers"] = out.str();
  mCallStack.PushCall("RunGarbageCollector", "", namedParams);
}

void TestGraphicsController::DiscardUnusedResources()
{
  mCallStack.PushCall("DiscardUnusedResources", "");
}

bool TestGraphicsController::IsDiscardQueueEmpty()
{
  mCallStack.PushCall("IsDiscardQueueEmpty", "");
  return isDiscardQueueEmptyResult;
}

/**
 * @brief Test if the graphics subsystem has resumed & should force a draw
 *
 * @return true if the graphics subsystem requires a re-draw
 */
bool TestGraphicsController::IsDrawOnResumeRequired()
{
  mCallStack.PushCall("IsDrawOnResumeRequired", "");
  return isDrawOnResumeRequiredResult;
}

Graphics::UniquePtr<Graphics::Buffer> TestGraphicsController::CreateBuffer(const Graphics::BufferCreateInfo& bufferCreateInfo, Graphics::UniquePtr<Graphics::Buffer>&& oldBuffer)
{
  std::ostringstream oss;
  oss << "bufferCreateInfo:" << bufferCreateInfo;
  mCallStack.PushCall("CreateBuffer", oss.str());

  return Graphics::MakeUnique<TestGraphicsBuffer>();
}

Graphics::UniquePtr<Graphics::CommandBuffer> TestGraphicsController::CreateCommandBuffer(const Graphics::CommandBufferCreateInfo& commandBufferCreateInfo, Graphics::UniquePtr<Graphics::CommandBuffer>&& oldCommandBuffer)
{
  std::ostringstream oss;
  oss << "commandBufferCreateInfo:" << commandBufferCreateInfo;
  mCallStack.PushCall("CreateCommandBuffer", oss.str());
  return Graphics::MakeUnique<TestGraphicsCommandBuffer>(mCommandBufferCallStack, mGlAbstraction);
}

Graphics::UniquePtr<Graphics::RenderPass> TestGraphicsController::CreateRenderPass(const Graphics::RenderPassCreateInfo& renderPassCreateInfo, Graphics::UniquePtr<Graphics::RenderPass>&& oldRenderPass)
{
  mCallStack.PushCall("CreateRenderPass", "");
  return nullptr;
}

Graphics::UniquePtr<Graphics::Texture> TestGraphicsController::CreateTexture(const Graphics::TextureCreateInfo& textureCreateInfo, Graphics::UniquePtr<Graphics::Texture>&& oldTexture)
{
  std::ostringstream params, oss;
  params << "textureCreateInfo:" << textureCreateInfo;
  TraceCallStack::NamedParams namedParams;
  oss << textureCreateInfo;
  namedParams["textureCreateInfo"] = oss.str();
  mCallStack.PushCall("CreateTexture", params.str(), namedParams);

  return Graphics::MakeUnique<TestGraphicsTexture>(mGlAbstraction, textureCreateInfo);
}

Graphics::UniquePtr<Graphics::Framebuffer> TestGraphicsController::CreateFramebuffer(const Graphics::FramebufferCreateInfo& framebufferCreateInfo, Graphics::UniquePtr<Graphics::Framebuffer>&& oldFramebuffer)
{
  mCallStack.PushCall("CreateFramebuffer", "");
  return nullptr;
}

Graphics::UniquePtr<Graphics::Pipeline> TestGraphicsController::CreatePipeline(const Graphics::PipelineCreateInfo& pipelineCreateInfo, Graphics::UniquePtr<Graphics::Pipeline>&& oldPipeline)
{
  mCallStack.PushCall("CreatePipeline", "");
  return nullptr;
}

Graphics::UniquePtr<Graphics::Shader> TestGraphicsController::CreateShader(const Graphics::ShaderCreateInfo& shaderCreateInfo, Graphics::UniquePtr<Graphics::Shader>&& oldShader)
{
  mCallStack.PushCall("CreateShader", "");
  return nullptr;
}

Graphics::UniquePtr<Graphics::Sampler> TestGraphicsController::CreateSampler(const Graphics::SamplerCreateInfo& samplerCreateInfo, Graphics::UniquePtr<Graphics::Sampler>&& oldSampler)
{
  std::ostringstream params, oss;
  params << "samplerCreateInfo:" << samplerCreateInfo;
  TraceCallStack::NamedParams namedParams;
  oss << samplerCreateInfo;
  namedParams["samplerCreateInfo"] = oss.str();
  mCallStack.PushCall("CreateSampler", params.str(), namedParams);

  return Graphics::MakeUnique<TestGraphicsSampler>(mGlAbstraction, samplerCreateInfo);
}

Graphics::UniquePtr<Graphics::RenderTarget> TestGraphicsController::CreateRenderTarget(const Graphics::RenderTargetCreateInfo& renderTargetCreateInfo, Graphics::UniquePtr<Graphics::RenderTarget>&& oldRenderTarget)
{
  mCallStack.PushCall("CreateRenderTarget", "");
  return nullptr;
}

Graphics::UniquePtr<Graphics::Memory> TestGraphicsController::MapBufferRange(const Graphics::MapBufferInfo& mapInfo)
{
  mCallStack.PushCall("MapBufferRange", "");
  return nullptr;
}

Graphics::UniquePtr<Graphics::Memory> TestGraphicsController::MapTextureRange(const Graphics::MapTextureInfo& mapInfo)
{
  mCallStack.PushCall("MapTextureRange", "");
  return nullptr;
}

void TestGraphicsController::UnmapMemory(Graphics::UniquePtr<Graphics::Memory> memory)
{
  mCallStack.PushCall("UnmapMemory", "");
}

Graphics::MemoryRequirements TestGraphicsController::GetTextureMemoryRequirements(Graphics::Texture& texture) const
{
  mCallStack.PushCall("GetTextureMemoryRequirements", "");
  return Graphics::MemoryRequirements{};
}

Graphics::MemoryRequirements TestGraphicsController::GetBufferMemoryRequirements(Graphics::Buffer& buffer) const
{
  mCallStack.PushCall("GetBufferMemoryRequirements", "");
  return Graphics::MemoryRequirements{};
}

const Graphics::TextureProperties& TestGraphicsController::GetTextureProperties(const Graphics::Texture& texture)
{
  static Graphics::TextureProperties textureProperties{};
  mCallStack.PushCall("GetTextureProperties", "");

  return textureProperties;
}

bool TestGraphicsController::PipelineEquals(const Graphics::Pipeline& pipeline0, const Graphics::Pipeline& pipeline1) const
{
  mCallStack.PushCall("PipelineEquals", "");
  return false;
}

} // namespace Dali
