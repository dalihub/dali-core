#ifndef __DALI_INTERNAL_SCENE_GRAPH_TEXTURE_CACHE_H__
#define __DALI_INTERNAL_SCENE_GRAPH_TEXTURE_CACHE_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <stdint.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/map-wrapper.h>
#include <dali/public-api/common/intrusive-ptr.h>
#include <dali/public-api/images/native-image.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/vector4.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/integration-api/resource-policies.h>
#include <dali/integration-api/gl-abstraction.h>
#include <dali/internal/common/owner-pointer.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/internal/render/common/texture-cache-dispatcher.h>
#include <dali/internal/render/gl-resources/texture-declarations.h>
#include <dali/internal/render/gl-resources/texture-units.h>

namespace Dali
{
class NativeImageInterface;


namespace Integration
{
class Bitmap;
typedef IntrusivePtr<Bitmap>   BitmapPtr;
}


namespace Internal
{
class Context;
class Texture;
class BitmapTexture;
class FrameBufferTexture;
class TextureObserver;

namespace SceneGraph
{
class RenderQueue;
class PostProcessResourceDispatcher;

typedef std::map<ResourceId, TexturePointer >   TextureContainer;
typedef std::pair<ResourceId, TexturePointer >  TexturePair;
typedef TextureContainer::iterator              TextureIter;
typedef TextureContainer::const_iterator        TextureConstIter;

/**
 * Caches textures. Owned by Render Thread
 */
class TextureCache : public TextureCacheDispatcher
{
public:
 /**
   * Constructor
   * @param[in] renderQueue Queue to use for dispatching messages to this object
   * @param[in] postProcessDispatcher Dispatcher for resource post processing requests
   * @param[in] context GL Context
   */
  TextureCache( RenderQueue& renderQueue,
                PostProcessResourceDispatcher& postProcessDispatcher,
                Context& context );

  /**
   * Destructor
   */
  ~TextureCache();

  /**
   * Creates a new empty texture object with the given dimensions.
   * @param[in] width       The width (pixels)
   * @param[in] height      The height (pixels)
   * @param[in] pixelFormat The pixel format
   * @param[in] clearPixels True if the pixel data should be cleared first
   */
  void CreateTexture( ResourceId        id,
                      unsigned int      width,
                      unsigned int      height,
                      Pixel::Format     pixelFormat,
                      bool              clearPixels );

  /**
   * Add a bitmap to the texture cache
   * @param[in] id Resource Id of the bitmap
   * @param[in] bitmap The bitmap
   */
  void AddBitmap( ResourceId id, Integration::BitmapPtr bitmap );

  /**
   * Add a native image to the texture cache
   * @param[in] id Resource Id of the native image
   * @param[in] nativeImage The native image
   */
  void AddNativeImage( ResourceId id, NativeImageInterfacePtr nativeImage );

  /**
   * Create a framebuffer texture and add it to the texture cache
   * @param[in] id Resource Id of the native image
   * @param[in] width Width of the framebuffer
   * @param[in] height Height of the framebuffer
   * @param[in] pixelFormat Pixel format of the framebuffer
   */
  void AddFrameBuffer( ResourceId id, unsigned int width, unsigned int height, Pixel::Format pixelFormat );

  /**
   * Create a framebuffer texture and add it to the texture cache
   * @param[in] id Resource Id of the native image
   * @param[in] nativeImage The NativeImage
   */
  void AddFrameBuffer( ResourceId id, NativeImageInterfacePtr nativeImage );

  /**
   * Update the texture with a newly loaded bitmap
   * @param[in] id Resource Id of the bitmap
   * @param[in] bitmap The bitmap
   */
  void UpdateTexture( ResourceId id, Integration::BitmapPtr bitmap );

  /**
   * Update the texture with a newly loaded bitmap
   * @param[in] id Resource Id of the bitmap
   * @param[in] bitmap The bitmap
   * @param[in] xOffset Specifies an offset in the x direction within the texture
   * @param[in] yOffset Specifies an offset in the y direction within the texture
   */
  void UpdateTexture( ResourceId id, Integration::BitmapPtr bitmap, std::size_t xOffset, std::size_t yOffset );

  /**
   * Update the part of a texture with a newly loaded bitmap
   * May be called from Update thread
   * @param[in] destId The ID of the texture to update
   * @param[in] srcId The resource ID of the source bitmap
   * @param[in] xOffset Specifies an offset in the x direction within the texture
   * @param[in] yOffset Specifies an offset in the y direction within the texture
   */
  void UpdateTexture( ResourceId destId, ResourceId srcId, std::size_t xOffset, std::size_t yOffset );

  /**
   * Update the area of the texture from the associated bitmap
   * @param[in] id Resource Id of the bitmap
   * @param[in] area The area of the bitmap that has changed
   */
  void UpdateTextureArea( ResourceId id, const RectArea& area );

  /**
   * Discard texture associated with resource ID
   * @param[in] id Resource Id of the texture
   */
  void DiscardTexture( ResourceId id );

