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


Render::NewTexture* NewTexture::GetRenderObject() const
{
  return mRenderObject;
}

NewTexture::NewTexture(TextureType::Type type, Pixel::Format format, unsigned int width, unsigned int height )
: mEventThreadServices( *Stage::GetCurrent() ),
  mRenderObject( NULL ),
  mType( type ),
  mFormat( format ),
  mWidth( width ),
  mHeight( height )
{
}

void NewTexture::Initialize()
{
  mRenderObject = new Render::NewTexture( mType, mFormat, mWidth, mHeight );
  AddTexture( mEventThreadServices.GetUpdateManager(), *mRenderObject );
}

NewTexture::~NewTexture()
{
  if( EventThreadServices::IsCoreRunning() && mRenderObject )
  {
    RemoveTexture( mEventThreadServices.GetUpdateManager(), *mRenderObject );
  }
}

void NewTexture::Upload( Vector<unsigned char>& buffer, const TextureUploadParams& params )
{
  UploadTextureMessage(mEventThreadServices.GetUpdateManager(), *mRenderObject, buffer, params );
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
