#ifndef __DALI_INTERNAL_TEXTURE_H__
#define __DALI_INTERNAL_TEXTURE_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>
#include <dali/integration-api/bitmap.h>
#include <dali/internal/render/common/uv-rect.h>
#include <dali/integration-api/gl-abstraction.h>
#include <dali/internal/render/gl-resources/gl-resource-owner.h>
#include <dali/public-api/images/image.h>
#include <dali/public-api/images/pixel.h>
#include <dali/public-api/images/native-image.h>
#include <dali/public-api/math/rect.h>
#include <dali/public-api/actors/sampling.h>

namespace Dali
{

class NativeImage;

namespace Internal
{

class Context;
struct Vertex3D;
struct Vertex2D;

/**
 * Texture class.
 */
class Texture: public RefObject,
               public GlResourceOwner
{
public:

  /**
   * Used to define the area of the texture to display
   */
  typedef Rect<int> PixelArea;

  /**
   * Used to define a region of a bitmap.
   */
  typedef Rect<unsigned int>    RectArea;     ///< rectangular area (x,y,w,h)

  /**
   * Initialization method for Texture.
   * Might or might not be needed in specific implementations.
   * @return true if successful, false otherwise
   */
  virtual bool Init() = 0;

  /**
   * Update the texture with the bitmap.
   */
  virtual void Update(Integration::Bitmap* bitmap);

  /**
   * Update the texture from the modified bitmap.
   * @param area to update
   */
  virtual void UpdateArea( const RectArea& area );

  /**
   * @return Return true if the texture should be updated on GL texture creation.
   */
  virtual bool UpdateOnCreate();

  /**
   * Binds the texture for use.
   * Only when Bind() is first called, does a texture create an
   * an opengl texture.
   * @param target (e.g. GL_TEXTURE_2D)
   * @param textureunit to bind to
   * @return True if the opengl texture was created
   */
  virtual bool Bind(GLenum target, GLenum textureunit);

  /**
   * Returns GL texture ID
   * @return texture id
   */
  unsigned int GetTextureId()
  {
    return mId;
  }

  /**
   * Return the width of image in pixels.
   * @return width
   */
  virtual unsigned int GetWidth() const;

  /**
   * Return the height of image in pixels.
   * @return height
   */
  virtual unsigned int GetHeight() const;

  /**
   * Query whether the texture data has an alpha channel.
   * @return True if the texture data has an alpha channel.
   */
  virtual bool HasAlphaChannel() const = 0;

  /**
   * Query whether the texture is completely opaque
   * @return True if all pixels of the texture data are opaque
   */
  virtual bool IsFullyOpaque() const = 0;

  /**
   * Get the pixel format of the image data.
   * @return the pixel format of the image.
   */
  virtual Pixel::Format GetPixelFormat() const;

  /**
   * Sets the texture id.
   * @param id OpenGL texture id
   */
  void SetTextureId(GLuint id);

  /**
   * When creating a texture mapped object, the developer can
   * can assume the texture u,v coordinates have a range of 0 to 1.
   * They then just call MapUV which will adjust uv values depending on
   * whether a pixel area is being used or not.
   *@param[in] numVerts number of vertices
   *@param[out] verts pointer to an array of vertex objects
   *@param[in] pixelArea the area of the texture to display, null = use default image area
   */
  void MapUV(unsigned int numVerts, Dali::Internal::Vertex3D* verts,  const PixelArea* pixelArea = NULL);

  /**
   * @copydoc MapUV(unsigned int,Dali::Internal::Vertex3D*, const PixelArea* pixelArea)
   */
  void MapUV(unsigned int numVerts, Dali::Internal::Vertex2D* verts, const PixelArea* pixelArea = NULL);

  /**
   * @copydoc MapUV(unsigned int,Dali::Internal::Vertex3D*, const PixelArea* pixelArea)
   * @param[in] stride The number of floats on each row of the vertex object table
   */
  void MapUV(unsigned int numVerts, float* verts, unsigned int stride, const PixelArea* pixelArea = NULL);

  /**
   * Gets the texture coordinates for the texture.
   * The texture maybe in an atlas or may only be part of a texture (that's been padded to be a power of 2).
   * Why do we specify u,v coordinates for all 4 points of a texture, instead of just having bottom left u,v and top right u,v?
   * If the texture is an atlas it maybe rotated, to encode that information we need to use all 4 u,v points.
   * @param[out] uv coordinates.
   * @param[in] pixelArea the area of the texture to display, null = use default image area
   */
  void GetTextureCoordinates(UvRect& uv, const PixelArea* pixelArea = NULL);

  /**
   * @brief Apply the given sampler to the texture.
   *
   * @param[in] samplerBitfield A bitfield with packed sampler options.
   */
  void ApplySampler( unsigned int samplerBitfield );

protected:

  /**
   * Constructor.
   * @param[in] context The GL context
   * @param[in] width       The buffer width
   * @param[in] height      The buffer height
   * @param[in] imageWidth  The image width
   * @param[in] imageHeight The image height
   * @param[in] pixelFormat The pixel format
   */
  Texture( Context&      context,
           unsigned int  width,
           unsigned int  height,
           unsigned int  imageWidth,
           unsigned int  imageHeight,
           Pixel::Format pixelFormat );
  /**
   * Constructor.
   * @param[in] context The GL context
   * @param[in] width       Both the buffer width and the image width (they are equal)
   * @param[in] height      Both the buffer height and the image height.
   * @param[in] pixelFormat The pixel format
   */
  Texture( Context&      context,
           unsigned int  width,
           unsigned int  height,
           Pixel::Format pixelFormat );

  /**
   * Initialize texture for rendering.
   * @return true on success
   */
  virtual bool CreateGlTexture() = 0;

public:
  /**
   * Destructor.
   * Delete the GL texture associated with it.
   */
  virtual ~Texture();

public: // From GlResourceOwner

  /**
   * @copydoc Dali::Internal::GlResourceOwner::GlContextDestroyed()
   */
  virtual void GlContextDestroyed();

  /**
   * @copydoc Dali::Internal::GlResourceOwner::GlCleanup()
   */
  virtual void GlCleanup();

private:

  // Undefined
  Texture(const Texture&);

  // Undefined
  Texture& operator=(const Texture& rhs);

  /**
   * Helper function for GetTextureCoordinates.
   * Gets the texture co-ordinates without using a pixel area.
   * It is possible the image is smaller than the texture
   * so the texture co-ordinates have to be adjusted.
   * @param uv texture co-ordinates
   */
  void GetDefaultTextureCoordinates(UvRect& uv) const;

  /**
   * @brief Apply the given texture parameters.
   *
   * @param[in] filterType Minification or magnification.
   * @param[in] currentFilterMode The current filter mode.
   * @param[in] newFilterMode The new filter mode.
   * @param[in] daliDefault The default dali filter mode for the given filterType.
   * @param[in] systemDefault The default system filter mode for the given filterType.
   */
  void ApplyTextureParameter( GLint filterType, FilterMode::Type currentFilterMode, FilterMode::Type newFilterMode, GLint daliDefault, GLint systemDefault );

protected:

  Context&      mContext;      ///< The GL Context

  GLuint        mId;           ///< Texture id

  unsigned int  mSamplerBitfield;    ///< The packed bitfield of the current sampler

  unsigned int  mWidth;        ///< texture width, may be scaled power of 2 (if not in an atlas)
  unsigned int  mHeight;       ///< texture width, may be scaled power of 2 (if not in an atlas)

  unsigned int  mImageWidth;   ///< width of the original image (may be smaller than texture width)
  unsigned int  mImageHeight;  ///< height of the original image (may be smaller than texture height)

  Pixel::Format mPixelFormat;  ///< Pixel format of the contained image data.
  bool          mDiscarded;    ///< True if texture was added to the DiscardQueue

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_TEXTURE_H__
