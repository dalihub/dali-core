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
#include "test-graphics-render-command.h"
#include <algorithm>

namespace Test
{

GraphicsController::GraphicsController()
: mTextureFactory(this),
  mShaderFactory(this),
  mFramebufferFactory(this),
  mBufferFactory(this),
  mPipelineFactory(this),
  mSamplerFactory(this)
{
}

GraphicsController::~GraphicsController() = default;


std::unique_ptr< Dali::Graphics::Shader > GraphicsController::CreateShader( const Dali::Graphics::BaseFactory< Dali::Graphics::Shader >& factory )
{
  mControllerTrace.PushCall("CreateShader");
  std::unique_ptr< Dali::Graphics::Shader > shader = std::move(factory.Create());
  //mShaders.push_back(shader.get());
  return shader;
}

std::unique_ptr< Dali::Graphics::Texture > GraphicsController::CreateTexture( const Dali::Graphics::BaseFactory< Dali::Graphics::Texture >& factory )
{
  mControllerTrace.PushCall("CreateTexture");
  std::unique_ptr< Dali::Graphics::Texture > texture = std::move(factory.Create());
  mTextures.push_back(texture.get()); // Idea is to allow test cases to use index in this array instead of GL ID's
  return texture;
}

std::unique_ptr< Dali::Graphics::Buffer > GraphicsController::CreateBuffer( const Dali::Graphics::BaseFactory< Dali::Graphics::Buffer >& factory )
{
  mControllerTrace.PushCall("CreateBuffer");
  std::unique_ptr< Dali::Graphics::Buffer > buffer = std::move(factory.Create());
  //mBuffers.push_back(buffer.get());
  return buffer;
}

std::unique_ptr< Dali::Graphics::Sampler > GraphicsController::CreateSampler( const Dali::Graphics::BaseFactory< Dali::Graphics::Sampler >& factory )
{
  mControllerTrace.PushCall("CreateSampler");
  std::unique_ptr< Dali::Graphics::Sampler > sampler = std::move(factory.Create());
  //mSamplers.push_back(sampler.get());
  return sampler;
}

std::unique_ptr< Dali::Graphics::Pipeline > GraphicsController::CreatePipeline( const Dali::Graphics::PipelineFactory& factory )
{
  mControllerTrace.PushCall("CreatePipeline");
  auto& pipelineFactory = const_cast<Dali::Graphics::PipelineFactory&>(dynamic_cast<const Dali::Graphics::PipelineFactory&>( factory ));

  std::unique_ptr< Dali::Graphics::Pipeline > pipeline = std::move(pipelineFactory.Create());
  return pipeline;
}

std::unique_ptr< Dali::Graphics::Framebuffer > GraphicsController::CreateFramebuffer( const Dali::Graphics::BaseFactory< Dali::Graphics::Framebuffer >& factory )
{
  mControllerTrace.PushCall("CreateFramebuffer");
  std::unique_ptr< Dali::Graphics::Framebuffer > framebuffer = std::move(factory.Create());
  //mFramebuffers.push_back(framebuffer.get());
  return framebuffer;
}

Dali::Graphics::TextureFactory& GraphicsController::GetTextureFactory() const
{
  const_cast<Test::TraceCallStack&>(mControllerTrace).PushCall("GetTextureFactory");
  return *(const_cast<GraphicsTextureFactory*>(&mTextureFactory));
}

Dali::Graphics::ShaderFactory& GraphicsController::GetShaderFactory() const
{
  const_cast<Test::TraceCallStack&>(mControllerTrace).PushCall("GetShaderFactory");
  return *(const_cast<GraphicsShaderFactory*>(&mShaderFactory));
}

Dali::Graphics::BufferFactory& GraphicsController::GetBufferFactory() const
{
  const_cast<Test::TraceCallStack&>(mControllerTrace).PushCall("GetBufferFactory");
  return *(const_cast<GraphicsBufferFactory*>(&mBufferFactory));
}

Dali::Graphics::FramebufferFactory& GraphicsController::GetFramebufferFactory() const
{
  const_cast<Test::TraceCallStack&>(mControllerTrace).PushCall("GetFramebufferFactory");
  return *(const_cast<GraphicsFramebufferFactory*>(&mFramebufferFactory));
}

Dali::Graphics::PipelineFactory& GraphicsController::GetPipelineFactory()
{
  mControllerTrace.PushCall("GetPipelineFactory");
  return *(const_cast<GraphicsPipelineFactory*>(&mPipelineFactory));
}

Dali::Graphics::SamplerFactory& GraphicsController::GetSamplerFactory()
{
  mControllerTrace.PushCall("GetSamplerFactory");
  return *(const_cast<GraphicsSamplerFactory*>(&mSamplerFactory));
}

std::unique_ptr< Dali::Graphics::RenderCommand > GraphicsController::AllocateRenderCommand()
{
  mControllerTrace.PushCall("AllocateRenderCommand");
  return std::make_unique< GraphicsRenderCommand >();
}

void GraphicsController::SubmitCommands( std::vector< Dali::Graphics::RenderCommand* > commands )
{
  TraceCallStack::NamedParams namedParams;
  int i=0;
  for( auto cmd : commands )
  {
    std::ostringstream o;
    o<<"commands["<<i++<<"].mDrawCommand.drawType";
    namedParams[o.str()] = Dali::Any(int(cmd->mDrawCommand.drawType));
  }
  mControllerTrace.PushCall("SubmitCommands", namedParams);

  // Copy the render commands to prevent any lifecycle problems interfering with the test.
  mRenderCommands.clear();
  mRenderCommands.reserve (commands.size());
  for( auto cmd : commands )
  {
    auto testCmd = static_cast<Test::GraphicsRenderCommand*>(cmd);
    mRenderCommands.emplace_back(*testCmd);
  }
}

void GraphicsController::BeginFrame()
{
  mControllerTrace.PushCall("BeginFrame");
}

void GraphicsController::EndFrame()
{
  mControllerTrace.PushCall("EndFrame");
}

void GraphicsController::Pause()
{
  mControllerTrace.PushCall("Pause");
}

void GraphicsController::Resume()
{
  mControllerTrace.PushCall("Resume");
}

bool GraphicsController::EnableDepthStencilBuffer( bool enableDepth, bool enableStencil )
{
  TraceCallStack::NamedParams namedParams;
  namedParams["enableDepth"] = Dali::Any(enableDepth);
  namedParams["enableStencil"] = Dali::Any(enableStencil);
  mControllerTrace.PushCall("EnableDepthStencilBuffer", namedParams);
  return true;
}

void GraphicsController::RunGarbageCollector( size_t numberOfDiscardedRenderers )
{
  TraceCallStack::NamedParams namedParams;
  namedParams["numberOfDiscardedRenderers"] = Dali::Any(numberOfDiscardedRenderers);
  mControllerTrace.PushCall("EnableDepthStencilBuffer", namedParams);
}

void GraphicsController::DiscardUnusedResources()
{
  mControllerTrace.PushCall("DiscardUnusedResources");
}

bool GraphicsController::IsDiscardQueueEmpty()
{
  mControllerTrace.PushCall("IsDiscardQueueEmpty");
  return true;
}

bool GraphicsController::IsDrawOnResumeRequired()
{
  mControllerTrace.PushCall("IsDrawOnResumeRequired");
  return false;
}

void GraphicsController::WaitIdle()
{
  mControllerTrace.PushCall("WaitIdle");
}

void GraphicsController::SwapBuffers()
{
  mControllerTrace.PushCall("SwapBuffers");
}

void GraphicsController::UpdateTextures( const std::vector<Dali::Graphics::TextureUpdateInfo>& updateInfoList, const std::vector<Dali::Graphics::TextureUpdateSourceInfo>& sourceList )
{
}

uint32_t GraphicsController::GetSwapchainBufferCount()
{
  mControllerTrace.PushCall("GetSwapchainBufferCount");
  return 2;
}

void GraphicsController::DestroyShader( Dali::Graphics::Shader* shader )
{
}

void GraphicsController::DestroyTexture( Dali::Graphics::Texture* texture )
{
  auto iter = std::find(mTextures.begin(), mTextures.end(), texture);
  if( iter != mTextures.end() )
  {
    mTextures.erase(iter);
  }
}

void GraphicsController::DestroyFramebuffer( Dali::Graphics::Framebuffer* framebuffer )
{
}
void GraphicsController::DestroyBuffer( Dali::Graphics::Buffer* buffer )
{
}
void GraphicsController::DestroySampler( Dali::Graphics::Sampler* sampler )
{
}
void GraphicsController::DestroyPipeline( Dali::Graphics::Pipeline* pipeline )
{
}

}
