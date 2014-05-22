//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// CLASS HEADER
#include <dali/internal/event/images/encoded-buffer-image-impl.h>

// INTERNAL INCLUDES
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/integration-api/platform-abstraction.h>

namespace Dali
{
namespace Internal
{

Dali::EncodedBufferImage EncodedBufferImage::New( const uint8_t * const encodedImage,
                   const std::size_t encodedImageByteCount,
                   const ImageAttributes& attributes,
                   const ReleasePolicy releasePol )
{
  DALI_ASSERT_DEBUG( encodedImage && "Null image pointer passed-in for decoding from memory." );
  DALI_ASSERT_DEBUG( encodedImageByteCount > 0U && "Zero size passed for image resource in memory buffer." );
  DALI_ASSERT_ALWAYS( encodedImage && (encodedImageByteCount != 0) );
  // SEGV early before we allocate anything if the caller passed in an invalid
  // input buffer by reading both ends of it:
  DALI_ASSERT_ALWAYS( static_cast<int>( encodedImage[0] + encodedImage[encodedImageByteCount-1] ) != -1 );

  EncodedBufferImage* const image = new EncodedBufferImage( releasePol );
  // Make sure that this image object cannot leak if we throw:
  Dali::EncodedBufferImage publicImage(image);

  // Replicate the functionality of ImageFactory::load() without the filesystem caching:
  Dali::Integration::BitmapResourceType resourceType( attributes );
  RequestBufferPtr buffer( new RequestBuffer );
  buffer->GetVector().Resize( encodedImageByteCount );
  // Resize() won't throw on failure, so avoid a SEGV if the allocation failed:
  DALI_ASSERT_ALWAYS( buffer->GetVector().Size() >= encodedImageByteCount );

  memcpy( &(buffer->GetVector()[0]), encodedImage, encodedImageByteCount );

  // Get image size from buffer
  Vector2 size;
  Internal::ThreadLocalStorage::Get().GetPlatformAbstraction().GetClosestImageSize( buffer, attributes, size );
  image->mWidth = (unsigned int) size.width;
  image->mHeight = (unsigned int) size.height;

  ResourceClient &resourceClient = ThreadLocalStorage::Get().GetResourceClient();
  ResourceTicketPtr ticket = resourceClient.DecodeResource( resourceType, buffer );
  if( ticket )
  {
    DALI_ASSERT_DEBUG( dynamic_cast<ImageTicket*>( ticket.Get() ) && "Resource ticket returned for image resource has to be an ImageTicket subclass.\n" );
    ImageTicket * const imageTicket = static_cast<ImageTicket*>( ticket.Get() );

    image->mTicket = imageTicket;
    imageTicket->AddObserver( *image );
  }

  return publicImage;
}

} // namespace Internal
} // namespace Dali
