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
#include <dali/internal/event/images/image-connector.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Internal
{

ImageConnector::ImageConnector()
{
}

ImageConnector::~ImageConnector()
{
}

ImagePtr ImageConnector::Get() const
{
  return mImage;
}

void ImageConnector::Set( ImagePtr image, bool onStage )
{
  if ( mImage != image )
  {
    // Disconnect from old image
    if ( mImage && onStage )
    {
      mImage->Disconnect();
    }

    mImage = image;

    // Connect to new image
    if ( mImage && onStage )
    {
      mImage->Connect();
    }
  }
}

void ImageConnector::OnStageConnect()
{
  if ( mImage )
  {
    mImage->Connect();
  }
}

void ImageConnector::OnStageDisconnect()
{
  if ( mImage )
  {
    mImage->Disconnect();
  }
}

} // namespace Internal

} // namespace Dali