  /**
   * Bind a texture. On the first call, the texture will copy it's
   * pixel data to an OpenGL texture.  If it's a BitmapTexture, then
   * it will also trigger SignalUpdated to be sent on the event thread
   * object.
   *
   * @param[in] texture pointer to the  texture
   * @param[in] id Resource id of texture
   * @param[in] target (e.g. GL_TEXTURE_2D)
   * @param[in] textureunit to use
   */
  void BindTexture( Texture* texture, ResourceId id, GLenum target, TextureUnit textureunit );

  /**
   * Get the texture associated with the resource ID
   * May be a texture or a framebuffer
   * @param[in] id Resource Id of the texture
   * @return NULL if the GL resource hasn't yet been created,
   * or a valid pointer if it has.
   */
  Texture* GetTexture( ResourceId id );

  /**
   * Get the bitmaptexture associated with the resource ID
   * @param[in] id Resource Id of the texture
   * @return The texture or NULL
   */
  BitmapTexture* GetBitmapTexture(ResourceId id);

  /**
   * Get the framebuffer texture associated with the resource ID
   * Used for writing to the framebuffer
   * @param[in] id Resource Id of the framebuffer
   * @return the framebuffer
   */
  FrameBufferTexture* GetFramebuffer(ResourceId id);

  /**
   * Add a texture observer. Should be called in render thread
   * @param[in] id The resource id to watch
   * @param[in] observer The observer to add
   */
  void AddObserver( ResourceId id, TextureObserver* observer );

  /**
   * Remove a texture observer. Should be called in render thread
   * @param[in] id The resource id to stop watching
   * @param[in] observer The observer to remove
   */
  void RemoveObserver( ResourceId id, TextureObserver* observer );

  /**
   * Reset all textures.
   * This method is called when context is or has been deleted.
   */
  void GlContextDestroyed();

  /**
   * Set whether textures should retain or discard their bitmaps after upload to GL
   * @param[in] policy Whether to retain or discard bitmaps
   */
  void SetDiscardBitmapsPolicy( ResourcePolicy::Discardable policy );

  /**
   * Get the discard policy.
   * @return The discard policy.
   */
  ResourcePolicy::Discardable GetDiscardBitmapsPolicy();

protected: // Implements TextureCacheDispatcher

  /**
   * @copydoc TextureCacheDispatcher::DispatchCreateTexture()
   */
  virtual void DispatchCreateTexture( ResourceId        id,
                                      unsigned int      width,
                                      unsigned int      height,
                                      Pixel::Format     pixelFormat,
                                      bool              clearPixels );

  /**
   * @copydoc TextureCacheDispatcher::DispatchCreateTextureForBitmap()
   */
  virtual void DispatchCreateTextureForBitmap( ResourceId id, Integration::Bitmap* bitmap );

  /**
   * @copydoc TextureCacheDispatcher::DispatchCreateTextureForNativeImage()
   */
  virtual void DispatchCreateTextureForNativeImage( ResourceId id, NativeImageInterfacePtr nativeImage );

  /**
   * @copydoc TextureCacheDispatcher::DispatchCreateTextureForFramebuffer()
   */
  virtual void DispatchCreateTextureForFrameBuffer( ResourceId id, unsigned int width, unsigned int height, Pixel::Format pixelFormat );

  /**
   * @copydoc TextureCacheDispatcher::DispatchCreateTextureForFramebuffer()
   */
  virtual void DispatchCreateTextureForFrameBuffer( ResourceId id, NativeImageInterfacePtr nativeImage );

  /**
   * @copydoc TextureCacheDispatcher::DispatchUpdateTexture()
   */
  virtual void DispatchUpdateTexture( ResourceId id, Integration::Bitmap* bitmap );

  /**
   * @copydoc TextureCacheDispatcher::DispatchUpdateTexture()
   */
  virtual void DispatchUpdateTexture( ResourceId id, Integration::BitmapPtr bitmap, std::size_t xOffset, std::size_t yOffset ) ;

  /**
   * @copydoc TextureCacheDispatcher::DispatchUpdateTexture()
   */
  virtual void DispatchUpdateTexture( ResourceId destId, ResourceId srcId, std::size_t xOffset, std::size_t yOffset );

  /**
   * @copydoc TextureCacheDispatcher::DispatchUpdateTextureArea()
   */
  virtual void DispatchUpdateTextureArea( ResourceId id, const RectArea& area );

  /**
   * @copydoc TextureCacheDispatcher::DispatchDiscardTexture()
   */
  virtual void DispatchDiscardTexture( ResourceId id );

private:

  PostProcessResourceDispatcher& mPostProcessResourceDispatcher;
  Context&         mContext;
  TextureContainer mTextures;
  TextureContainer mFramebufferTextures;

  typedef std::vector< TextureObserver* > TextureObservers;
  typedef TextureObservers::iterator      TextureObserversIter;

  typedef std::map< ResourceId, TextureObservers > TextureResourceObservers;
  typedef TextureResourceObservers::iterator       TextureResourceObserversIter;

  TextureResourceObservers mObservers;
  ResourcePolicy::Discardable mDiscardBitmapsPolicy;
};



} // SceneGraph
} // Internal
} // Dali

#endif //__DALI_INTERNAL_SCENE_GRAPH_TEXTURE_CACHE_H__
