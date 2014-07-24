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
#include <dali/internal/event/images/image-factory-cache.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/images/image-factory.h>

// EXTERNAL INCLUDES


using namespace Dali::Internal::ImageFactoryCache;

namespace Dali
{

namespace Internal
{

namespace ImageFactoryCache
{
  Request::Request( RequestLifetimeObserver& observer,
                    RequestId reqId,
                    ResourceId resId,
                    const std::string& path,
                    const ImageAttributes* attr )
  : resourceId( resId ),
    url( path ),
    mId( reqId ),
    mLifetimeObserver( &observer )
  {
    if( attr )
    {
      attributes = new ImageAttributes( *attr );
    }
    else
    {
      attributes = NULL;
    }
  }

  Request::~Request()
  {
    if( Stage::IsInstalled() && mLifetimeObserver )
    {
      mLifetimeObserver->RequestDiscarded( *this );
    }
    delete attributes;
  }

  RequestId Request::GetId() const
  {
    return mId;
  }

  void Request::StopLifetimeObservation()
  {
    mLifetimeObserver = NULL;
  }

} // namespace ImageFactoryCache

} // namespace Internal

} // namespace Dali
