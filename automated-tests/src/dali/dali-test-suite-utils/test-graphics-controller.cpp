/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
#include "test-graphics-controller.h"


namespace Test
{

GraphicsController::GraphicsController()
{
}

GraphicsController::~GraphicsController() = default;


std::unique_ptr< Dali::Graphics::Shader > GraphicsController::CreateShader( const Dali::Graphics::BaseFactory< Dali::Graphics::Shader >& factory )
{
  return factory.Create();
}

std::unique_ptr< Dali::Graphics::Texture > GraphicsController::CreateTexture( const Dali::Graphics::BaseFactory< Dali::Graphics::Texture >& factory )
{
  return factory.Create();
}

std::unique_ptr< Dali::Graphics::Buffer > GraphicsController::CreateBuffer( const Dali::Graphics::BaseFactory< Dali::Graphics::Buffer >& factory )
{
  return factory.Create();
}

std::unique_ptr< Dali::Graphics::Sampler > GraphicsController::CreateSampler( const Dali::Graphics::BaseFactory< Dali::Graphics::Sampler >& factory )
{
  return factory.Create();
}

std::unique_ptr< Dali::Graphics::Pipeline > GraphicsController::CreatePipeline( const Dali::Graphics::PipelineFactory& factory )
{
  auto& pipelineFactory = const_cast<Dali::Graphics::PipelineFactory&>(dynamic_cast<const Dali::Graphics::PipelineFactory&>( factory ));
  return pipelineFactory.Create();
}

std::unique_ptr< Dali::Graphics::Framebuffer > GraphicsController::CreateFramebuffer( const Dali::Graphics::BaseFactory< Dali::Graphics::Framebuffer >& factory )
{
  return factory.Create();
}

Dali::Graphics::TextureFactory& GraphicsController::GetTextureFactory() const
{
  return *(const_cast<GraphicsTextureFactory*>(&mTextureFactory));
}

Dali::Graphics::ShaderFactory& GraphicsController::GetShaderFactory() const
{
  return *(const_cast<GraphicsShaderFactory*>(&mShaderFactory));
}

Dali::Graphics::BufferFactory& GraphicsController::GetBufferFactory() const
{
  return *(const_cast<GraphicsBufferFactory*>(&mBufferFactory));
}

Dali::Graphics::FramebufferFactory& GraphicsController::GetFramebufferFactory() const
{
  return *(const_cast<GraphicsFramebufferFactory*>(&mFramebufferFactory));
}

Dali::Graphics::PipelineFactory& GraphicsController::GetPipelineFactory()
{
  return *(const_cast<GraphicsPipelineFactory*>(&mPipelineFactory));
}

Dali::Graphics::SamplerFactory& GraphicsController::GetSamplerFactory()
{
  return *(const_cast<GraphicsSamplerFactory*>(&mSamplerFactory));
}

std::unique_ptr< Dali::Graphics::RenderCommand > GraphicsController::AllocateRenderCommand()
{
  return nullptr;
}

void GraphicsController::SubmitCommands( std::vector< Dali::Graphics::RenderCommand* > commands )
{
}

void GraphicsController::BeginFrame()
{
}

void GraphicsController::EndFrame()
{
}

void GraphicsController::Pause()
{
}

void GraphicsController::Resume()
{
}

bool GraphicsController::EnableDepthStencilBuffer( bool enableDepth, bool enableStencil )
{
  return true;
}

void GraphicsController::RunGarbageCollector( size_t numberOfDiscardedRenderers )
{
}

void GraphicsController::DiscardUnusedResources()
{
}

bool GraphicsController::IsDiscardQueueEmpty()
{
  return true;
}

bool GraphicsController::IsDrawOnResumeRequired()
{
  return false;
}

void GraphicsController::WaitIdle()
{
}

void GraphicsController::SwapBuffers()
{
}

uint32_t GraphicsController::GetSwapchainBufferCount()
{
  return 2;
}

}
