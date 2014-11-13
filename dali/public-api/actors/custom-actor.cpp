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
#include <dali/public-api/actors/custom-actor.h>

#include <dali/public-api/object/type-registry.h>

// INTERNAL INCLUDES
#include <dali/internal/event/actors/custom-actor-internal.h>

namespace Dali
{

namespace
{

using namespace Dali;

BaseHandle Create()
{
  // not directly creatable
  return BaseHandle();
}

TypeRegistration mType( typeid(Dali::CustomActor), typeid(Dali::Actor), Create );

}

CustomActor::CustomActor()
{
}

CustomActor CustomActor::DownCast( BaseHandle handle )
{
  return CustomActor( dynamic_cast<Dali::Internal::CustomActor*>(handle.GetObjectPtr()) );
}

CustomActor::~CustomActor()
{
}

CustomActor::CustomActor(const CustomActor& copy)
: Actor(copy)
{
}

CustomActor& CustomActor::operator=(const CustomActor& rhs)
{
  BaseHandle::operator=(rhs);
  return *this;
}

CustomActorImpl& CustomActor::GetImplementation()
{
  Internal::CustomActor& internal = GetImpl(*this);

  return internal.GetImplementation();
}

const CustomActorImpl& CustomActor::GetImplementation() const
{
  const Internal::CustomActor& internal = GetImpl(*this);

  return internal.GetImplementation();
}

CustomActor::CustomActor(CustomActorImpl& implementation)
: Actor(Internal::CustomActor::New(implementation).Get())
{
}

CustomActor::CustomActor(Internal::CustomActor* internal)
: Actor(internal)
{
  // Check we haven't just constructed a new handle, while in the internal custom actors destructor.
  // This can happen if the user defined CustomActorImpl destructor calls Self(), to create a new handle
  // to the CustomActor.
  //
  // If it's in the destructor then the ref count is zero, so once we've created a new handle it will be 1
  // Without this check, the actor will be deleted a second time, when the handle is disposed of
  // causing a crash.

  if (internal)
  {
    DALI_ASSERT_ALWAYS(internal->ReferenceCount() != 1 && "Are you trying to use CustomActorImpl::Self() inside a CustomActorImpl destructor?");
  }
}

} // namespace Dali
