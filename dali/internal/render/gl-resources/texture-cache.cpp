//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <dali/internal/render/gl-resources/texture-cache.h>

#include <dali/integration-api/bitmap.h>

#include <dali/internal/update/resources/resource-manager-declarations.h>
#include <dali/internal/render/common/post-process-resource-dispatcher.h>
#include <dali/internal/render/queue/render-queue.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/render/gl-resources/texture-factory.h>
#include <dali/internal/render/gl-resources/texture.h>
#include <dali/internal/render/gl-resources/texture-observer.h>
#include <dali/internal/render/gl-resources/bitmap-texture.h>
#include <dali/internal/render/gl-resources/native-texture.h>
#include <dali/internal/render/gl-resources/frame-buffer-texture.h>

#include <dali/integration-api/debug.h>

using Dali::Internal::Texture;
using Dali::Internal::FrameBufferTexture;
using Dali::Integration::Bitmap;

namespace Dali
{
namespace Internal
{

// value types used by messages
template <> struct ParameterType< Pixel::Format > : public BasicType< Pixel::Format > {};

namespace SceneGraph
{

TextureCache::TextureCache( RenderQueue& renderQueue,
                            PostProcessResourceDispatcher& postProcessResourceDispatcher,
                            Context& context)
: TextureCacheDispatcher(renderQueue),
  mPostProcessResourceDispatcher(postProcessResourceDispatcher),
  mContext(context)
{
}

TextureCache::~TextureCache()
{
}

void TextureCache::CreateTexture( ResourceId        id,
                                  unsigned int      width,
                                  unsigned int      height,
                                  Pixel::Format     pixelFormat,
                                  bool              clearPixels )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::CreateTexture(id=%i width:%u height:%u)\n", id, width, height);

  Texture* texture = TextureFactory::NewBitmapTexture(width, height, pixelFormat, clearPixels, mContext);
  mTextures.insert(TexturePair(id, texture));
}

void TextureCache::AddBitmap(ResourceId id, Integration::BitmapPtr bitmap)
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::AddBitmap(id=%i Bitmap:%p)\n", id, bitmap.Get());

  Texture* texture = TextureFactory::NewBitmapTexture(bitmap.Get(), mContext);
  mTextures.insert(TexturePair(id, texture));
}

void TextureCache::AddNativeImage(ResourceId id, NativeImagePtr nativeImage)
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::AddNativeImage(id=%i NativeImg:%p)\n", id, nativeImage.Get());

  /// TODO - currently a new Texture is created even if we reuse the same NativeImage
  Texture* texture = TextureFactory::NewNativeImageTexture(*nativeImage, mContext);
  mTextures.insert(TexturePair(id, texture));
}

void TextureCache::AddFrameBuffer( ResourceId id, unsigned int width, unsigned int height, Pixel::Format pixelFormat )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::AddFrameBuffer(id=%i width:%u height:%u)\n", id, width, height);

  // Note: Do not throttle framebuffer generation - a request for a framebuffer should always be honoured
  // as soon as possible.
  Texture* texture = TextureFactory::NewFrameBufferTexture( width, height, pixelFormat, mContext );
  mFramebufferTextures.insert(TexturePair(id, texture));
}

void TextureCache::AddFrameBuffer( ResourceId id, NativeImagePtr nativeImage )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::AddFrameBuffer(id=%i width:%u height:%u)\n", id, nativeImage->GetWidth(), nativeImage->GetHeight());

  // Note: Do not throttle framebuffer generation - a request for a framebuffer should always be honoured
  // as soon as possible.
  Texture* texture = TextureFactory::NewFrameBufferTexture( nativeImage, mContext );
  mFramebufferTextures.insert(TexturePair(id, texture));
}

void TextureCache::UpdateTexture( ResourceId id, Integration::BitmapPtr bitmap )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::UpdateTexture(id=%i bitmap:%p )\n", id, bitmap.Get());

  TextureIter textureIter = mTextures.find(id);
  if( textureIter != mTextures.end() )
  {
    // we have reloaded the image from file, update texture
    TexturePointer texturePtr = textureIter->second;
    if( texturePtr )
    {
      texturePtr->Update(bitmap.Get());

      ResourcePostProcessRequest ppRequest( id, ResourcePostProcessRequest::UPLOADED );
      mPostProcessResourceDispatcher.DispatchPostProcessRequest(ppRequest);
    }
  }
}

void TextureCache::UpdateTextureArea( ResourceId id, const Dali::RectArea& area )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::UpdateTextureArea(id=%i)\n", id );

  TextureIter textureIter = mTextures.find(id);
  if( textureIter != mTextures.end() )
  {
    TexturePointer texturePtr = textureIter->second;
    if( texturePtr )
    {
      texturePtr->UpdateArea( area );

      ResourcePostProcessRequest ppRequest( id, ResourcePostProcessRequest::UPLOADED );
      mPostProcessResourceDispatcher.DispatchPostProcessRequest(ppRequest);
    }
  }
}

