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
 */

// CLASS HEADER
#include <dali/internal/update/rendering/render-frame-buffer.h>

// INTERNAL INCLUDES
#include <dali/internal/update/rendering/render-texture.h>

namespace Dali
{
namespace Internal
{
namespace Render
{

FrameBuffer::FrameBuffer( unsigned int width, unsigned int height, unsigned int attachments )
:  mWidth( width ),
   mHeight( height )
{
}

FrameBuffer::~FrameBuffer()
{
}

void FrameBuffer::AttachColorTexture( Render::Texture* texture, unsigned int mipmapLevel, unsigned int layer )
{
}

unsigned int FrameBuffer::GetWidth() const
{
  return mWidth;
}

unsigned int FrameBuffer::GetHeight() const
{
  return mHeight;
}


} //Render

} //Internal

} //Dali
