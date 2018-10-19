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

// CLASS HEADER
#include <dali/devel-api/update/frame-callback-interface.h>

// INTERNAL INCLUDES
#include <dali/devel-api/update/update-proxy.h>
#include <dali/internal/event/common/stage-impl.h>
#include <dali/internal/event/update/frame-callback-interface-impl.h>

namespace Dali
{

FrameCallbackInterface::FrameCallbackInterface()
: mImpl( std::unique_ptr< Impl >( new Impl ) )
{
}

FrameCallbackInterface::~FrameCallbackInterface()
{
  if( Internal::Stage::IsInstalled() )
  {
    Internal::StagePtr stage = Internal::Stage::GetCurrent();
    if( stage )
    {
      // This will be a no-op if the callback has already been removed
      stage->RemoveFrameCallback( *this );
    }
  }
}

} // namespace Dali
