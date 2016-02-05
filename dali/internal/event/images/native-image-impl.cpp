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

// CLASS HEADER
#include <dali/internal/event/images/native-image-impl.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/object/type-registry.h>
#include <dali/integration-api/debug.h>
#include <dali/internal/event/resources/resource-ticket.h>
#include <dali/internal/event/common/thread-local-storage.h>
#include <dali/internal/event/resources/resource-client.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/devel-api/images/native-image-interface-extension.h>

using namespace Dali::Integration;

namespace Dali
{

namespace Internal
{

namespace
{
TypeRegistration mType( typeid(Dali::NativeImage), typeid(Dali::Image), NULL );
}

NativeImage::NativeImage( NativeImageInterface& resourceData )
: Image()
{
  NativeImageInterface::Extension* extension = resourceData.GetExtension();
  if( extension != NULL )
  {
    mCustomFragmentPreFix = extension->GetCustomFragmentPreFix();
    mCustomSamplerTypename = extension->GetCustomSamplerTypename();
  }
}

NativeImagePtr NativeImage::New( NativeImageInterface& resourceData )
{
  NativeImagePtr image = new NativeImage( resourceData );
  image->Initialize();

  ResourceClient &resourceClient = ThreadLocalStorage::Get().GetResourceClient();

  image->mWidth  = resourceData.GetWidth();
  image->mHeight = resourceData.GetHeight();

  const ResourceTicketPtr& ticket = resourceClient.AddNativeImage( resourceData );
  DALI_ASSERT_DEBUG( dynamic_cast<ImageTicket*>( ticket.Get() ) && "Resource ticket not ImageTicket subclass for image resource.\n" );
  image->mTicket = static_cast<ImageTicket*>(ticket.Get());
  image->mTicket->AddObserver( *image );

  return image;
}

NativeImage::~NativeImage()
{
}

void NativeImage::CreateGlTexture()
{
  ResourceClient& resourceClient = ThreadLocalStorage::Get().GetResourceClient();
  resourceClient.CreateGlTexture( GetResourceId() );
}

const char* NativeImage::GetCustomFragmentPreFix()
{
  if( mCustomFragmentPreFix.empty() )
  {
    return NULL;
  }

  return mCustomFragmentPreFix.c_str();
}

const char* NativeImage::GetCustomSamplerTypename()
{
  if( mCustomSamplerTypename.empty() )
  {
    return NULL;
  }

  return mCustomSamplerTypename.c_str();
}

} // namespace Internal

} // namespace Dali
