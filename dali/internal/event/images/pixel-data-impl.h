#ifndef __DALI_INTERNAL_PIXEL_DATA_H__
#define __DALI_INTERNAL_PIXEL_DATA_H__

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

// INTERNAL INCLUDES
#include <dali/devel-api/images/pixel-data.h>
#include <dali/public-api/object/base-object.h>

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
   * @param [in] width            Buffer width in pixels
   * @param [in] height           Buffer height in pixels
   * @param [in] pixelFormat      The pixel format
   * @param [in] releaseFunction  The function used to release the memory.
   */
  static PixelDataPtr New( unsigned char* buffer,
                           unsigned int width,
                           unsigned int height,
                           Pixel::Format pixelFormat,
                           Dali::PixelData::ReleaseFunction releaseFunction);

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
   * @return The buffer if exists, or NULL if there is no pixel buffer.
   */
  unsigned char* GetBuffer() const;

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
