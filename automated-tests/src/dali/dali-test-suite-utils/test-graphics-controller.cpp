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


std::unique_ptr< Dali::Graphics::API::Shader > GraphicsController::CreateShader( const Dali::Graphics::API::BaseFactory< Dali::Graphics::API::Shader >& factory )
{
  return factory.Create();
}

std::unique_ptr< Dali::Graphics::API::Texture > GraphicsController::CreateTexture( const Dali::Graphics::API::BaseFactory< Dali::Graphics::API::Texture >& factory )
{
  return factory.Create();
}

std::unique_ptr< Dali::Graphics::API::Buffer > GraphicsController::CreateBuffer( const Dali::Graphics::API::BaseFactory< Dali::Graphics::API::Buffer >& factory )
{
  return factory.Create();
}

std::unique_ptr< Dali::Graphics::API::Sampler > GraphicsController::CreateSampler( const Dali::Graphics::API::BaseFactory< Dali::Graphics::API::Sampler >& factory )
{
  return factory.Create();
}

std::unique_ptr< Dali::Graphics::API::Pipeline > GraphicsController::CreatePipeline( const Dali::Graphics::API::PipelineFactory& factory )
{
  auto& pipelineFactory = const_cast<Dali::Graphics::API::PipelineFactory&>(dynamic_cast<const Dali::Graphics::API::PipelineFactory&>( factory ));
  return pipelineFactory.Create();
}

std::unique_ptr< Dali::Graphics::API::Framebuffer > GraphicsController::CreateFramebuffer( const Dali::Graphics::API::BaseFactory< Dali::Graphics::API::Framebuffer >& factory )
{
  return factory.Create();
}

Dali::Graphics::API::TextureFactory& GraphicsController::GetTextureFactory() const
{
  return *(const_cast<GraphicsTextureFactory*>(&mTextureFactory));
}

Dali::Graphics::API::ShaderFactory& GraphicsController::GetShaderFactory() const
{
  return *(const_cast<GraphicsShaderFactory*>(&mShaderFactory));
}

Dali::Graphics::API::BufferFactory& GraphicsController::GetBufferFactory() const
{
  return *(const_cast<GraphicsBufferFactory*>(&mBufferFactory));
}

Dali::Graphics::API::FramebufferFactory& GraphicsController::GetFramebufferFactory() const
{
  return *(const_cast<GraphicsFramebufferFactory*>(&mFramebufferFactory));
}

Dali::Graphics::API::PipelineFactory& GraphicsController::GetPipelineFactory()
{
  return *(const_cast<GraphicsPipelineFactory*>(&mPipelineFactory));
}

Dali::Graphics::API::SamplerFactory& GraphicsController::GetSamplerFactory()
{
  return *(const_cast<GraphicsSamplerFactory*>(&mSamplerFactory));
}

std::unique_ptr< Dali::Graphics::API::RenderCommand > GraphicsController::AllocateRenderCommand()
{
  return nullptr;
}

void GraphicsController::SubmitCommands( std::vector< Dali::Graphics::API::RenderCommand* > commands )
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
