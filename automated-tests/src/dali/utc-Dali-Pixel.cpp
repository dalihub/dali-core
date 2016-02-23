/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <iostream>
#include <algorithm>

#include <stdlib.h>
#include <dali/public-api/dali-core.h>
#include <dali-test-suite-utils.h>

using std::max;
using namespace Dali;


static const float ROTATION_EPSILON = 0.0001f;

void utc_dali_pixel_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_pixel_cleanup(void)
{
  test_return_value = TET_PASS;
}

static void TestPixelEnumSize(const int size)
{
  DALI_TEST_CHECK( (static_cast<int>( Pixel::LAST_VALID_PIXEL_FORMAT ) - static_cast<int>( Pixel::FIRST_VALID_PIXEL_FORMAT ) + 1 ) == size &&
      "The Pixel::Format enum may have had new formats added. Expand the test cases to include them.");
}

int UtcDaliPixelHasAlpha(void)
{
  TestApplication application;

  tet_infoline("UtcDaliPixelHasAlpha");

  TestPixelEnumSize( 54 );

  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::INVALID ) == false ); // For completeness

  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::L8 ) == false );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::RGB565 ) == false );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::RGB888 ) == false );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::RGB8888 ) == false );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::BGR8888 ) == false );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::BGR565 ) == false );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_R11_EAC ) == false );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_SIGNED_R11_EAC ) == false );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RG11_EAC ) == false );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_SIGNED_RG11_EAC ) == false );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RGB8_ETC2 ) == false );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_SRGB8_ETC2 ) == false );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RGB8_ETC1 ) == false );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RGB_PVRTC_4BPPV1 ) == false );

  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::A8 ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::LA88 ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::RGBA5551 ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::RGBA4444 ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::RGBA8888 ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::BGRA8888 ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::BGRA5551 ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::BGRA4444 ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RGBA8_ETC2_EAC ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC ) == true );

  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RGBA_ASTC_4x4_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RGBA_ASTC_5x4_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RGBA_ASTC_5x5_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RGBA_ASTC_6x5_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RGBA_ASTC_6x6_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RGBA_ASTC_8x5_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RGBA_ASTC_8x6_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RGBA_ASTC_8x8_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RGBA_ASTC_10x5_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RGBA_ASTC_10x6_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RGBA_ASTC_10x8_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RGBA_ASTC_10x10_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RGBA_ASTC_12x10_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_RGBA_ASTC_12x12_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR ) == true );
  DALI_TEST_CHECK( Pixel::HasAlpha( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR ) == true );

  END_TEST;
}

int UtcDaliPixelHasAlphaN(void)
{
  DALI_TEST_EQUALS( Pixel::HasAlpha( Pixel::Format( 123123123123 ) ), false, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPixelGetBytesPerPixel(void)
{
  TestApplication application;

  tet_infoline("UtcDaliPixelGetBytesPerPixel");

  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::INVALID ) == 0 ); // For completeness

  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::L8 ) == 1 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::A8 ) == 1 );

  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::LA88 ) == 2 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::RGB565 ) == 2 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::RGBA5551 ) == 2 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::RGBA4444 ) == 2 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::BGR565 ) == 2 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::BGRA5551 ) == 2 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::BGRA4444 ) == 2 );

  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::RGB888 ) == 3 );

  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::RGB8888 ) == 4 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::BGR8888 ) == 4 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::RGBA8888 ) == 4 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::BGRA8888 ) == 4 );

  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::L8 ) == 1 );

  // Compressed formats are marked as 0 bytes per pixel.
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_R11_EAC ) == 0);
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_SIGNED_R11_EAC ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RG11_EAC ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_SIGNED_RG11_EAC ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RGB8_ETC2 ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_SRGB8_ETC2 ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RGB8_ETC1 ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RGB_PVRTC_4BPPV1 ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RGBA8_ETC2_EAC ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RGBA_ASTC_4x4_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RGBA_ASTC_5x4_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RGBA_ASTC_5x5_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RGBA_ASTC_6x5_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RGBA_ASTC_6x6_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RGBA_ASTC_8x5_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RGBA_ASTC_8x6_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RGBA_ASTC_8x8_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RGBA_ASTC_10x5_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RGBA_ASTC_10x6_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RGBA_ASTC_10x8_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RGBA_ASTC_10x10_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RGBA_ASTC_12x10_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_RGBA_ASTC_12x12_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR ) == 0 );
  DALI_TEST_CHECK( Pixel::GetBytesPerPixel( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR ) == 0 );

  END_TEST;
}

int UtcDaliPixelGetBytesPerPixelN(void)
{
  DALI_TEST_EQUALS( Pixel::GetBytesPerPixel( Pixel::Format( 123123123123 ) ), 0u, TEST_LOCATION );
  END_TEST;
}

int UtcDaliPixelGetAlphaOffsetAndMaskP(void)
{
  TestApplication application;

  tet_infoline("UtcDaliPixelGetAlphaOffsetAndMask");

  int byteOffset = 0;
  int bitMask = 0;

  // Be sure that the number of cases tested below is correct:
  TestPixelEnumSize( 54 );

  Pixel::GetAlphaOffsetAndMask( Pixel::INVALID, byteOffset, bitMask ); // For completeness
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );

  Pixel::GetAlphaOffsetAndMask( Pixel::L8, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::A8, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0xff );
  Pixel::GetAlphaOffsetAndMask( Pixel::RGB888, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::RGB565, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::RGB8888, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::BGR8888, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::LA88, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 1 && bitMask == 0xff );
  Pixel::GetAlphaOffsetAndMask( Pixel::RGBA4444, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 1 && bitMask == 0x0f );
  Pixel::GetAlphaOffsetAndMask( Pixel::RGBA5551, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 1 && bitMask == 0x01 );
  Pixel::GetAlphaOffsetAndMask( Pixel::RGBA8888, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 3 && bitMask == 0xff );
  Pixel::GetAlphaOffsetAndMask( Pixel::BGRA8888, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 3 && bitMask == 0xff );
  Pixel::GetAlphaOffsetAndMask( Pixel::BGR565, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::BGRA4444, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 1 && bitMask == 0x0f );
  Pixel::GetAlphaOffsetAndMask( Pixel::BGRA5551, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 1 && bitMask == 0x01 );

  // Compressed formats with no meaningful result:
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_R11_EAC, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_SIGNED_R11_EAC, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RG11_EAC, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_SIGNED_RG11_EAC, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RGB8_ETC2, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_SRGB8_ETC2, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RGB8_ETC1, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RGB_PVRTC_4BPPV1, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RGBA8_ETC2_EAC, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_4x4_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_5x4_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_5x5_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_6x5_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_6x6_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_8x5_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_8x6_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_8x8_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_10x5_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_10x6_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_10x8_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_10x10_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_12x10_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_RGBA_ASTC_12x12_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );
  Pixel::GetAlphaOffsetAndMask( Pixel::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR, byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 0 && bitMask == 0 );

  END_TEST;
}

int UtcDaliPixelGetAlphaOffsetAndMaskN(void)
{
  int byteOffset = 200;
  int bitMask = 200;
  Pixel::GetAlphaOffsetAndMask( Pixel::Format( 123123123123 ), byteOffset, bitMask );
  DALI_TEST_CHECK( byteOffset == 200 );
  DALI_TEST_CHECK( bitMask == 200 );
  END_TEST;
}
