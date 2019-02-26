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
 */

#include "test-graphics-render-command.h"

namespace Test
{

GraphicsRenderCommand::GraphicsRenderCommand()
{
}

GraphicsRenderCommand::~GraphicsRenderCommand()
{
}

GraphicsRenderCommand::GraphicsRenderCommand( const GraphicsRenderCommand& rhs)
{
  mVertexBufferBindings = rhs.mVertexBufferBindings;
  mUniformBufferBindings = rhs.mUniformBufferBindings;
  mTextureBindings = rhs.mTextureBindings;
  mSamplerBindings = rhs.mSamplerBindings;
  mIndexBufferBinding = rhs.mIndexBufferBinding;
  mRenderTargetBinding = rhs.mRenderTargetBinding;
  mDrawCommand = rhs.mDrawCommand;
  mPushConstantsBindings = rhs.mPushConstantsBindings;
  mPipeline = rhs.mPipeline;
}

GraphicsRenderCommand& GraphicsRenderCommand::operator=( const GraphicsRenderCommand& rhs)
{
  mVertexBufferBindings = rhs.mVertexBufferBindings;
  mUniformBufferBindings = rhs.mUniformBufferBindings;
  mTextureBindings = rhs.mTextureBindings;
  mSamplerBindings = rhs.mSamplerBindings;
  mIndexBufferBinding = rhs.mIndexBufferBinding;
  mRenderTargetBinding = rhs.mRenderTargetBinding;
  mDrawCommand = rhs.mDrawCommand;
  mPushConstantsBindings = rhs.mPushConstantsBindings;
  mPipeline = rhs.mPipeline;
  return *this;
}


} // Test namespace
