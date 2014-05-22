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

// CLASS HEADER
#include <dali/internal/event/text/glyph-status/glyph-status.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{
namespace
{
const unsigned int MAX_NUMBER_FONTS_SUPPORTED = 0x7FF; ///< total number of font/style combinations Dali can support
const unsigned int MAX_UNICODE_VALUE = 0x10FFFF; ///< maximum unicode value
const unsigned int MAX_DEAD_TIME = 16384;   ///< restricted to 14 bits for dead time counter

}

GlyphStatus::GlyphStatus()
:mCharacterCode(0),
mFontId( 0 ),
mDeadTime( 0 ),
mLoadState( GLYPH_NOT_LOADED ),
mTextureState(  GLYPH_NOT_UPLOADED_TO_TEXTURE ),
mReferenceCount( 0 )
{
}

GlyphStatus::~GlyphStatus()
{
}

GlyphStatus::GlyphStatus( unsigned int characterCode,
                          FontId fontId,
                          LoadState loadStatus)
 :mCharacterCode( characterCode ),
  mFontId( fontId ),
  mDeadTime( 0 ),
  mLoadState( loadStatus ),
  mTextureState( GLYPH_NOT_UPLOADED_TO_TEXTURE ),
  mReferenceCount( 0 )
{
}

// copy constructor
GlyphStatus::GlyphStatus( const GlyphStatus& rhs)
:mCharacterCode( rhs.mCharacterCode ),
 mFontId( rhs.mFontId ),
 mDeadTime( rhs.mDeadTime ),
 mLoadState( rhs.mLoadState ),
 mTextureState( rhs.mTextureState ),
 mReferenceCount( rhs.mReferenceCount )
{
}

// assignment operator
GlyphStatus& GlyphStatus::operator=( const GlyphStatus& rhs )
{
  mCharacterCode = rhs.mCharacterCode;
  mFontId = rhs.mFontId;
  mDeadTime = rhs.mDeadTime;
  mLoadState = rhs.mLoadState;
  mTextureState = rhs.mTextureState;
  mReferenceCount = rhs.mReferenceCount;
  return *this;
}

unsigned int GlyphStatus::GetCharacterCode() const
{
  return mCharacterCode;
}

unsigned int GlyphStatus::GetUniqueId() const
{
  return GetEncodedValue( mCharacterCode, mFontId);
}

void GlyphStatus::IncreaseRefCount() const
{
  mReferenceCount++;
}

void GlyphStatus::DecreaseRefCount() const
{
  DALI_ASSERT_DEBUG( mReferenceCount!= 0 );

  mReferenceCount--;
}

void GlyphStatus::ResetRefCount() const
{
  mReferenceCount = 0;
}

unsigned int GlyphStatus::GetRefCount() const
{
  return mReferenceCount;
}

GlyphStatus::LoadState GlyphStatus::GetLoadState() const
{
  return mLoadState;
}

bool GlyphStatus::IsLoadRequested() const
{
  return ((mLoadState == GLYPH_LOAD_REQUESTED) || (mLoadState == GLYPH_LOW_QUALITY_LOADED_HIGH_REQUESTED));
}

bool GlyphStatus::IsUploadedToTexture() const
{
  return (mTextureState == GLYPH_UPLOADED_TO_TEXTURE);
}

FontId GlyphStatus::GetFontId() const
{
  return mFontId;
}

void GlyphStatus::SetLoadStatus( GlyphStatus::LoadState loadState ) const
{
  mLoadState = loadState;

}
void GlyphStatus::SetTextureStatus( GlyphStatus::TextureState textureState ) const
{
  mTextureState =  textureState;
}

unsigned int GlyphStatus::GetDeadTime() const
{
  return mDeadTime;
}

void GlyphStatus::SetDeadTime( unsigned int deadTime ) const
{
  DALI_ASSERT_ALWAYS( deadTime < MAX_DEAD_TIME );
  mDeadTime = deadTime;
}

unsigned int GlyphStatus::GetEncodedValue(  unsigned int code, FontId fontId )
{
  DALI_ASSERT_ALWAYS( (fontId < MAX_NUMBER_FONTS_SUPPORTED ) &&  (code < MAX_UNICODE_VALUE) );

  // encoded value is a combination of the character code and font id
  //
  // We can do this because unicode values only go up to 0x10FFFF
  // and the font id is a zero based counter, limited to 2048 fonts.
  //
  // So format is:
  //
  // bit 0                                        bit 31
  // |                   |                          |
  // | font id (11 bits) | character code (21 bits) |
  // |                   |                          |

  unsigned int combined = (code<<11) |  fontId  ;
  return combined;
}

void GlyphStatus::GetDecodedValue( unsigned int encodedValue, unsigned int& code, FontId& fontId )
{
  code = encodedValue >> 11;
  fontId = encodedValue & MAX_NUMBER_FONTS_SUPPORTED;
}

unsigned int GlyphStatus::GetMaximumDeadTime()
{
  return MAX_DEAD_TIME;
}

} // namespace Internal

} // namespace Dali
