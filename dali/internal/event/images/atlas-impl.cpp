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
#include <dali/internal/event/images/atlas-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for memset()

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/images/image-factory.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/integration-api/bitmap.h>
#include <dali/integration-api/platform-abstraction.h>


namespace Dali
{

namespace Internal
{

namespace
{
TypeRegistration mType( typeid( Dali::Atlas ), typeid( Dali::Image ), NULL );
}

Atlas* Atlas::New( SizeType width,
                   SizeType height,
                   Pixel::Format pixelFormat,
                   bool recoverContext)
{
  return new Atlas( width, height, pixelFormat, recoverContext );
}

void Atlas::Clear(const Vector4& color)
{
  ClearCache();
  ClearBackground( color );
}

bool Atlas::Upload( BufferImage& bufferImage,
                    SizeType xOffset,
                    SizeType yOffset )
{
  bool uploadSuccess( false );

  if( Compatible(bufferImage.GetPixelFormat(),
                 xOffset + bufferImage.GetWidth(),
                 yOffset + bufferImage.GetHeight() ) )
  {
    AllocateAtlas();
    ResourceId destId = GetResourceId();
    ResourceId srcId = bufferImage.GetResourceId();

    if( destId && srcId )
    {
      mResourceClient.UploadBitmap( destId, srcId, xOffset, yOffset );
      uploadSuccess = true;
    }
  }

  return uploadSuccess;
}

bool Atlas::Upload( const std::string& url,
                    SizeType xOffset,
                    SizeType yOffset)
{
  bool uploadSuccess( false );

  Integration::BitmapPtr bitmap = LoadBitmap( url );

  if( bitmap && Compatible(bitmap->GetPixelFormat(), xOffset + bitmap->GetImageWidth(), yOffset + bitmap->GetImageHeight()) )
  {
    AllocateAtlas();
    ResourceId destId = GetResourceId();
    if( destId )
    {
      mResourceClient.UploadBitmap( destId, bitmap, xOffset, yOffset  );
      uploadSuccess = true;

      if( mRecoverContext )
      {
        mTiles.PushBack( new Tile(xOffset, yOffset, url) );
      }
    }
  }
  return uploadSuccess;
}

void Atlas::RecoverFromContextLoss()
{
  ResourceId destId = GetResourceId();
  if( destId )
  {
    if( mClear )
    {
      ClearBackground( mClearColor );
    }

    if( mRecoverContext )
    {
      // Restore the atlas by re-uploading the url resources
      Vector< Tile* >::ConstIterator end = mTiles.End();
      for( Vector<Tile*>::Iterator iter = mTiles.Begin(); iter != end; iter++ )
      {
        Integration::BitmapPtr bitmap = LoadBitmap( (*iter)->url );
        mResourceClient.UploadBitmap( destId, bitmap, (*iter)->xOffset, (*iter)->yOffset  );
      }
    }
  }
}

Atlas::~Atlas()
{
  ReleaseAtlas();
}

Atlas::Atlas( SizeType width,
              SizeType height,
              Pixel::Format pixelFormat,
              bool recoverContext )
: mResourceClient( ThreadLocalStorage::Get().GetResourceClient() ),
  mImageFactory( ThreadLocalStorage::Get().GetImageFactory() ),
  mClearColor( Vector4::ZERO ),
  mPixelFormat( pixelFormat ),
  mClear( false ),
  mRecoverContext( recoverContext )
{
  mWidth  = width;
  mHeight = height;
}

void Atlas::Connect()
{
  ++mConnectionCount;

  if( mConnectionCount == 1 )
  {
    AllocateAtlas();
  }
}

void Atlas::Disconnect()
{
  if( mConnectionCount )
  {
    --mConnectionCount;

    if ( Dali::Image::UNUSED == mReleasePolicy &&
         mConnectionCount == 0 )
    {
      ReleaseAtlas();
    }
  }
}

bool Atlas::Compatible( Pixel::Format pixelFormat,
                        SizeType x,
                        SizeType y )
{
  bool Compatible(false);

  if( mPixelFormat != pixelFormat )
  {
    DALI_LOG_ERROR( "Pixel format %d does not match Atlas format %d\n", pixelFormat, mPixelFormat );
  }
  else
  {
    if( x <= mWidth  && y <= mHeight )
    {
      Compatible = true;
    }
    else
    {
      DALI_LOG_ERROR( "image does not fit within the atlas \n" );
    }
  }

  return Compatible;
}

void Atlas::AllocateAtlas()
{
  if( !mTicket )
  {
    mTicket = mResourceClient.AllocateTexture( mWidth, mHeight, mPixelFormat );
    mTicket->AddObserver( *this );
    mImageFactory.RegisterForContextRecovery( this );
  }
}

void Atlas::ReleaseAtlas()
{
  mTicket.Reset();
  ClearCache();
  mImageFactory.UnregisterFromContextRecovery( this );
}

void Atlas::ClearBackground(const Vector4& color )
{
  AllocateAtlas();
  ResourceId destId = GetResourceId();
  if( destId )
  {
    const unsigned int numPixels = mWidth * mHeight;
    unsigned int bytesPerPixel = Pixel::GetBytesPerPixel(mPixelFormat);
    BufferImagePtr imageData = BufferImage::New( mWidth, mHeight, mPixelFormat );
    PixelBuffer* pixbuf = imageData->GetBuffer();

    // converting color value from float 0.f~1.f to byte 0~255
    unsigned char r =  static_cast<unsigned char>( 255.f * Clamp( color.r, 0.f, 1.f ) );
    unsigned char g =  static_cast<unsigned char>( 255.f * Clamp( color.g, 0.f, 1.f ) );
    unsigned char b =  static_cast<unsigned char>( 255.f * Clamp( color.b, 0.f, 1.f ) );
    unsigned char a =  static_cast<unsigned char>( 255.f * Clamp( color.a, 0.f, 1.f ) );
    if( mPixelFormat == Pixel::RGBA8888 )
    {
      // For little-endian byte order, the RGBA channels needs to be reversed for bit shifting.
      uint32_t clearColor = ( (uint32_t) a<<24 | (uint32_t)b << 16 | (uint32_t)g << 8 | (uint32_t)r );
      uint32_t* buf = (uint32_t *) pixbuf;
      for( unsigned int i = 0; i < numPixels; ++i )
      {
        buf[i] = clearColor;
      }
    }
    else if( mPixelFormat == Pixel::RGB888 )
    {
      for( unsigned int i = 0; i < numPixels; ++i )
      {
        pixbuf[i*bytesPerPixel] = r;
        pixbuf[i*bytesPerPixel+1] = g;
        pixbuf[i*bytesPerPixel+2] = b;
      }
    }
    else if( mPixelFormat == Pixel::A8 )
    {
      memset( pixbuf, a, numPixels );
    }

    RectArea area;
    imageData->Update(area);

    mClearColor = color;
    mClear = true;
    mResourceClient.UploadBitmap( destId, imageData->GetResourceId(), 0, 0 );
  }
}

void Atlas::ClearCache()
{
  Vector< Tile* >::ConstIterator end = mTiles.End();
  for( Vector<Tile*>::Iterator iter = mTiles.Begin(); iter != end; iter++ )
  {
    delete *iter;
  }
  mTiles.Clear();
}

Integration::BitmapPtr Atlas::LoadBitmap( const std::string& url )
{
  Integration::BitmapResourceType resourceType;
  Integration::PlatformAbstraction& platformAbstraction = Internal::ThreadLocalStorage::Get().GetPlatformAbstraction();

  Integration::ResourcePointer resource = platformAbstraction.LoadResourceSynchronously(resourceType, url);
  Integration::BitmapPtr bitmap = static_cast<Integration::Bitmap*>( resource.Get());

  return bitmap;
}

} // namespace Internal

} // namespace Dali
