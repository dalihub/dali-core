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
#include <dali/internal/event/actor-attachments/actor-attachment-impl.h>

namespace Dali
{

namespace Internal
{

bool ActorAttachment::OnStage() const
{
  return mIsOnStage;
}

/**
 * This method is called by the parent actor.
 */
void ActorAttachment::Connect()
{
  mIsOnStage = true;

  // Notification for derived classes
  OnStageConnection();
}

/**
 * This method is called by the parent actor.
 */
void ActorAttachment::Disconnect()
{
  // Notification for derived classes
  OnStageDisconnection();

  mIsOnStage = false;
}

ActorAttachment::ActorAttachment( EventThreadServices& eventThreadServices )
: mEventThreadServices( eventThreadServices ),
  mIsOnStage( false )
{
}

ActorAttachment::~ActorAttachment()
{
}

} // namespace Internal

} // namespace Dali
