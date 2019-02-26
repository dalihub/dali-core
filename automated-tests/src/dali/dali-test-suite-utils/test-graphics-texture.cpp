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

#include "test-graphics-texture.h"
#include "test-graphics-controller.h"

namespace Test
{

GraphicsTexture::GraphicsTexture(GraphicsController& controller, const GraphicsTextureCreateInfo& createInfo)
: mController(controller),
  mCreateInfo( createInfo )
{
}

GraphicsTexture::~GraphicsTexture()
{
  mController.DestroyTexture(this);
}

void GraphicsTexture::CopyMemory(
  const void *srcMemory, uint32_t srcMemorySize, Dali::Graphics::Extent2D srcExtent, Dali::Graphics::Offset2D dstOffset,
  uint32_t layer, uint32_t level, Dali::Graphics::TextureDetails::UpdateMode updateMode )
{
  Test::TraceCallStack::NamedParams namedParams;
  namedParams["srcMemory"] = Dali::Any(srcMemory);
  namedParams["srcMemorySize"] = Dali::Any( srcMemorySize );
  namedParams["srcExtent.width"] = Dali::Any( srcExtent.width );
  namedParams["srcExtent.height"] = Dali::Any( srcExtent.height );
  namedParams["dstOffset.x"] = Dali::Any( dstOffset.x );
  namedParams["dstOffset.y"] = Dali::Any( dstOffset.y );
  namedParams["layer"] = Dali::Any( layer );
  namedParams["level"] = Dali::Any( level );
  namedParams["updateMode"] = Dali::Any( int(updateMode) );
  mController.mTextureTrace.PushCall("CopyMemory", namedParams);
}

void GraphicsTexture::CopyTexture(
  const Dali::Graphics::Texture &srcTexture, Dali::Graphics::Rect2D srcRegion, Dali::Graphics::Offset2D dstOffset,
  uint32_t layer, uint32_t level, Dali::Graphics::TextureDetails::UpdateMode updateMode )
{
  Test::TraceCallStack::NamedParams namedParams;
  namedParams["srcRegion.width"] = Dali::Any( srcRegion.width );
  namedParams["srcRegion.height"] = Dali::Any( srcRegion.height );
  namedParams["srcRegion.x"] = Dali::Any( srcRegion.x );
  namedParams["srcRegion.y"] = Dali::Any( srcRegion.y );
  namedParams["dstOffset.x"] = Dali::Any( dstOffset.x );
  namedParams["dstOffset.y"] = Dali::Any( dstOffset.y );
  namedParams["layer"] = Dali::Any( layer );
  namedParams["level"] = Dali::Any( level );
  namedParams["updateMode"] = Dali::Any( int(updateMode) );
  mController.mTextureTrace.PushCall("CopyMemory", namedParams);
}

void GraphicsTexture::CopyBuffer(
  const Dali::Graphics::Buffer &srcBuffer, Dali::Graphics::Extent2D srcExtent, Dali::Graphics::Offset2D dstOffset,
  uint32_t layer, uint32_t level, Dali::Graphics::TextureDetails::UpdateMode updateMode )
{
  Test::TraceCallStack::NamedParams namedParams;
  namedParams["srcExtent.width"] = Dali::Any( srcExtent.width );
  namedParams["srcExtent.height"] = Dali::Any( srcExtent.height );
  namedParams["dstOffset.x"] = Dali::Any( dstOffset.x );
  namedParams["dstOffset.y"] = Dali::Any( dstOffset.y );
  namedParams["layer"] = Dali::Any( layer );
  namedParams["level"] = Dali::Any( level );
  namedParams["updateMode"] = Dali::Any( int(updateMode) );
  mController.mTextureTrace.PushCall("CopyMemory", namedParams);
}

} // namespace Test
