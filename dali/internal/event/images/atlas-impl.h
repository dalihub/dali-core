#ifndef __DALI_INTERNAL_ATLAS_H__
#define __DALI_INTERNAL_ATLAS_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-vector.h>
#include <dali/devel-api/images/atlas.h>
#include <dali/internal/event/images/context-recovery-interface.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/event/images/buffer-image-impl.h>

namespace Dali
{

namespace Internal
{

class ResourceClient;

typedef Dali::Atlas::SizeType SizeType;

/**
 * Internal class for Dali::Atlas
 */
class Atlas : public Image, public ContextRecoveryInterface
{
public:

  /**
   * @brief Create a new Atlas.
   *
   * @pre width & height are greater than zero.
   * The maximum size of the atlas is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] width          The atlas width in pixels.
   * @param [in] height         The atlas height in pixels.
   * @param [in] pixelFormat    The pixel format.
   * @param [in] recoverContext Whether re-uploading the resource images automatically when regaining the context
   * @return A pointer to a new Atlas.
   */
  static Atlas* New( SizeType width,
                     SizeType height,
                     Pixel::Format pixelFormat,
                     bool recoverContext);

  /**
   * @copydoc Dali::Atlas::Clear
   */
  void Clear( const Vector4& color  );

  /**
   * @copydoc Dali::Atlas::Upload( const BufferImage&, uint32_t, uint32_t )
   */
  bool Upload( BufferImage& bufferImage,
               SizeType xOffset,
               SizeType yOffset );

  /**
   * @copydoc Dali::Atlas::Upload( const std::string&, uint32_t, uint32_t )
   */
  bool Upload( const std::string& url,
               SizeType xOffset,
               SizeType yOffset );

  /**
   * @copydoc Dali::Atlas::Upload( Dali::Atlas::PixelDataPtr, uint32_t )
   */
  bool Upload( PixelDataPtr pixelData,
               SizeType xOffset,
               SizeType yOffset );

  /**
   * @copydoc ContextRecoveryInterface::RecoverFromContextLoss
   */
  virtual void RecoverFromContextLoss();

protected:

  /**
   * @brief Protected constructor.
   *
   * @pre width & height are greater than zero.
   * The maximum size of the atlas is limited by GL_MAX_TEXTURE_SIZE.
   * @param [in] width          The atlas width in pixels.
   * @param [in] height         The atlas height in pixels.
   * @param [in] pixelFormat    The pixel format.
   * @param [in] recoverContext Whether re-uploading the resource images automatically when regaining the context
   */
  Atlas( SizeType width,
         SizeType height,
         Pixel::Format pixelFormat,
         bool recoverContext);

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~Atlas();

  /**
   * @copydoc Dali::Internal::Image::Connect
   */
  virtual void Connect();

  /**
   * @copydoc Dali::Internal::Image::Disconnect
   */
  virtual void Disconnect();

private:

  /**
   * Helper for Upload methods
   * @return True if the size of the bitmap fits within the atlas at the specified offset
   */
  bool IsInside( SizeType x, SizeType y );

  /**
   * Helper to create the Atlas resource
   */
  void AllocateAtlas();

  /**
   * Helper to release the Atlas resource
   */
  void ReleaseAtlas();

  /**
   * Upload a bitmap with the given color to clear the background.
   */
  void ClearBackground( const Vector4& color  );

  /**
   * Clear all the current tiles and resources of the atlas
   */
  void ClearCache();

  /**
   * Load the bitmap data from the url
   */
  Integration::BitmapPtr LoadBitmap( const std::string& url );

private:

  /**
   * Record of the url resource in the Atlas.
   */
  struct Tile
  {
    Tile( SizeType xOffset, SizeType yOffset, const std::string& url )
    : xOffset( xOffset ), yOffset( yOffset ), url(url)
    {}

    ~Tile(){};

    SizeType xOffset;   ///< Offset in the x direction within the atlas
    SizeType yOffset;   ///< Offset in the y direction within the atlas
    std::string url;    ///< The URL of the resource image file to use

  private:
    Tile(const Tile& rhs); ///< not defined
    Tile& operator=(const Tile& rhs); ///< not defined
  };

private:

  ResourceClient&          mResourceClient;
  ImageFactory&            mImageFactory;
  Vector4                  mClearColor;       ///< The background clear color
  Vector<Tile*>            mTiles;            ///< The url resources, which would recover automatically when regaining context
  Pixel::Format            mPixelFormat;      ///< The pixel format (rgba 32 bit by default)
  bool                     mClear:1;          ///< Clear the backgound or not
  bool                     mRecoverContext:1; ///< Re-upload the url resources or not when regaining context
};

} // namespace Internal

/**
 * Helper methods for public API.
 */
inline Internal::Atlas& GetImplementation(Dali::Atlas& image)
{
  DALI_ASSERT_ALWAYS( image && "Atlas handle is empty" );

  BaseObject& handle = image.GetBaseObject();

  return static_cast<Internal::Atlas&>(handle);
}

inline const Internal::Atlas& GetImplementation(const Dali::Atlas& image)
{
  DALI_ASSERT_ALWAYS( image && "Atlas handle is empty" );

  const BaseObject& handle = image.GetBaseObject();

  return static_cast<const Internal::Atlas&>(handle);
}

} // namespace Dali

#endif // __DALI_INTERNAL_ATLAS_H__
