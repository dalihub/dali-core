#ifndef __DALI_INTERNAL_ANIMATION_PLAYLIST_H__
#define __DALI_INTERNAL_ANIMATION_PLAYLIST_H__

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

// EXTERNAL INCLUDES
#include <set>

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>
#include <dali/internal/event/animation/animation-finished-notifier.h>
#include <dali/public-api/animation/animation.h>

namespace Dali
{

namespace Internal
{

class Animation;

/**
 * AnimationPlaylist provides notifications to applications when animations are finished.
 * It reference-counts playing animations, to allow "fire and forget" behaviour.
 */
class AnimationPlaylist : public AnimationFinishedNotifier
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
   * From AnimationFinishedNotifier; emit "Finished" signal on any animations that have finished.
   * This method should be called in the event-thread; the update-thread must use AnimationFinishedMessage.
   * @post The "Finished" animations will no longer be referenced by AnimationPlaylist.
   */
  void NotifyFinishedAnimations();

private:

  /**
   * Create an AnimationPlaylist.
   */
  AnimationPlaylist();

  // Undefined
  AnimationPlaylist(const AnimationPlaylist&);

  // Undefined
  AnimationPlaylist& operator=(const AnimationPlaylist& rhs);

private:

  std::set< Animation* > mAnimations; ///< All existing animations (not referenced)

  std::set< Dali::Animation > mPlaylist; ///< The currently playing animations (reference counted)
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_NOTIFICATION_MANAGER_H__

