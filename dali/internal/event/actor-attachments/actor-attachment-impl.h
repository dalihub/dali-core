#ifndef __DALI_INTERNAL_ACTOR_ATTACHMENT_H__
#define __DALI_INTERNAL_ACTOR_ATTACHMENT_H__

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

// INTERNAL INCLUDES
#include <dali/public-api/object/ref-object.h>

namespace Dali
{

namespace Internal
{

class Stage;

/**
 * An abstract base class for attachments, such a renderable objects and lights.
 * ActorAttachments must be attached to an actor, in order to be displayed.
 */
class ActorAttachment : public Dali::RefObject
{
public:

  /**
   * Query whether the attachment is connected to the stage.
   * @return True if the attachment is connected to the stage.
   */
  bool OnStage() const;

  /**
   * Called by the attached actor, when connected to the Stage.
   */
  void Connect();

  /**
   * Called by the attached actor, when the actor is disconnected from the Stage.
   */
  void Disconnect();

  /**
   * Sets whether a renderable attachment is visible.
   * By default attachments are visible.
   * @param[in] visible True if the object is visible.
   */
  void SetVisible(bool visible);

  /**
   * Retrieve whether a renderable attachment is visible.
   * @return True if the object is visible.
   */
  bool IsVisible() const;

protected:

  /**
   * Construct a new attachment.
   * @param[in] stage Used to send messages to scene-graph.
   */
  ActorAttachment( Stage& stage );

  /**
   * A reference counted object may only be deleted by calling Unreference()
   */
  virtual ~ActorAttachment();

private:

  // Undefined
  ActorAttachment(const ActorAttachment&);

  // Undefined
  ActorAttachment& operator=(const ActorAttachment& rhs);

  /**
   * For use in derived classes, called after ConnectToStage()
   */
  virtual void OnStageConnection() = 0;

  /**
   * For use in derived classes, called after DisconnectFromStage()
   */
  virtual void OnStageDisconnection() = 0;

protected:

  Stage* mStage; ///< Used to send messages to scene-graph; valid until Core destruction

  bool mIsOnStage : 1; ///< Flag to identify whether the attachment is on-stage
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_ACTOR_ATTACHMENT_H__
