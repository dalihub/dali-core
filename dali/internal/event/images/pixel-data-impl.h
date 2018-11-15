#ifndef DALI_INTERNAL_PIXEL_DATA_H
#define DALI_INTERNAL_PIXEL_DATA_H

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/images/pixel-data.h>
#include <dali/public-api/object/base-object.h>
#include <dali/devel-api/images/pixel-data-devel.h>

namespace Dali
{

namespace Internal
{

class PixelData;
typedef IntrusivePtr<PixelData> PixelDataPtr;

class PixelData : public BaseObject
{
public:

  /**
   * @brief Create a PixelData object.
   *
   * @param [in] buffer           The raw pixel data.
   * @param [in] bufferSize       The size of the buffer in bytes
   * @param [in] width            Buffer width in pixels
   * @param [in] height           Buffer height in pixels
   * @param [in] pixelFormat      The pixel format
   * @param [in] releaseFunction  The function used to release the memory.
   */
  static PixelDataPtr New( uint8_t* buffer,
                           uint32_t bufferSize,
                           uint32_t width,
                           uint32_t height,
                           Pixel::Format pixelFormat,
                           Dali::PixelData::ReleaseFunction releaseFunction);

  /**
   * @brief Constructor.
   *
   * @param [in] buffer           The raw pixel data.
   * @param [in] bufferSize       The size of the buffer in bytes
   * @param [in] width            Buffer width in pixels
   * @param [in] height           Buffer height in pixels
   * @param [in] pixelFormat      The pixel format
   * @param [in] releaseFunction  The function used to release the memory.
   */
  PixelData( uint8_t* buffer,
             uint32_t bufferSize,
             uint32_t width,
             uint32_t height,
             Pixel::Format pixelFormat,
             Dali::PixelData::ReleaseFunction releaseFunction );

protected:

  /**
   * @brief Destructor.
   *
   * Release the pixel buffer if exists.
   */
  ~PixelData();

public:

  /**
   * Get the width of the buffer in pixels.
   * @return The width of the buffer in pixels
   */
  uint32_t GetWidth() const;

  /**
   * Get the height of the buffer in pixels
   * @return The height of the buffer in pixels
   */
  uint32_t GetHeight() const;

  /**
   * Get the pixel format
   * @return The pixel format
   */
  Pixel::Format GetPixelFormat() const;

  /**
   * Get the pixel buffer if it's present.
   * @return The buffer if exists, or NULL if there is no pixel buffer.
   */
  uint8_t* GetBuffer() const;

  /**
   * Get the size of the buffer in bytes
   * @return The size of the buffer
   */
  uint32_t GetBufferSize() const;

  /**
   * Return the buffer pointer and reset the internal buffer to zero.
   * @return The buffer pointer and associated data.
   */
  DevelPixelData::PixelDataBuffer ReleaseBuffer();

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

  uint8_t*   mBuffer;           ///< The raw pixel data
  uint32_t   mBufferSize;       ///< Buffer sized in bytes
  uint32_t   mWidth;            ///< Buffer width in pixels
  uint32_t   mHeight;           ///< Buffer height in pixels
  Pixel::Format  mPixelFormat;  ///< Pixel format
  Dali::PixelData::ReleaseFunction mReleaseFunction;  ///< Function for releasing memory
};

} // namespace Internal

/**
 * Helper methods for public API
 */
inline Internal::PixelData& GetImplementation( Dali::PixelData& handle )
{
  DALI_ASSERT_ALWAYS( handle && "handle is empty" );

  BaseObject& object = handle.GetBaseObject();

  return static_cast<Internal::PixelData&>( object );
}

inline const Internal::PixelData& GetImplementation( const Dali::PixelData& handle )
{
  DALI_ASSERT_ALWAYS( handle && "handle is empty" );

  const BaseObject& object = handle.GetBaseObject();

  return static_cast<const Internal::PixelData&>( object );
}

} // namespace Dali

#endif // __DALI_INTERNAL_PIXEL_DATA_H__