void TextureCache::AddBitmapUploadArray( ResourceId id, const BitmapUploadArray& uploadArray )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::AddBitmapUploadArray(id=%i )\n", id);

  TextureIter textureIter = mTextures.find(id);

  DALI_ASSERT_DEBUG( textureIter != mTextures.end() );
  if( textureIter != mTextures.end() )
  {
    TexturePointer texturePtr = textureIter->second;
    if( texturePtr )
    {
      BitmapTexture* texture = static_cast< BitmapTexture* >( texturePtr.Get() );
      texture->UploadBitmapArray( uploadArray );
    }
  }
}

void TextureCache::ClearAreas( ResourceId id,
                               const BitmapClearArray& areaArray,
                               std::size_t blockSize,
                               uint32_t color )
{
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::ClearAreas(id: %d)\n", id);
  TextureIter textureIter = mTextures.find(id);
  DALI_ASSERT_DEBUG( textureIter != mTextures.end() );
  if( textureIter != mTextures.end() )
  {
    TexturePointer texturePtr = textureIter->second;
    if( texturePtr )
    {
      BitmapTexture* texture = static_cast< BitmapTexture* >( texturePtr.Get() );
      texture->ClearAreas( areaArray, blockSize, color );
    }
  }
}

void TextureCache::DiscardTexture( ResourceId id )
{
  bool deleted = false;

  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::DiscardTexture(id:%u)\n", id);

  if( mTextures.size() > 0)
  {
    TextureIter iter = mTextures.find(id);
    if( iter != mTextures.end() )
    {
      TexturePointer texturePtr = iter->second;
      if( texturePtr )
      {
        // if valid texture pointer, cleanup GL resources
        texturePtr->GlCleanup();
      }
      mTextures.erase(iter);
      deleted = true;
    }
  }

  if( mFramebufferTextures.size() > 0)
  {
    TextureIter iter = mFramebufferTextures.find(id);
    if( iter != mFramebufferTextures.end() )
    {
      TexturePointer texturePtr = iter->second;
      if( texturePtr )
      {
        // if valid texture pointer, cleanup GL resources
        texturePtr->GlCleanup();
      }
      mFramebufferTextures.erase(iter);
      deleted = true;
    }
  }

  if(deleted)
  {
    if( mObservers.size() > 0 )
    {
      TextureResourceObserversIter observersIter = mObservers.find(id);
      if( observersIter != mObservers.end() )
      {
        TextureObservers observers = observersIter->second;
        for( TextureObserversIter iter = observers.begin(); iter != observers.end(); ++iter )
        {
          TextureObserver* observer = *iter;
          observer->TextureDiscarded( id );
        }

        mObservers.erase( observersIter );
      }
    }

    // Tell resource manager
    ResourcePostProcessRequest ppRequest( id, ResourcePostProcessRequest::DELETED );
    mPostProcessResourceDispatcher.DispatchPostProcessRequest(ppRequest);
  }
}

void TextureCache::BindTexture( Texture *texture, ResourceId id, GLenum target, GLenum textureunit )
{
  bool created = texture->Bind(target, textureunit);
  if( created && texture->UpdateOnCreate() ) // i.e. the pixel data was sent to GL
  {
    ResourcePostProcessRequest ppRequest( id, ResourcePostProcessRequest::UPLOADED );
    mPostProcessResourceDispatcher.DispatchPostProcessRequest(ppRequest);
  }
}


Texture* TextureCache::GetTexture(ResourceId id)
{
  Texture* texture = NULL;
  TextureIter iter = mTextures.find(id);

  if( iter != mTextures.end() )
  {
    TexturePointer texturePtr = iter->second;
    if( texturePtr )
    {
      texture = texturePtr.Get();
    }
  }

  if( texture == NULL )
  {
    TextureIter iter = mFramebufferTextures.find(id);
    if( iter != mFramebufferTextures.end() )
    {
      TexturePointer texturePtr = iter->second;
      if( texturePtr )
      {
        texture = texturePtr.Get();
      }
    }
  }

  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::GetTexture(id:%u) : %p\n", id, texture);

  return texture;
}

FrameBufferTexture* TextureCache::GetFramebuffer(ResourceId id)
{
  FrameBufferTexture* offscreen = NULL;
  TextureIter iter = mFramebufferTextures.find(id);

  DALI_ASSERT_DEBUG( iter != mFramebufferTextures.end() );

  if( iter != mFramebufferTextures.end() )
  {
    TexturePointer texturePtr = iter->second;
    if( texturePtr )
    {
      offscreen = dynamic_cast< FrameBufferTexture* >( texturePtr.Get() );
    }
  }
  DALI_ASSERT_DEBUG( offscreen );

  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "TextureCache::GetFramebuffer(id:%u) : %p\n", id, offscreen);

  return offscreen;
}

