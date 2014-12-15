#ifndef __DALI_INTERNAL_BITMAP_UPLOAD_H__
#define __DALI_INTERNAL_BITMAP_UPLOAD_H__

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
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{

namespace Internal
{

/**
 * Holds a bitmap and a x,y pixel position of where the bitmap
 * should be uploaded to in a texture.
 *
 */
struct BitmapUpload
{

  enum DiscardMode
  {
    DISCARD_PIXEL_DATA,       ///< Delete the pixel data after upload
    DONT_DISCARD_PIXEL_DATA  ///< Don't delete the pixel data after upload
  };

  typedef unsigned char      PixelData;  ///< pixel data type

  /**
   * Constructor
   * @param data pixel data
   * @param xPos x position of where to place the bitmap in a texture
   * @param yPos y position of where to place the bitmap in a texture
   * @param width bitmap width in pixels
   * @param height bitmap height in pixels
   * @param discardMode whether to delete the pixel data after the upload or not
   */
  BitmapUpload(PixelData*   data,
               unsigned int xPos,
               unsigned int yPos,
               unsigned int width,
               unsigned int height,
               DiscardMode discardMode = DISCARD_PIXEL_DATA)
  :mPixelData(data),
   mXpos(xPos),
   mYpos(yPos),
   mWidth(width),
   mHeight(height),
   mDiscard(discardMode)
  {
  }


  PixelData*   mPixelData;         ///< bitmap data to upload
  unsigned int mXpos;              ///< x position to place the bitmap in the texture
  unsigned int mYpos;              ///< y position to place the bitmap in the texture
  unsigned int mWidth;             ///< width of the bitmap to upload
  unsigned int mHeight;            ///< height of the bitmap to upload
  DiscardMode  mDiscard;           ///< whether the pixel data should be discarded after the upload
};

typedef std::vector<BitmapUpload> BitmapUploadArray; ///< typedef for a array of uploads


/**
 * Structure is used for clearing areas of the bitmap
 */
typedef std::vector<Vector2> BitmapClearArray;

}  //namespace Internal

} //namespace Dali

#endif
