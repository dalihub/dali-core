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
#include <dali/internal/event/images/encoded-buffer-image-impl.h>

// EXTERNAL INCLUDES
#include <cstring> // for memcpy

// INTERNAL INCLUDES
#include <dali/public-api/object/type-registry.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/integration-api/platform-abstraction.h>

namespace Dali
{
namespace Internal
{

namespace
{
TypeRegistration mType( typeid( Dali::EncodedBufferImage ), typeid( Dali::Image ), NULL );
} // unnamed namespace

EncodedBufferImagePtr EncodedBufferImage::New( const uint8_t * const encodedImage,
                                               std::size_t encodedImageByteCount,
                                               ImageDimensions size, FittingMode::Type fittingMode, SamplingMode::Type samplingMode,
                                               bool orientationCorrection )
{
  DALI_ASSERT_DEBUG( encodedImage && "Null image pointer passed-in for decoding from memory." );
  DALI_ASSERT_DEBUG( encodedImageByteCount > 0U && "Zero size passed for image resource in memory buffer." );
  DALI_ASSERT_ALWAYS( encodedImage && (encodedImageByteCount != 0) );
  // SEGV early before we allocate anything if the caller passed in an invalid
  // input buffer by reading both ends of it:
  DALI_ASSERT_ALWAYS( static_cast<int>( encodedImage[0] + encodedImage[encodedImageByteCount-1] ) != -1 );

  EncodedBufferImagePtr image( new EncodedBufferImage() );
  image->Initialize(); // Second stage initialization

  Dali::Integration::BitmapResourceType resourceType( size, fittingMode, samplingMode, orientationCorrection );
  RequestBufferPtr buffer( new RequestBuffer );
  buffer->GetVector().Resize( encodedImageByteCount );
  // Resize() won't throw on failure, so avoid a SEGV if the allocation failed:
  DALI_ASSERT_ALWAYS( buffer->GetVector().Size() >= encodedImageByteCount );

  memcpy( &(buffer->GetVector()[0]), encodedImage, encodedImageByteCount );

  // Get image size from buffer
  Dali::Integration::PlatformAbstraction& platformAbstraction = Internal::ThreadLocalStorage::Get().GetPlatformAbstraction();
  const ImageDimensions expectedSize = platformAbstraction.GetClosestImageSize( buffer, size, fittingMode, samplingMode, orientationCorrection );
  image->mWidth = (unsigned int) expectedSize.GetWidth();
  image->mHeight = (unsigned int) expectedSize.GetHeight();

  // Load the image synchronously
  Integration::BitmapPtr bitmap = platformAbstraction.DecodeBuffer( resourceType, &(buffer->GetVector()[0]), encodedImageByteCount );

  if( bitmap )
  {
    ResourceClient &resourceClient = ThreadLocalStorage::Get().GetResourceClient();
    image->mTicket = resourceClient.AddBitmapImage( bitmap.Get() );
    image->mTicket->AddObserver( *image );
  }

  return image;
}

} // namespace Internal
} // namespace Dali
