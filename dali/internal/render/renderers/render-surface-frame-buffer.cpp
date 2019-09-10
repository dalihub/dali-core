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
  mContext( nullptr ),
  mWidth( mSurface->GetPositionSize().width ),
  mHeight( mSurface->GetPositionSize().height ),
  mBackgroundColor( 0.f, 0.f, 0.f, 1.f ),
  mSizeChanged( false ),
  mIsSurfaceInvalid( false )
{
}

SurfaceFrameBuffer::~SurfaceFrameBuffer()
{}

void SurfaceFrameBuffer::Destroy( Context& context )
{
  if ( IsSurfaceValid() )
  {
    mSurface->DestroySurface();
    mSurface = nullptr;
  }
}

void SurfaceFrameBuffer::GlContextDestroyed()
{
  if ( mContext )
  {
    mContext->GlContextDestroyed();
  }

  if ( IsSurfaceValid() )
  {
    mSurface->DestroySurface();
    mSurface = nullptr;
  }
}

void SurfaceFrameBuffer::Initialize(Context& context)
{
  mContext = &context;
  mContext->GlContextCreated();

  if ( IsSurfaceValid() )
  {
    mSurface->InitializeGraphics();
  }
}

void SurfaceFrameBuffer::Bind( Context& context )
{
  if ( IsSurfaceValid() )
  {
    mSurface->PreRender( mSizeChanged );

    context.BindFramebuffer( GL_FRAMEBUFFER, 0u );
  }
}

uint32_t SurfaceFrameBuffer::GetWidth() const
{
  return mWidth;
}

uint32_t SurfaceFrameBuffer::GetHeight() const
{
  return mHeight;
}

void SurfaceFrameBuffer::PostRender()
{
  if ( IsSurfaceValid() )
  {
    mSurface->PostRender( false, false, mSizeChanged );
  }

  mSizeChanged = false;
}

Context* SurfaceFrameBuffer::GetContext()
{
  return mContext;
}

void SurfaceFrameBuffer::MakeContextCurrent()
{
  if ( IsSurfaceValid() )
  {
    mSurface->MakeContextCurrent();
  }
}

Vector4 SurfaceFrameBuffer::GetBackgroundColor()
{
  return mBackgroundColor;
}

void SurfaceFrameBuffer::SetSize( uint32_t width, uint32_t height )
{
  mWidth = width;
  mHeight = height;
  mSizeChanged = true;
}

void SurfaceFrameBuffer::SetBackgroundColor( const Vector4& color )
{
  mBackgroundColor = color;
}

bool SurfaceFrameBuffer::IsSurfaceValid() const
{
  return mSurface && !mIsSurfaceInvalid;
}

} //Render

} //Internal

} //Dali
