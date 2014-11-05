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
#include <dali/public-api/images/glyph-image.h>

// INTERNAL INCLUDES
#include <dali/public-api/images/bitmap-image.h>
#include <dali/public-api/text/text-style.h>
#include <dali/integration-api/platform-abstraction.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/images/image-impl.h>
#include <dali/internal/event/images/emoji-factory.h>
#include <dali/internal/event/text/character-impl.h>

namespace Dali
{

GlyphImage::GlyphImage()
{
}

GlyphImage::GlyphImage( Internal::Image* internal )
: Image( internal )
{
}

GlyphImage::~GlyphImage()
{
}

GlyphImage::GlyphImage(const GlyphImage& handle)
: Image(handle)
{
}

GlyphImage& GlyphImage::operator=(const GlyphImage& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

GlyphImage GlyphImage::New( const Character& character )
{
  // Retrieves the font family name for the given character.
  const std::string fontFamilyName = Font::GetFamilyForText( character );

  // Create a text style and set the font family name.
  TextStyle style;
  style.SetFontName( fontFamilyName );

  return GlyphImage::New( character, style );
}

GlyphImage GlyphImage::New( const Character& character, const TextStyle& style )
{
  Image image;

  //  Retrieves the font family name for the given character if is not defined in the text style.
  std::string fontFamilyName = style.GetFontName();
  if( fontFamilyName.empty() )
  {
    fontFamilyName = Font::GetFamilyForText( character );
  }

  // Creates a font with the parameters given in the text style.
  const Font font = Font::New( FontParameters( fontFamilyName, style.GetFontStyle(), style.GetFontPointSize() ) );

  if( GlyphImage::IsColorGlyph( character ) )
  {
    // Work around. Create images from .png files in the file system.

    // Get the glyph metrics.
    const Font::Metrics metrics = font.GetMetrics( character );

    // Set the image size.
    ImageAttributes attributes;
    attributes.SetSize( metrics.GetWidth(), metrics.GetHeight() );

    image = Image::New( DALI_EMOTICON_DIR + Internal::ThreadLocalStorage::Get().GetEmojiFactory().GetEmojiFileNameFromCharacter( character.GetImplementation().GetCharacter() ), attributes );
  }
  else
  {
    // Retrieves the alpha bitmap from the font.

    Integration::PlatformAbstraction& platform = Internal::ThreadLocalStorage::Get().GetPlatformAbstraction();

    Integration::BitmapPtr bitmapPtr = platform.GetGlyphImage( font.GetName(),
                                                               font.GetStyle(),
                                                               font.GetPointSize(),
                                                               character.GetImplementation().GetCharacter() );

    if( bitmapPtr )
    {
      // Create a color bitmap with the alpha bitmap retrieved from the platform and the color specified
      // in the text style.
      //
      // TODO: support more text decoration features: outline, glow, shadow, underline, ...
      const std::size_t width = bitmapPtr->GetImageWidth();
      const std::size_t height = bitmapPtr->GetImageHeight();
      BitmapImage bitmapImage = BitmapImage::New( width, height, Pixel::RGBA8888 );

      // Point the source bitmap buffer.
      unsigned char* src = bitmapPtr->GetBuffer();

      // Point the destination image buffer.
      unsigned char* dst = bitmapImage.GetBuffer();

      //Retrieve the color from the text style.
      const Vector4& color = style.GetTextColor();

      std::size_t srcOffset = 0;
      std::size_t dstOffset = 0;
      for( std::size_t y = 0; y < height; ++y )
      {
        for( std::size_t x = 0; x < width; ++x )
        {
          const float srcAlpha = static_cast<float>( src[ srcOffset ] );

          dst[ dstOffset + 0 ] = static_cast<unsigned char>( srcAlpha * color.r );
          dst[ dstOffset + 1 ] = static_cast<unsigned char>( srcAlpha * color.g );
          dst[ dstOffset + 2 ] = static_cast<unsigned char>( srcAlpha * color.b );
          dst[ dstOffset + 3 ] = static_cast<unsigned char>( srcAlpha * color.a );
          ++srcOffset;
          dstOffset += 4u;
        }
      }

      bitmapImage.Update();

      image = bitmapImage;
    }
  }

  return GlyphImage( static_cast<Internal::Image*>( image.GetObjectPtr() ) );
}

GlyphImage GlyphImage::DownCast( BaseHandle handle )
{
  return GlyphImage( dynamic_cast<Dali::Internal::Image*>( handle.GetObjectPtr() ) );
}

bool GlyphImage::IsColorGlyph( const Character& character )
{
  return Internal::ThreadLocalStorage::Get().GetEmojiFactory().IsEmoji( character.GetImplementation().GetCharacter() );
}

} // namespace Dali
