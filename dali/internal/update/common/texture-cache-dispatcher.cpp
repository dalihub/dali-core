/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// CLASS HEADER
#include <dali/internal/update/common/texture-cache-dispatcher.h>

// INTERNAL INCLUDES
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/gl-resources/texture-cache.h>

namespace Dali
{

namespace Internal
{

// value types used by messages
template <> struct ParameterType< Pixel::Format > : public BasicType< Pixel::Format > {};
template <> struct ParameterType< RenderBuffer::Format > : public BasicType< RenderBuffer::Format > {};

namespace SceneGraph
{

TextureCacheDispatcher::TextureCacheDispatcher( RenderQueue& renderQueue, TextureCache& textureCache )
: mRenderQueue( renderQueue ),
  mTextureCache( textureCache ),
  mSceneGraphBuffers(NULL)
{
}

TextureCacheDispatcher::~TextureCacheDispatcher()
{
}

void TextureCacheDispatcher::SetBufferIndices( const SceneGraphBuffers* bufferIndices )
{
  mSceneGraphBuffers = bufferIndices;
}

/********************************************************************************
 **********************  Implements TextureCacheDispatcher  *********************
 ********************************************************************************/

void TextureCacheDispatcher::DispatchCreateTexture( ResourceId        id,
                                                    unsigned int      width,
                                                    unsigned int      height,
                                                    Pixel::Format     pixelFormat,
                                                    bool              clearPixels )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue5< TextureCache, ResourceId, unsigned int, unsigned int, Pixel::Format, bool > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( &mTextureCache, &TextureCache::CreateTexture, id, width, height, pixelFormat, clearPixels );
  }
}

void TextureCacheDispatcher::DispatchCreateTextureForBitmap( ResourceId id, Integration::Bitmap* bitmap )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue2< TextureCache, ResourceId, Integration::BitmapPtr > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( &mTextureCache, &TextureCache::AddBitmap, id, bitmap );
  }
}

void TextureCacheDispatcher::DispatchCreateTextureForNativeImage( ResourceId id, NativeImageInterfacePtr nativeImage )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue2< TextureCache, ResourceId, NativeImageInterfacePtr > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( &mTextureCache, &TextureCache::AddNativeImage, id, nativeImage );
  }
}

void TextureCacheDispatcher::DispatchCreateGlTexture( ResourceId id )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue1< TextureCache, ResourceId > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( &mTextureCache, &TextureCache::CreateGlTexture, id );
  }
}

void TextureCacheDispatcher::DispatchCreateTextureForFrameBuffer( ResourceId id, unsigned int width, unsigned int height, Pixel::Format pixelFormat, RenderBuffer::Format bufferFormat )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue5< TextureCache, ResourceId, unsigned int, unsigned int, Pixel::Format, RenderBuffer::Format > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( &mTextureCache, &TextureCache::AddFrameBuffer, id, width, height, pixelFormat, bufferFormat );
  }
}

void TextureCacheDispatcher::DispatchCreateTextureForFrameBuffer( ResourceId id, NativeImageInterfacePtr nativeImage )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue2< TextureCache, ResourceId, NativeImageInterfacePtr > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( &mTextureCache, &TextureCache::AddFrameBuffer, id, nativeImage );
  }
}

void TextureCacheDispatcher::DispatchUpdateTexture( ResourceId id, Integration::Bitmap* bitmap )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue2< TextureCache, ResourceId, Integration::BitmapPtr > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( &mTextureCache, &TextureCache::UpdateTexture, id, bitmap );
  }
}

void TextureCacheDispatcher::DispatchUpdateTexture( ResourceId id, Integration::BitmapPtr bitmap , std::size_t xOffset, std::size_t yOffset)
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue4< TextureCache, ResourceId, Integration::BitmapPtr, std::size_t, std::size_t > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( &mTextureCache, &TextureCache::UpdateTexture, id, bitmap, xOffset, yOffset );
  }
}

void TextureCacheDispatcher::DispatchUpdateTexture( ResourceId destId, ResourceId srcId, std::size_t xOffset, std::size_t yOffset )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue4< TextureCache, ResourceId, ResourceId, std::size_t, std::size_t > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( &mTextureCache, &TextureCache::UpdateTexture, destId, srcId, xOffset, yOffset );
  }
}

void TextureCacheDispatcher::DispatchUpdateTexture( ResourceId id, PixelDataPtr pixelData , std::size_t xOffset, std::size_t yOffset)
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue4< TextureCache, ResourceId, PixelDataPtr, std::size_t, std::size_t > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( &mTextureCache, &TextureCache::UpdateTexture, id, pixelData, xOffset, yOffset );
  }
}

void TextureCacheDispatcher::DispatchUpdateTextureArea( ResourceId id, const Dali::RectArea& area )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue2< TextureCache, ResourceId, Dali::RectArea > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( &mTextureCache, &TextureCache::UpdateTextureArea, id, area );
  }
}

void TextureCacheDispatcher::DispatchDiscardTexture( ResourceId id )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue1< TextureCache, ResourceId > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( &mTextureCache, &TextureCache::DiscardTexture, id );
  }
}

} // SceneGraph

} // Internal

} // Dali
