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

// CLASS HEADER
#include <dali/internal/render/gl-resources/texture.h>

// EXTERNAL INCLUDES
#include <math.h>
#include <memory.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/render/gl-resources/context.h>
#include <dali/internal/common/image-sampler.h>

namespace Dali
{

namespace Internal
{

namespace
{

// These match the GL specification
const GLint SYSTEM_MINIFY_DEFAULT  = GL_NEAREST_MIPMAP_LINEAR;
const GLint SYSTEM_MAGNIFY_DEFAULT = GL_LINEAR;
const GLint SYSTEM_WRAP_DEFAULT  = GL_CLAMP_TO_EDGE;

// These are the Dali defaults
const GLint DALI_MINIFY_DEFAULT  = GL_LINEAR;
const GLint DALI_MAGNIFY_DEFAULT = GL_LINEAR;

} // namespace

/**
 * @brief Convert a FilterMode to it's corresponding GL type.
 *
 * @param[in] filterMode The FilterMode type.
 * @param[in] defaultfilterMode The filter mode to use if filterMode is DEFAULT.
 * @param[in] defaultSystemFilterMode The filter mode to use if filterMode is NONE.
 * @return Return the equivalent GL type.
 */
GLint FilterModeToGL( FilterMode::Type filterMode, GLint defaultfilterMode, GLint defaultSystemFilterMode )
{
  switch( filterMode )
  {
    case FilterMode::NEAREST:
    {
      return GL_NEAREST;
    }
    case FilterMode::LINEAR:
    {
      return GL_LINEAR;
    }
    case FilterMode::NONE:
    {
      return defaultSystemFilterMode;
    }
    case FilterMode::DEFAULT:
    {
      return defaultfilterMode;
    }
  }

  return GL_LINEAR;
}

GLint WrapModeToGL( WrapMode::Type wrapMode, GLint defaultWrapMode )
{
  switch( wrapMode )
  {
    case WrapMode::DEFAULT:
    {
      return defaultWrapMode;
    }
    case WrapMode::CLAMP_TO_EDGE:
    {
      return GL_CLAMP_TO_EDGE;
    }
    case WrapMode::REPEAT:
    {
      return GL_REPEAT;
    }
    case WrapMode::MIRRORED_REPEAT:
    {
      return GL_MIRRORED_REPEAT;
    }
  }

  return defaultWrapMode;
}

using Dali::Internal::Vertex2D;

using namespace Dali::Pixel;

Texture::Texture(Context&      context,
                 unsigned int  width,
                 unsigned int  height,
                 unsigned int  imageWidth,
                 unsigned int  imageHeight)
: mContext(context),
  mId(0),
  mSamplerBitfield( 0 ),
  mWidth(width),
  mHeight(height),
  mImageWidth(imageWidth),
  mImageHeight(imageHeight)
{
}

Texture::Texture(Context&      context,
                 unsigned int  width,
                 unsigned int  height)
: mContext(context),
  mId(0),
  mSamplerBitfield( 0 ),
  mWidth(width),
  mHeight(height),
  mImageWidth(width),
  mImageHeight(height)
{
}

Texture::~Texture()
{
  // GlCleanup() should already have been called by TextureCache ensuring the resource is destroyed
  // on the render thread. (And avoiding a potentially problematic virtual call in the destructor)
}

void Texture::SetTextureId(GLuint id)
{
  mId=id;
}

void Texture::Update(Integration::Bitmap* bitmap)
{
  DALI_ASSERT_DEBUG( "Updating incorrect texture type" == NULL );
}

void Texture::UpdateArea( const RectArea& area )
{
  DALI_ASSERT_DEBUG( "Updating incorrect texture type" == NULL );
}

bool Texture::UpdateOnCreate()
{
  return false;
}

bool Texture::Bind(GLenum target, TextureUnit textureunit )
{
  // This is the only supported type at the moment
  DALI_ASSERT_DEBUG( target == GL_TEXTURE_2D );
  bool result = true;

  if( mId == 0 )
  {
    result = CreateGlTexture();
  }

  // Bind the texture id
  mContext.BindTextureForUnit(textureunit, mId );

  return result;
}

void Texture::GlContextDestroyed()
{
  // texture is gone
  mId = 0;
  // reset sampler state as well
  mSamplerBitfield = 0;
}

void Texture::GlCleanup()
{
  // delete the gl texture
  if (mId != 0)
  {
    mContext.DeleteTextures(1,&mId);
    mId = 0;
  }
}

unsigned int Texture::GetWidth() const
{
  return mWidth;
}

unsigned int Texture::GetHeight() const
{
  return mHeight;
}

void Texture::ApplyFilterModeParameter( TextureUnit unit, GLint filterType, FilterMode::Type currentFilterMode, FilterMode::Type newFilterMode, GLint daliDefault, GLint systemDefault )
{
  GLint newFilterModeGL = FilterModeToGL( newFilterMode, daliDefault, systemDefault );
  GLint currentFilterModeGL = FilterModeToGL( currentFilterMode, daliDefault, systemDefault );

  if( newFilterModeGL != currentFilterModeGL )
  {
    mContext.ActiveTexture( unit );
    mContext.TexParameteri( GL_TEXTURE_2D, filterType, newFilterModeGL );
  }
}

void Texture::ApplyWrapModeParameter( TextureUnit unit, GLint wrapType, WrapMode::Type currentWrapMode, WrapMode::Type newWrapMode )
{
  GLint newWrapModeGL = WrapModeToGL( newWrapMode, SYSTEM_WRAP_DEFAULT );
  GLint currentWrapModeGL = WrapModeToGL( currentWrapMode, SYSTEM_WRAP_DEFAULT );

  if( newWrapModeGL != currentWrapModeGL )
  {
    mContext.ActiveTexture( unit );
    mContext.TexParameteri( GL_TEXTURE_2D, wrapType, newWrapModeGL );
  }
}

void Texture::ApplySampler( TextureUnit unit, unsigned int samplerBitfield )
{
  if( mSamplerBitfield != samplerBitfield && mId != 0 )
  {
    ApplyFilterModeParameter( unit,
                           GL_TEXTURE_MIN_FILTER,
                           ImageSampler::GetMinifyFilterMode( mSamplerBitfield ),
                           ImageSampler::GetMinifyFilterMode( samplerBitfield ),
                           DALI_MINIFY_DEFAULT,
                           SYSTEM_MINIFY_DEFAULT );

    ApplyFilterModeParameter( unit,
                              GL_TEXTURE_MAG_FILTER,
                              ImageSampler::GetMagnifyFilterMode( mSamplerBitfield ),
                              ImageSampler::GetMagnifyFilterMode( samplerBitfield ),
                              DALI_MAGNIFY_DEFAULT,
                              SYSTEM_MAGNIFY_DEFAULT );

    ApplyWrapModeParameter( unit,
                            GL_TEXTURE_WRAP_S,
                            ImageSampler::GetUWrapMode( mSamplerBitfield ),
                            ImageSampler::GetUWrapMode( samplerBitfield ));

    ApplyWrapModeParameter( unit,
                            GL_TEXTURE_WRAP_T,
                            ImageSampler::GetVWrapMode( mSamplerBitfield ),
                            ImageSampler::GetVWrapMode( samplerBitfield ));



    mSamplerBitfield = samplerBitfield;
  }
}

} // namespace Internal

} // namespace Dali
