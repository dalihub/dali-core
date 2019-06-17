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

// CLASS HEADER
#include <dali/internal/render/renderers/render-surface-frame-buffer.h>

// INTERNAL INCLUDES
#include <dali/internal/render/renderers/render-texture.h>

namespace Dali
{
namespace Internal
{
namespace Render
{

SurfaceFrameBuffer::SurfaceFrameBuffer( Integration::RenderSurface* surface )
: FrameBuffer(),
  mSurface( surface ),
  mContext( nullptr )
{
}

SurfaceFrameBuffer::~SurfaceFrameBuffer()
{}

void SurfaceFrameBuffer::Destroy( Context& context )
{
}

void SurfaceFrameBuffer::GlContextDestroyed()
{
  if ( mContext )
  {
    mContext->GlContextDestroyed();
  }
}

void SurfaceFrameBuffer::Initialize(Context& context)
{
  mContext = &context;
  mContext->GlContextCreated();
  mSurface->InitializeGraphics();
}

void SurfaceFrameBuffer::Bind( Context& context )
{
  mSurface->PreRender( false );
  context.BindFramebuffer( GL_FRAMEBUFFER, 0u );
}

uint32_t SurfaceFrameBuffer::GetWidth() const
{
  return mSurface->GetPositionSize().width;
}

uint32_t SurfaceFrameBuffer::GetHeight() const
{
  return mSurface->GetPositionSize().height;
}

void SurfaceFrameBuffer::PostRender()
{
  mSurface->PostRender( false, false, false );
}

Context* SurfaceFrameBuffer::GetContext()
{
  return mContext;
}

Integration::DepthBufferAvailable SurfaceFrameBuffer::GetDepthBufferRequired()
{
  return mSurface->GetDepthBufferRequired();
}

Integration::StencilBufferAvailable SurfaceFrameBuffer::GetStencilBufferRequired()
{
  return mSurface->GetStencilBufferRequired();
}

Vector4 SurfaceFrameBuffer::GetBackgroundColor()
{
  return mSurface->GetBackgroundColor();
}

} //Render

} //Internal

} //Dali
