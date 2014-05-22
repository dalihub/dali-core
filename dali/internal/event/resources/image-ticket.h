#ifndef __DALI_INTERNAL_IMAGE_TICKET_H__
#define __DALI_INTERNAL_IMAGE_TICKET_H__

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
#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/public-api/images/image.h>

namespace Dali
{

namespace Internal
{
class ImageTicket;
class  ResourceManager;

typedef IntrusivePtr<ImageTicket>    ImageTicketPtr;

/**
 * ImageTicket is essentially the same as a ResourceTicket,
 * except that it contains additional information about the loaded image resource.
 * The data is the result of the resource loading request.
 *
 * Multi-threading note: this class should be used in the main thread only
 * i.e. not from within Dali::Integration::Core::Render().
 */
class ImageTicket : public ResourceTicket
{
public:

  /**
   * Create an image resource request.
   * This should only be done by the ResourceTicketRegistry.
   * @param [in] observer A resource ticket lifetime observer.
   * @param [in] id A unique ID for this ticket (loading request).
   * @param [in] typePath The resource Type & Path pair.
   */
  ImageTicket( ResourceTicketLifetimeObserver& observer, unsigned int id, ResourceTypePath& typePath );

  /**
   * Get the attributes of an image.
   * Only to be used after the image has finished loading.
   * (Ticket's LoadingSucceeded callback was called)
   * Reflects the last cached values after a LoadComplete.
   * If requested width or height was 0, they are replaced by concrete dimensions.
   * @return a copy of the image attributes
   */
  const Dali::ImageAttributes& GetAttributes() const { return mAttributes;}

  /**
   * Get the width of an image.
   * Only to be used after the image has finished loading.
   * (Ticket's LoadingSucceeded callback was called)
   * Reflects the last cached values after a LoadComplete.
   * If requested width or height was 0, they are replaced by concrete dimensions.
   * @return the width
   */
  int GetWidth() const;

  /**
   * Get the height of an image.
   * Only to be used after the image has finished loading.
   * (Ticket's LoadingSucceeded callback was called)
   * Reflects the last cached values after a LoadComplete.
   * If requested width or height was 0, they are replaced by concrete dimensions.
   * @return the height
   */
  int GetHeight() const;

protected:

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ImageTicket()
  {
  }

private:

  // Undefined copy constructor.
  ImageTicket(const ImageTicket& typePath);

  // Undefined copy constructor.
  ImageTicket& operator=(const ImageTicket& rhs);

private:

  /**
   * Loaded Image attributes (width, height, pixelformat etc.).
   * Contains actual values only after the image has finished loading.
   * If requested width or height was 0, the natural size is used.
   */
  Dali::ImageAttributes mAttributes;

  /*
   * ResourceClient needs to set dimensions and pixelformat.
   * Image needs to know about them.
   */
  friend class ResourceClient;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_IMAGE_TICKET_H__
