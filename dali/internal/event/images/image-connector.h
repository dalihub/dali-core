#ifndef __DALI_INTERNAL_IMAGE_CONNECTOR_H__
#define __DALI_INTERNAL_IMAGE_CONNECTOR_H__

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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/internal/event/images/image-impl.h>

namespace Dali
{

namespace Internal
{

/**
 * Wrapper class which helps managing intrusive pointer assignments and Connect / Disconnect.
 */
class ImageConnector
{

public:
  /**
   * Constructor. Takes no parameters.
   */
  ImageConnector();

  /**
   * Destructor.
   */
  ~ImageConnector();

  /**
   * Returns a pointer to the managed Image or NULL.
   * @return a pointer to mImage or NULL if not set.
   */
  Image* Get() const;

  /**
   * Assigns image, calling Connect and Disconnect methods accordingly, taking onStage into account.
   * @param [in] image   pointer to new Image
   * @param [in] onStage whether Image is used on stage or not
   */
  void Set ( Image *image, bool onStage);

  /**
   * Manages connection reference count.
   * Must be called from owner when connected to stage.
   */
  void OnStageConnect();

  /**
   * Manages connection reference count.
   * Must be called from owner when disconnecting from stage.
   */
  void OnStageDisconnect();

private:

  ImageConnector( const ImageConnector& ptr );                     ///< copy constructor, not defined
  const ImageConnector& operator=( const ImageConnector& ptr );  ///< copy assignment operator, not defined

  ImagePtr mImage;  ///< intrusive pointer to the Image. ImageConnector owns this.
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_IMAGE_CONNECTOR_H__
