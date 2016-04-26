#ifndef __DALI_PIXEL_DATA_H__
#define __DALI_PIXEL_DATA_H__

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

#include <dali/public-api/images/pixel.h>
#include <dali/public-api/object/ref-object.h>

namespace Dali
{

class PixelData;
typedef IntrusivePtr<PixelData> PixelDataPtr;

/**
 * @brief Reference counted pixel data .
 *
 * The PixelData takes over the ownership of the pixel buffer.
 * The buffer memory must NOT be released outside of this class, instead, the PixelData object will release it automatically when the reference count falls to zero.
 */
class DALI_IMPORT_API PixelData : public RefObject
{
public:

  enum ReleaseFunction
  {
    FREE,          ///< Use free function to release the pixel buffer
    DELETE_ARRAY,  ///< Use delete[] operator to release the pixel buffer
  };

  /**
   * @brief Create a PixelData object.
   *
   * @param [in] buffer           The raw pixel data.
   * @param [in] width            Buffer width in pixels
   * @param [in] height           Buffer height in pixels
   * @param [in] pixelFormat      The pixel format
   * @param [in] releaseFunction  The function used to release the memory.
   */
  static PixelDataPtr New( unsigned char* buffer,
                           unsigned int width,
                           unsigned int height,
                           Pixel::Format pixelFormat,
                           ReleaseFunction releaseFunction);

  /**
   * Get the width of the buffer in pixels.
   * @return The width of the buffer in pixels
   */
  unsigned int GetWidth() const;

  /**
   * Get the height of the buffer in pixels
   * @return The height of the buffer in pixels
   */
  unsigned int GetHeight() const;

  /**
   * Get the pixel format
   * @return The pixel format
   */
  Pixel::Format GetPixelFormat() const;

  /**
   * Get the pixel buffer if it's present.
   * @return The buffer if exits, or NULL if there is no pixel buffer.
   */
  unsigned char* GetBuffer() const;

public:

  /**
   * @brief Constructor.
   *
   * @param [in] buffer           The raw pixel data.
   * @param [in] width            Buffer width in pixels
   * @param [in] height           Buffer height in pixels
   * @param [in] pixelFormat      The pixel format
   * @param [in] releaseFunction  The function used to release the memory.
   */
  PixelData( unsigned char* buffer,
             unsigned int width,
             unsigned int height,
             Pixel::Format pixelFormat,
             ReleaseFunction releaseFunction );

  /**
   * @brief Destructor.
   *
   * Release the pixel buffer if exists.
   */
  ~PixelData();


private:

  /*
   * Undefined copy constructor.
   */
  PixelData(const PixelData& other);

  /*
   * Undefined assignment operator.
   */
  PixelData& operator = (const PixelData& other);

private:

  unsigned char* mBuffer;           ///< The raw pixel data.
  unsigned int   mWidth;            ///< Buffer width in pixels.
  unsigned int   mHeight;           ///< Buffer height in pixels.
  Pixel::Format  mPixelFormat;      ///< Pixel format
  ReleaseFunction mReleaseFunction;  ///< Function for releasing memory
};


} //namespace Dali

#endif // __DALI_PIXEL_DATA_H__
