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

// CLASS HEADER
#include <dali/internal/render/gl-resources/compressed-bitmap-texture.h>

// EXTERNAL INCLUDES
#include <math.h>
#include <memory.h>

// INTERNAL INCLUDES
#include <dali/public-api/math/rect.h>
#include <dali/public-api/math/math-utils.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/render/common/vertex.h>
#include <dali/internal/render/common/performance-monitor.h>
#include <dali/internal/render/gl-resources/context.h>

namespace Dali
{

namespace Internal
{

CompressedBitmapTexture::CompressedBitmapTexture(Internal::BitmapCompressed* const bitmap, Context& context)
: Texture(context,
          bitmap->GetImageWidth(),
          bitmap->GetImageHeight(),
          bitmap->GetImageWidth(),
          bitmap->GetImageHeight(),
          bitmap->GetPixelFormat()),
  mBitmap(bitmap)
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);
  DALI_LOG_SET_OBJECT_STRING(this, DALI_LOG_GET_OBJECT_STRING(bitmap));
}

CompressedBitmapTexture::~CompressedBitmapTexture()
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);
  // GlCleanup() should already have been called by TextureCache ensuring the resource is destroyed
  // on the render thread. (And avoiding a potentially problematic virtual call in the destructor)
}

bool CompressedBitmapTexture::HasAlphaChannel() const
{
  return Pixel::HasAlpha(mPixelFormat);
}

bool CompressedBitmapTexture::IsFullyOpaque() const
{
  if( mBitmap )
  {
    return mBitmap->IsFullyOpaque();
  }
  else
  {
    return ! HasAlphaChannel(); // Todo: amalgamate updated bitmap's IsFullyOpaque()
  }
}


void CompressedBitmapTexture::AssignBitmap( bool generateTexture, const unsigned char* const pixels, const size_t bufferSize )
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::Verbose, "CompressedBitmapTexture::AssignBitmap()\n");

  if( generateTexture )
  {
    mContext.GenTextures(1, &mId);
  }
  DALI_ASSERT_DEBUG( mId != 0 );

  mContext.ActiveTexture(GL_TEXTURE7); // bind in unused unit so rebind works the first time
  mContext.Bind2dTexture(mId);

  GLenum pixelFormat = GL_RGBA;
  GLenum pixelDataType = GL_UNSIGNED_BYTE;
  Integration::ConvertToGlFormat(mPixelFormat, pixelDataType, pixelFormat);

  mContext.PixelStorei(GL_UNPACK_ALIGNMENT, 1); // We always use tightly packed data
  mContext.CompressedTexImage2D(GL_TEXTURE_2D, 0, pixelFormat, mWidth, mHeight, 0, bufferSize, pixels);
  mContext.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  mContext.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  mContext.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  mContext.TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  INCREASE_BY( PerformanceMonitor::TEXTURE_DATA_UPLOADED, bufferSize );
}

void CompressedBitmapTexture::Update( Integration::Bitmap* bitmap )
{
  DALI_ASSERT_DEBUG(bitmap);
  DALI_ASSERT_DEBUG(mImageWidth == mWidth && mImageHeight == mHeight);
  DALI_LOG_INFO(Debug::Filter::gGLResource, Debug::General, "CompressedBitmapTexture::Update(bitmap:%p )\n", bitmap);

  if( !bitmap )
  {
    DALI_LOG_ERROR( "Passed a null bitmap to update this compressed bitmap texture." );
    return;
  }

  Internal::BitmapCompressed * const compressedBitmap = dynamic_cast<Dali::Internal::BitmapCompressed*>( bitmap );
  if( compressedBitmap == 0 )
  {
    DALI_LOG_ERROR("CompressedBitmapTexture was passed a non-compressed bitmap to update with.\n");
    return;
  }
  mBitmap = compressedBitmap;

  const unsigned char * const pixels = mBitmap->GetBuffer();

  DALI_ASSERT_DEBUG(pixels != NULL);

  if ( NULL == pixels )
  {
    DALI_LOG_ERROR("Bitmap has no data\n");
  }
  else
  {
    mImageWidth  = mBitmap->GetImageWidth();
    mImageHeight = mBitmap->GetImageHeight();
    mWidth       = mImageWidth;
    mHeight      = mImageHeight;
    mPixelFormat = mBitmap->GetPixelFormat();

    if ( mId ) // If the texture is already bound
    {
      AssignBitmap( false, pixels, mBitmap->GetBufferSize() );
      mBitmap->DiscardBuffer();
    }
  }
}

bool CompressedBitmapTexture::UpdateOnCreate()
{
  return true;
}

bool CompressedBitmapTexture::CreateGlTexture()
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);
  DALI_LOG_INFO(Debug::Filter::gImage, Debug::Verbose, "Bitmap: %s\n", DALI_LOG_GET_OBJECT_C_STR(this));

  if( mBitmap )
  {
    const unsigned char* pixels = mBitmap->GetBuffer();

    DALI_ASSERT_DEBUG(pixels != NULL);

    if( NULL != pixels )
    {
      AssignBitmap( true, pixels, mBitmap->GetBufferSize() );
      mBitmap->DiscardBuffer();
    }
  }
  else
  {
    AssignBitmap( true, NULL, 0 );
  }

  return mId != 0;
}

bool CompressedBitmapTexture::Init()
{
  DALI_LOG_TRACE_METHOD(Debug::Filter::gImage);
  // mBitmap should be initialized by now
  return (mBitmap != 0);
}

unsigned int CompressedBitmapTexture::GetWidth() const
{
  unsigned int width = mWidth;
  if( mBitmap )
  {
    width = mBitmap->GetImageWidth();
  }
  return width;
}

unsigned int CompressedBitmapTexture::GetHeight() const
{
  unsigned int height = mHeight;
  if( mBitmap )
  {
    height = mBitmap->GetImageHeight();
  }
  return height;
}

} //namespace Internal

} //namespace Dali
