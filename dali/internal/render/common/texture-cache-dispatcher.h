#ifndef __DALI_INTERNAL_TEXTURE_CACHE_DISPATCHER_H__
#define __DALI_INTERNAL_TEXTURE_CACHE_DISPATCHER_H__

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
#include <dali/public-api/images/native-image.h>
#include <dali/public-api/images/bitmap-image.h>
#include <dali/public-api/images/pixel.h>
#include <dali/internal/common/message.h>
#include <dali/internal/update/common/scene-graph-buffers.h>
#include <dali/internal/common/bitmap-upload.h>
#include <dali/integration-api/resource-declarations.h>
#include <dali/integration-api/bitmap.h>

namespace Dali
{

namespace Internal
{
typedef Integration::ResourceId ResourceId;

namespace SceneGraph
{

class RenderQueue;

class TextureCacheDispatcher
{
public:

  /**
   * Constructor
   * @param[in] renderQueue The queue to which to send messages
   */
  TextureCacheDispatcher( RenderQueue& renderQueue );

  /**
   * Virtual destructor; TextureCacheDispatcher is a base class.
   */
  virtual ~TextureCacheDispatcher();

  /**
   * mRenderQueue needs the update buffer index when any of the dispatch methods are
   * called. Should really store a functor to get this index, but existing functors
   * use references. Could heap allocate the functor, but that seems overly wasteful.
   * Instead, store a pointer to the SceneGraphBuffers object, and call the relevant
   * function when required. (increases coupling, requires object lifetime management :/ )
   *
   * @param[in] bufferIndices Pointer to SceneGraphBuffers object that can query
   * the current RenderQueue buffer index.
   */
  void SetBufferIndices( const SceneGraphBuffers* bufferIndices );

  /**
   * Dispatch a message to create an empty texture and add it to the texture cache.
   * May be called from Update thread
   * @param[in] id Resource Id of the texture
   * @param[in] width Width of the texture
   * @param[in] height Height of the texture
   * @param[in] pixelFormat Pixel format of the texture
   * @param[in] clearPixels True if the data should be cleared to 0 on creation
   */
  virtual void DispatchCreateTexture( ResourceId        id,
                                      unsigned int      width,
                                      unsigned int      height,
                                      Pixel::Format     pixelFormat,
                                      bool              clearPixels ) = 0;

  /**
   * Dispatch a message to add a texture for bitmap.
   * May be called from Update thread
   * @param[in] id Resource Id of the bitmap
   * @param[in] bitmap The bitmap
   */
  virtual void DispatchCreateTextureForBitmap( ResourceId id, Integration::Bitmap* bitmap ) = 0;

  /**
   * Dispatch a message to add a native image to the texture cache
   * May be called from Update thread
   * @param[in] id Resource Id of the native image
   * @param[in] nativeImage The native image
   */
  virtual void DispatchCreateTextureForNativeImage( ResourceId id, NativeImagePtr nativeImage ) = 0;

  /**
   * Dispatch a message to create a framebuffer texture and add it to the texture cache
   * May be called from Update thread
   * @param[in] id Resource Id of the framebuffer
   * @param[in] width Width of the framebuffer
   * @param[in] height Height of the framebuffer
   * @param[in] pixelFormat Pixel format of the framebuffer
   */
  virtual void DispatchCreateTextureForFrameBuffer( ResourceId id, unsigned int width, unsigned int height, Pixel::Format pixelFormat ) = 0;

  /**
   * Dispatch a message to create a framebuffer texture and add it to the texture cache
   * May be called from Update thread
   * @param[in] id Resource Id of the framebuffer
   * @param[in] nativeImage The NativeImage
   */
  virtual void DispatchCreateTextureForFrameBuffer( ResourceId id, NativeImagePtr nativeImage ) = 0;

  /**
   * Dispatch a message to update the texture.
   * May be called from Update thread
   * @param[in] id Resource Id of the texture
   * @param[in] bitmap The updated bitmap
   */
  virtual void DispatchUpdateTexture( ResourceId id, Integration::Bitmap* bitmap ) = 0;

  /**
   * Dispatch a message to update the texture area
   * May be called from the Update thread
   * @param[in] id Resource Id of the texture
   * @param[in] area The area of the bitmap that has changed
   */
  virtual void DispatchUpdateTextureArea( ResourceId id, const RectArea& area ) = 0;

 /**
   * Dispatch a message to insert an array of bitmaps into the texture.
   * Used for uploading multiple images into an atlas.
   * @param[in] id Resource id of the texture
   * @param[in] uploadArray array of BitmapUpload structures
   */
  virtual void DispatchUploadBitmapArrayToTexture( ResourceId id, const BitmapUploadArray& uploadArray ) = 0;

  /**
   * Dispatch a message to clear multiple areas of a texture to a specific color
   * @param[in] id Resource id of the texture to clear
   * @param[in] area Rectangles to clear
   * @param[in] blockSize Size of block to clear
   * @param[in] color Clear color (as a pixel value)
   */
  virtual void DispatchClearAreas( ResourceId id, const BitmapClearArray& area, std::size_t blockSize, uint32_t color ) = 0;

  /**
   * Dispatch a message to discard a texture
   * May be called from Update thread
   * @param[in] id Resource Id of the texture
   */
  virtual void DispatchDiscardTexture( ResourceId id ) = 0;

protected:

  RenderQueue&             mRenderQueue;
  const SceneGraphBuffers* mSceneGraphBuffers;
};

} // SceneGraph

} // Internal

} // Dali

#endif // __DALI_INTERNAL_TEXTURE_CACHE_DISPATCHER_H__
