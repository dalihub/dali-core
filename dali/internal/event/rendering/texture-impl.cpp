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
#include <dali/internal/event/rendering/texture-impl.h> // Dali::Internal::Texture

// INTERNAL INCLUDES
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/event/common/stage-impl.h>

namespace Dali
{
namespace Internal
{

NewTexturePtr NewTexture::New(TextureType::Type type, Pixel::Format format, unsigned int width, unsigned int height )
{
  NewTexturePtr texture( new NewTexture( type, format, width, height ) );
  texture->Initialize();
  return texture;
}

NewTexturePtr NewTexture::New( NativeImageInterface& nativeImageInterface )
{
  NewTexturePtr texture( new NewTexture( &nativeImageInterface ) );
  texture->Initialize();
  return texture;
}

Render::NewTexture* NewTexture::GetRenderObject() const
{
  return mRenderObject;
}

NewTexture::NewTexture(TextureType::Type type, Pixel::Format format, unsigned int width, unsigned int height )
: mEventThreadServices( *Stage::GetCurrent() ),
  mRenderObject( NULL ),
  mNativeImage(),
  mType( type ),
  mFormat( format ),
  mWidth( width ),
  mHeight( height )
{
}

NewTexture::NewTexture( NativeImageInterfacePtr nativeImageInterface )
: mEventThreadServices( *Stage::GetCurrent() ),
  mRenderObject( NULL ),
  mNativeImage( nativeImageInterface ),
  mType( TextureType::TEXTURE_2D ),
  mFormat( Pixel::RGB888 ),
  mWidth( nativeImageInterface->GetWidth() ),
  mHeight( nativeImageInterface->GetHeight() )
{
}

void NewTexture::Initialize()
{
  if( mNativeImage )
  {
    mRenderObject = new Render::NewTexture( mNativeImage );
  }
  else
  {
    mRenderObject = new Render::NewTexture( mType, mFormat, mWidth, mHeight );
  }

  AddTexture( mEventThreadServices.GetUpdateManager(), *mRenderObject );
}

NewTexture::~NewTexture()
{
  if( EventThreadServices::IsCoreRunning() && mRenderObject )
  {
    RemoveTexture( mEventThreadServices.GetUpdateManager(), *mRenderObject );
  }
}

bool NewTexture::Upload( PixelDataPtr pixelData )
{
  return Upload( pixelData, 0u, 0u, 0u, 0u, mWidth, mHeight );
}

bool NewTexture::Upload( PixelDataPtr pixelData,
                         unsigned int layer, unsigned int mipmap,
                         unsigned int xOffset, unsigned int yOffset,
                         unsigned int width, unsigned int height )
{
  bool result(false);
  if( mNativeImage )
  {
    DALI_LOG_ERROR( "OpenGLES does not support uploading data to native texture");
  }
  else
  {
    unsigned int pixelDataSize = pixelData->GetWidth()*pixelData->GetHeight();
    if( pixelData->GetBuffer() == NULL || pixelDataSize == 0 )
    {
      DALI_LOG_ERROR( "PixelData is empty");
    }
    else
    {
      Pixel::Format pixelDataFormat = pixelData->GetPixelFormat();
      if( ( pixelDataFormat == mFormat ) || ( (pixelDataFormat == Pixel::RGB888 ) && ( mFormat == Pixel::RGBA8888 ) ) )
      {
        if( pixelDataSize < width * height )
        {
          DALI_LOG_ERROR( "Pixel data of an incorrect size when trying to update texture");
        }
        else if( ( xOffset + width  > ( mWidth  / (1<<mipmap) ) ) ||
                 ( yOffset + height > ( mHeight / (1<<mipmap) ) ) )
        {
          DALI_LOG_ERROR( "Texture update area out of bounds");
        }
        else
        {
          //Parameters are correct. Send message to upload data to the texture
          UploadParams params = { layer, mipmap, xOffset, yOffset, width, height };
          UploadTextureMessage(mEventThreadServices.GetUpdateManager(), *mRenderObject, pixelData, params );
          result = true;
        }
      }
      else
      {
        DALI_LOG_ERROR( "Bad format");
      }
    }
  }

  return result;
}

void NewTexture::GenerateMipmaps()
{
  GenerateMipmapsMessage(mEventThreadServices.GetUpdateManager(), *mRenderObject );
}

unsigned int NewTexture::GetWidth() const
{
  return mWidth;
}

unsigned int NewTexture::GetHeight() const
{
  return mHeight;
}

} // namespace Internal
} // namespace Dali
