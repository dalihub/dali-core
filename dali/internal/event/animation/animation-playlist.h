#ifndef __DALI_INTERNAL_ANIMATION_PLAYLIST_H__
#define __DALI_INTERNAL_ANIMATION_PLAYLIST_H__

/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/complete-notification-interface.h>
#include <dali/internal/update/animation/scene-graph-animation.h>

namespace Dali
{

namespace Internal
{

class Animation;

/**
 * AnimationPlaylist provides notifications to applications when animations are finished.
 * It reference-counts playing animations, to allow "fire and forget" behaviour.
 */
class AnimationPlaylist : public CompleteNotificationInterface
{
public:

  /**
   * Create an AnimationPlaylist.
   * @return A newly allocated animation playlist.
   */
  static AnimationPlaylist* New();

  /**
   * Virtual destructor.
   */
  virtual ~AnimationPlaylist();

  /**
   * Called when an animation is constructed.
   */
  void AnimationCreated( Animation& animation );

  /**
   * Called when an animation is destroyed.
   */
  void AnimationDestroyed( Animation& animation );

  /**
   * Called when an animation is playing.
   * @post The animation will be referenced by AnimationPlaylist, until the "Finished" signal is emitted.
   */
  void OnPlay( Animation& animation );

  /**
   * Called when an animation is cleared.
   * @post The animation will no longer be referenced by AnimationPlaylist.
   */
  void OnClear( Animation& animation );

  /**
   * @brief Notify that an animation has reached a progress marker
   * @param[in] sceneGraphAnimation scene graph animation that has reached progress
   */
  void NotifyProgressReached( const SceneGraph::Animation* sceneGraphAnimation );

private:

  /**
   * Create an AnimationPlaylist.
   */
  AnimationPlaylist();

  // Undefined
  AnimationPlaylist(const AnimationPlaylist&);

  // Undefined
  AnimationPlaylist& operator=(const AnimationPlaylist& rhs);

private: // from CompleteNotificationInterface

  /**
   * @copydoc CompleteNotificationInterface::NotifyCompleted()
   */
  virtual void NotifyCompleted();

private:

  Dali::Vector< Animation* > mAnimations; ///< All existing animations (not owned)
  std::vector< Dali::Animation > mPlaylist; ///< The currently playing animations (owned through handle)

};

/**
 * Called when an animation reaches a progress marker
 *
 * Note animationPlaylist is of type CompleteNotificationInterface because of updateManager only knowing about the interface not actual playlist
 */
inline MessageBase* NotifyProgressReachedMessage( CompleteNotificationInterface& animationPlaylist, const SceneGraph::Animation* animation )
{
  return new MessageValue1< AnimationPlaylist, const SceneGraph::Animation* >( static_cast<AnimationPlaylist*>(&animationPlaylist), &AnimationPlaylist::NotifyProgressReached, animation );
}

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_NOTIFICATION_MANAGER_H__