void TextureCache::AddObserver( ResourceId id, TextureObserver* observer )
{
  TextureResourceObserversIter observersIter = mObservers.find(id);
  if( observersIter != mObservers.end() )
  {
    TextureObservers& observers = observersIter->second;
    bool foundObserver = false;
    for( TextureObserversIter iter = observers.begin(); iter != observers.end(); ++iter )
    {
      if( *iter == observer )
      {
        foundObserver = true;
        break;
      }
    }
    if( ! foundObserver )
    {
      observers.push_back(observer);
    }
  }
  else
  {
    TextureObservers observers;
    observers.push_back(observer);
    mObservers.insert(std::pair<ResourceId, TextureObservers>(id, observers));
  }
}

void TextureCache::RemoveObserver( ResourceId id, TextureObserver* observer )
{
  TextureResourceObserversIter observersIter = mObservers.find(id);
  if( observersIter != mObservers.end() )
  {
    TextureObservers& observers = observersIter->second;
    for( TextureObserversIter iter = observers.begin(); iter != observers.end(); ++iter )
    {
      if(*iter == observer)
      {
        observers.erase(iter);
        break;
      }
    }
  }
}

/********************************************************************************
 **********************  Implements TextureCacheDispatcher  *********************
 ********************************************************************************/

void TextureCache::DispatchCreateTexture( ResourceId        id,
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
    new (slot) DerivedType( this, &TextureCache::CreateTexture, id, width, height, pixelFormat, clearPixels );
  }
}

void TextureCache::DispatchCreateTextureForBitmap( ResourceId id, Bitmap* bitmap )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue2< TextureCache, ResourceId, Integration::BitmapPtr > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( this, &TextureCache::AddBitmap, id, bitmap );
  }
}

void TextureCache::DispatchCreateTextureForNativeImage( ResourceId id, NativeImagePtr nativeImage )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue2< TextureCache, ResourceId, NativeImagePtr > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( this, &TextureCache::AddNativeImage, id, nativeImage );
  }
}

void TextureCache::DispatchCreateTextureForFrameBuffer( ResourceId id, unsigned int width, unsigned int height, Pixel::Format pixelFormat )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue4< TextureCache, ResourceId, unsigned int, unsigned int, Pixel::Format > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( this, &TextureCache::AddFrameBuffer, id, width, height, pixelFormat );
  }
}

void TextureCache::DispatchCreateTextureForFrameBuffer( ResourceId id, NativeImagePtr nativeImage )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue2< TextureCache, ResourceId, NativeImagePtr > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( this, &TextureCache::AddFrameBuffer, id, nativeImage );
  }
}

void TextureCache::DispatchUpdateTexture( ResourceId id, Bitmap* bitmap )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue2< TextureCache, ResourceId, Integration::BitmapPtr > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( this, &TextureCache::UpdateTexture, id, bitmap );
  }
}

void TextureCache::DispatchUpdateTextureArea( ResourceId id, const Dali::RectArea& area )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue2< TextureCache, ResourceId, Dali::RectArea > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( this, &TextureCache::UpdateTextureArea, id, area );
  }
}

void TextureCache::DispatchUploadBitmapArrayToTexture( ResourceId id, const BitmapUploadArray& uploadArray )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue2< TextureCache, ResourceId, BitmapUploadArray > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( this, &TextureCache::AddBitmapUploadArray, id, uploadArray );
  }
}

void TextureCache::DispatchClearAreas( ResourceId id, const BitmapClearArray& areaArray, std::size_t blockSize, uint32_t color )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue4< TextureCache, ResourceId, BitmapClearArray, std::size_t, uint32_t > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( this, &TextureCache::ClearAreas, id, areaArray, blockSize, color );
  }
}

void TextureCache::DispatchDiscardTexture( ResourceId id )
{
  // NULL, means being shutdown, so ignore msgs
  if( mSceneGraphBuffers != NULL )
  {
    typedef MessageValue1< TextureCache, ResourceId > DerivedType;

    // Reserve some memory inside the render queue
    unsigned int* slot = mRenderQueue.ReserveMessageSlot( mSceneGraphBuffers->GetUpdateBufferIndex(), sizeof( DerivedType ) );

    // Construct message in the render queue memory; note that delete should not be called on the return value
    new (slot) DerivedType( this, &TextureCache::DiscardTexture, id );
  }
}

} // SceneGraph

} // Internal

} // Dali
