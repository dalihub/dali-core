/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
 */

// CLASS HEADER
#include <dali/internal/event/images/pixel-manipulation.h>

// INTERNAL HEADERS
#include <dali/public-api/images/pixel.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

namespace Pixel
{

struct Location
{
  unsigned int bitShift;
  unsigned int bitMask;
  bool available;
};

struct Locations
{
  Location luminance;
  Location alpha;
  Location red;
  Location green;
  Location blue;
};


bool HasChannel( Dali::Pixel::Format pixelFormat, Channel channel )
{
  switch (pixelFormat)
  {
    case Dali::Pixel::A8:
    {
      return (channel == ALPHA);
    }
    case Dali::Pixel::L8:
    {
      return (channel == LUMINANCE);
    }
    case Dali::Pixel::LA88:
    {
      return ( channel == LUMINANCE || channel == ALPHA );
    }
    case Dali::Pixel::RGB565:
    case Dali::Pixel::BGR565:
    case Dali::Pixel::RGB888:
    case Dali::Pixel::RGB8888:
    case Dali::Pixel::BGR8888:
    {
      return ( channel == RED || channel == GREEN || channel == BLUE );
    }

    case Dali::Pixel::RGBA8888:
    case Dali::Pixel::BGRA8888:
    case Dali::Pixel::RGBA4444:
    case Dali::Pixel::BGRA4444:
    case Dali::Pixel::RGBA5551:
    case Dali::Pixel::BGRA5551:
    {
      return ( channel == RED || channel == GREEN || channel == BLUE || channel == ALPHA );
    }

    case Dali::Pixel::INVALID:
    case Dali::Pixel::COMPRESSED_R11_EAC:
    case Dali::Pixel::COMPRESSED_SIGNED_R11_EAC:
    case Dali::Pixel::COMPRESSED_RG11_EAC:
    case Dali::Pixel::COMPRESSED_SIGNED_RG11_EAC:
    case Dali::Pixel::COMPRESSED_RGB8_ETC2:
    case Dali::Pixel::COMPRESSED_SRGB8_ETC2:
    case Dali::Pixel::COMPRESSED_RGB8_ETC1:
    case Dali::Pixel::COMPRESSED_RGB_PVRTC_4BPPV1:
    case Dali::Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case Dali::Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case Dali::Pixel::COMPRESSED_RGBA8_ETC2_EAC:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_4x4_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_5x4_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_5x5_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_6x5_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_6x6_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_8x5_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_8x6_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_8x8_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_10x5_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_10x6_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_10x8_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_10x10_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_12x10_KHR:
    case Dali::Pixel::COMPRESSED_RGBA_ASTC_12x12_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
    case Dali::Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
    {
      DALI_LOG_ERROR("Pixel formats for compressed images are not compatible with simple channels.\n");
      break;
    }
  }

  return false;
}

unsigned int ReadChannel( unsigned char* pixelData,
                          Dali::Pixel::Format pixelFormat,
                          Channel channel )
{
  switch (pixelFormat)
  {
    case Dali::Pixel::A8:
    {
      if( channel == ALPHA )
      {
        return static_cast<unsigned int>(*pixelData);
      }
      else return 0u;
    }
    case Dali::Pixel::L8:
    {
      if( channel == LUMINANCE )
      {
        return static_cast<unsigned int>(*pixelData);
      }
      else return 0u;
    }
    case Dali::Pixel::LA88:
    {
      if( channel == LUMINANCE )
      {
        return static_cast<unsigned int>(*pixelData);
      }
      else if( channel == ALPHA )
      {
        return static_cast<unsigned int>(*(pixelData+1));
      }
      else return 0u;
    }
    case Dali::Pixel::RGB565:
    {
      if( channel == RED )
      {
        return (static_cast<unsigned int>(*pixelData) & 0xF8) >> 3;
      }
      else if( channel == GREEN )
      {
        return ((static_cast<unsigned int>(*pixelData) & 0x07) << 3) |
          ((static_cast<unsigned int>(*(pixelData+1)) & 0xE0) >> 5);
      }
      else if( channel == BLUE )
      {
        return static_cast<unsigned int>(*(pixelData+1)) & 0x1F;
      }
      else return 0u;
    }

    case Dali::Pixel::BGR565:
    {
      if( channel == BLUE )
      {
        return (static_cast<unsigned int>(*pixelData) & 0xF8) >> 3;
      }
      else if( channel == GREEN )
      {
        return ((static_cast<unsigned int>(*pixelData) & 0x07) << 3) |
          ((static_cast<unsigned int>(*(pixelData+1)) & 0xE0) >> 5);
      }
      else if( channel == RED )
      {
        return (static_cast<unsigned int>(*(pixelData+1) & 0x1F) );
      }
      else return 0u;
    }

    case Dali::Pixel::RGB888:
    case Dali::Pixel::RGB8888:
    {
      if( channel == RED )
      {
        return static_cast<unsigned int>(*pixelData);
      }
      else if( channel == GREEN )
      {
        return static_cast<unsigned int>(*(pixelData+1));
      }
      else if( channel == BLUE )
      {
        return static_cast<unsigned int>(*(pixelData+2));
      }
      else return 0u;
    }

    case Dali::Pixel::BGR8888:
    {
      if( channel == BLUE )
      {
        return static_cast<unsigned int>(*pixelData);
      }
      else if( channel == GREEN )
      {
        return static_cast<unsigned int>(*(pixelData+1));
      }
      else if( channel == RED )
      {
        return static_cast<unsigned int>(*(pixelData+2));
      }
      else return 0u;
    }

    case Dali::Pixel::RGBA8888:
    {
      if( channel == RED )
      {
        return static_cast<unsigned int>(*pixelData);
      }
      else if( channel == GREEN )
      {
        return static_cast<unsigned int>(*(pixelData+1));
      }
      else if( channel == BLUE )
      {
        return static_cast<unsigned int>(*(pixelData+2));
      }
      else if( channel == ALPHA )
      {
        return static_cast<unsigned int>(*(pixelData+3));
      }
      else return 0u;
    }

    case Dali::Pixel::BGRA8888:
    {
      if( channel == BLUE )
      {
        return static_cast<unsigned int>(*pixelData);
      }
      else if( channel == GREEN )
      {
        return static_cast<unsigned int>(*(pixelData+1));
      }
      else if( channel == RED )
      {
        return static_cast<unsigned int>(*(pixelData+2));
      }
      else if( channel == ALPHA )
      {
        return static_cast<unsigned int>(*(pixelData+3));
      }
      else return 0u;
    }

    case Dali::Pixel::RGBA4444:
    {
      if( channel == RED )
      {
        return (static_cast<unsigned int>(*pixelData) & 0xF0) >> 4;
      }
      else if( channel == GREEN )
      {
        return (static_cast<unsigned int>(*pixelData) & 0x0F);
      }
      else if( channel == BLUE )
      {
        return (static_cast<unsigned int>(*(pixelData+1)) & 0xF0) >> 4;
      }
      else if( channel == ALPHA )
      {
        return (static_cast<unsigned int>(*(pixelData+1)) & 0x0F);
      }
      else return 0u;
    }

    case Dali::Pixel::BGRA4444:
    {
      if( channel == BLUE )
      {
        return (static_cast<unsigned int>(*pixelData) & 0xF0) >> 4;
      }
      else if( channel == GREEN )
      {
        return (static_cast<unsigned int>(*pixelData) & 0x0F);
      }
      else if( channel == RED )
      {
        return (static_cast<unsigned int>(*(pixelData+1)) & 0xF0) >> 4;
      }
      else if( channel == ALPHA )
      {
        return (static_cast<unsigned int>(*(pixelData+1)) & 0x0F);
      }
      else return 0u;
    }

    case Dali::Pixel::RGBA5551:
    {
      if( channel == RED )
      {
        return (static_cast<unsigned int>(*pixelData) & 0xF8) >> 3;
      }
      else if( channel == GREEN )
      {
        return ((static_cast<unsigned int>(*pixelData) & 0x07) << 2) |
          ((static_cast<unsigned int>(*(pixelData+1)) & 0xC0) >> 6);
      }
      else if( channel == BLUE )
      {
        return (static_cast<unsigned int>(*(pixelData+1)) & 0x3E) >> 1;
      }
      else if( channel == ALPHA )
      {
        return static_cast<unsigned int>(*(pixelData+1)) & 0x01;
      }

      else return 0u;
    }

    case Dali::Pixel::BGRA5551:
    {
      if( channel == BLUE )
      {
        return (static_cast<unsigned int>(*pixelData) & 0xF8) >> 3;
      }
      else if( channel == GREEN )
      {
        return ((static_cast<unsigned int>(*pixelData) & 0x07) << 2) |
          ((static_cast<unsigned int>(*(pixelData+1)) & 0xC0) >> 6);
      }
      else if( channel == RED )
      {
        return ( static_cast<unsigned int>(*(pixelData+1)) & 0x3E) >> 1;
      }
      else if( channel == ALPHA )
      {
        return static_cast<unsigned int>(*(pixelData+1)) & 0x01;
      }

      else return 0u;
    }

    default:
    {
      return 0u;
    }
  }
}

void WriteChannel( unsigned char* pixelData,
                   Dali::Pixel::Format pixelFormat,
                   Channel channel,
                   unsigned int channelValue )
{
  switch (pixelFormat)
  {
    case Dali::Pixel::A8:
    {
      if( channel == ALPHA )
      {
        *pixelData = static_cast<unsigned char>( channelValue & 0xFF );
      }
      break;
    }
    case Dali::Pixel::L8:
    {
      if( channel == LUMINANCE )
      {
        *pixelData = static_cast<unsigned char>( channelValue & 0xFF );
      }
      break;
    }
    case Dali::Pixel::LA88:
    {
      if( channel == LUMINANCE )
      {
        *pixelData = static_cast<unsigned char>( channelValue & 0xFF );
      }
      else if( channel == ALPHA )
      {
        *(pixelData+1) = static_cast<unsigned char>( channelValue & 0xFF );
      }
      break;
    }
    case Dali::Pixel::RGB565:
    {
      if( channel == RED )
      {
        *pixelData &= ~0xF8;
        *pixelData |= static_cast<unsigned char>( (channelValue << 3) & 0xF8 );
      }
      else if( channel == GREEN )
      {
        *pixelData &= ~0x07;
        *pixelData |= static_cast<unsigned char>( (channelValue >> 3) & 0x07 );

        *(pixelData+1) &= ~0xE0;
        *(pixelData+1) |= static_cast<unsigned char>( (channelValue << 5) & 0xE0 );
      }
      else if( channel == BLUE )
      {
        *(pixelData+1) &= ~0x1F;
        *(pixelData+1) |= static_cast<unsigned char>( channelValue & 0x1F );
      }
      break;
    }

    case Dali::Pixel::BGR565:
    {
      if( channel == BLUE )
      {
        *pixelData &= ~0xF8;
        *pixelData |= static_cast<unsigned char>( (channelValue << 3) & 0xF8 );
      }
      else if( channel == GREEN )
      {
        *pixelData &= ~0x07;
        *pixelData |= static_cast<unsigned char>( (channelValue >> 3) & 0x07 );

        *(pixelData+1) &= ~0xE0;
        *(pixelData+1) |= static_cast<unsigned char>( (channelValue << 5) & 0xE0 );
      }
      else if( channel == RED )
      {
        *(pixelData+1) &= ~0x1F;
        *(pixelData+1) |= static_cast<unsigned char>( channelValue & 0x1F );
      }
      break;
    }

    case Dali::Pixel::RGB888:
    case Dali::Pixel::RGB8888:
    {
      if( channel == RED )
      {
        *pixelData = static_cast<unsigned char>( channelValue & 0xFF );
      }
      else if( channel == GREEN )
      {
        *(pixelData+1) = static_cast<unsigned char>( channelValue & 0xFF );
      }
      else if( channel == BLUE )
      {
        *(pixelData+2) = static_cast<unsigned char>( channelValue & 0xFF );
      }
      break;
    }

    case Dali::Pixel::BGR8888:
    {
      if( channel == BLUE )
      {
        *pixelData = static_cast<unsigned char>( channelValue & 0xFF );
      }
      else if( channel == GREEN )
      {
        *(pixelData+1) = static_cast<unsigned char>( channelValue & 0xFF );
      }
      else if( channel == RED )
      {
        *(pixelData+2) = static_cast<unsigned char>( channelValue & 0xFF );
      }
      break;
    }

    case Dali::Pixel::RGBA8888:
    {
      if( channel == RED )
      {
        *pixelData = static_cast<unsigned char>( channelValue & 0xFF );
      }
      else if( channel == GREEN )
      {
        *(pixelData+1) = static_cast<unsigned char>( channelValue & 0xFF );
      }
      else if( channel == BLUE )
      {
        *(pixelData+2) = static_cast<unsigned char>( channelValue & 0xFF );
      }
      else if( channel == ALPHA )
      {
        *(pixelData+3) = static_cast<unsigned char>( channelValue & 0xFF );
      }
      break;
    }

    case Dali::Pixel::BGRA8888:
    {
      if( channel == BLUE )
      {
        *pixelData = static_cast<unsigned char>( channelValue & 0xFF );
      }
      else if( channel == GREEN )
      {
        *(pixelData+1) = static_cast<unsigned char>( channelValue & 0xFF );
      }
      else if( channel == RED )
      {
        *(pixelData+2) = static_cast<unsigned char>( channelValue & 0xFF );
      }
      else if( channel == ALPHA )
      {
        *(pixelData+3) = static_cast<unsigned char>( channelValue & 0xFF );
      }
      break;
    }

    case Dali::Pixel::RGBA4444:
    {
      if( channel == RED )
      {
        *pixelData &= ~0xF0;
        *pixelData |= static_cast<unsigned char>( (channelValue << 4) & 0xF0 );
      }
      else if( channel == GREEN )
      {
        *pixelData &= ~0x0F;
        *pixelData |= static_cast<unsigned char>( channelValue & 0x0F );
      }
      else if( channel == BLUE )
      {
        *(pixelData+1) &= ~0xF0;
        *(pixelData+1) |= static_cast<unsigned char>( (channelValue << 4) & 0xF0 );
      }
      else if( channel == ALPHA )
      {
        *(pixelData+1) &= ~0x0F;
        *(pixelData+1) |= static_cast<unsigned char>( channelValue & 0x0F );
      }
      break;
    }

    case Dali::Pixel::BGRA4444:
    {
      if( channel == BLUE )
      {
        *pixelData &= ~0xF0;
        *pixelData |= static_cast<unsigned char>( (channelValue << 4) & 0xF0 );
      }
      else if( channel == GREEN )
      {
        *pixelData &= ~0x0F;
        *pixelData |= static_cast<unsigned char>( channelValue & 0x0F );
      }
      else if( channel == RED )
      {
        *(pixelData+1) &= ~0xF0;
        *(pixelData+1) |= static_cast<unsigned char>( (channelValue << 4) & 0xF0 );
      }
      else if( channel == ALPHA )
      {
        *(pixelData+1) &= ~0x0F;
        *(pixelData+1) |= static_cast<unsigned char>( channelValue & 0x0F );
      }
      break;
    }

    case Dali::Pixel::RGBA5551:
    {
      // rrrrrggg ggbbbbba
      //    F8  7 C0  3E 1
      if( channel == RED )
      {
        *pixelData &= ~0xF8;
        *pixelData |= static_cast<unsigned char>( (channelValue << 3) & 0xF8 );
      }
      else if( channel == GREEN )
      {
        *pixelData &= ~0x07;
        *pixelData |= static_cast<unsigned char>( (channelValue >> 2) & 0x07 );

        *(pixelData+1) &= ~0xC0;
        *(pixelData+1) |= static_cast<unsigned char>( (channelValue << 6) & 0xC0 );
      }
      else if( channel == BLUE )
      {
        *(pixelData+1) &= ~0x3E;
        *(pixelData+1) |= static_cast<unsigned char>( (channelValue << 1) & 0x3E );
      }
      else if( channel == ALPHA )
      {
        *(pixelData+1) &= ~0x01;
        *(pixelData+1) |= static_cast<unsigned char>( channelValue & 0x01 );
      }
      break;
    }

    case Dali::Pixel::BGRA5551:
    {
      if( channel == BLUE )
      {
        *pixelData &= ~0xF8;
        *pixelData |= static_cast<unsigned char>( (channelValue << 3) & 0xF8 );
      }
      else if( channel == GREEN )
      {
        *pixelData &= ~0x07;
        *pixelData |= static_cast<unsigned char>( (channelValue >> 2) & 0x07 );

        *(pixelData+1) &= ~0xC0;
        *(pixelData+1) |= static_cast<unsigned char>( (channelValue << 6) & 0xC0 );
      }
      else if( channel == RED )
      {
        *(pixelData+1) &= ~0x3E;
        *(pixelData+1) |= static_cast<unsigned char>( (channelValue << 1 ) & 0x3E );
      }
      else if( channel == ALPHA )
      {
        *(pixelData+1) &= ~0x01;
        *(pixelData+1) |= static_cast<unsigned char>( channelValue & 0x01 );
      }
      break;
    }

    default:
      break;
  }
}

void ConvertColorChannelsToRGBA8888(
  unsigned char* srcPixel,  int srcOffset,  Dali::Pixel::Format srcFormat,
  unsigned char* destPixel, int destOffset )
{
  int red   = ReadChannel(srcPixel+srcOffset, srcFormat, RED );
  int green = ReadChannel(srcPixel+srcOffset, srcFormat, GREEN );
  int blue  = ReadChannel(srcPixel+srcOffset, srcFormat, BLUE );
  switch( srcFormat )
  {
    case Dali::Pixel::RGB565:
    case Dali::Pixel::BGR565:
    {
      red<<=3;
      green<<=2;
      blue<<=3;
      break;
    }
    case Dali::Pixel::RGBA4444:
    case Dali::Pixel::BGRA4444:
    {
      red<<=4;
      green<<=4;
      blue<<=4;
      break;
    }
    case Dali::Pixel::RGBA5551:
    case Dali::Pixel::BGRA5551:
    {
      red<<=3;
      green<<=3;
      blue<<=3;
      break;
    }
    default:
      break;
  }
  WriteChannel(destPixel+destOffset, Dali::Pixel::RGBA8888, RED, red);
  WriteChannel(destPixel+destOffset, Dali::Pixel::RGBA8888, GREEN, green);
  WriteChannel(destPixel+destOffset, Dali::Pixel::RGBA8888, BLUE, blue);
}



} // Pixel
} // Internal
} // Dali
