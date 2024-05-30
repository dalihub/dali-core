/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
#include <dali/internal/event/animation/animation-playlist.h>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/event/animation/animation-impl.h>
#include <dali/public-api/common/vector-wrapper.h>

namespace Dali
{
namespace Internal
{
AnimationPlaylist* AnimationPlaylist::New()
{
  return new AnimationPlaylist();
}

AnimationPlaylist::AnimationPlaylist() = default;

AnimationPlaylist::~AnimationPlaylist() = default;

void AnimationPlaylist::AnimationCreated(Animation& animation)
{
  mAnimations.PushBack(&animation);
}

void AnimationPlaylist::AnimationDestroyed(Animation& animation)
{
  Dali::Vector<Animation*>::Iterator iter = std::find(mAnimations.Begin(), mAnimations.End(), &animation);
  DALI_ASSERT_ALWAYS(iter != mAnimations.End() && "Animation not found");

  mAnimations.Remove(iter);
}

void AnimationPlaylist::OnPlay(Animation& animation)
{
  mPlaylist.insert(Dali::Animation(&animation));
}

void AnimationPlaylist::OnClear(Animation& animation, bool ignoreRequired)
{
  // Keep handle for safety.
  auto handle = Dali::Animation(&animation);

  auto iter = mPlaylist.find(handle);
  if(iter != mPlaylist.end())
  {
    mPlaylist.erase(iter);
  }

  if(ignoreRequired)
  {
    mIgnoredAnimations.insert(&animation);
  }
}

void AnimationPlaylist::EventLoopFinished()
{
  mIgnoredAnimations.clear();
}

void AnimationPlaylist::NotifyCompleted()
{
  std::vector<Dali::Animation> finishedAnimations;

  // Since animations can be unreferenced during the signal emissions, iterators into animationPointers may be invalidated.
  // First copy and reference the finished animations, then emit signals
  for(Dali::Vector<Animation*>::Iterator iter = mAnimations.Begin(); iter != mAnimations.End(); ++iter)
  {
    Animation* animation = *iter;

    if(mIgnoredAnimations.find(animation) == mIgnoredAnimations.end() && animation->HasFinished())
    {
      Dali::Animation handle(animation);

      finishedAnimations.push_back(handle);

      // The animation may be present in mPlaylist - remove if necessary
      // Note that the animation "Finish" signal is emitted after Stop() has been called
      OnClear(*animation, false);
    }
  }

  // Now it's safe to emit the signals
  for(std::vector<Dali::Animation>::iterator iter = finishedAnimations.begin(); iter != finishedAnimations.end(); ++iter)
  {
    Dali::Animation& handle = *iter;

    GetImplementation(handle).EmitSignalFinish();
  }
}

void AnimationPlaylist::NotifyProgressReached(const SceneGraph::Animation* sceneGraphAnimation)
{
  std::vector<Dali::Animation> notifyProgressAnimations; // Will own animations until all emits have been done

  for(Dali::Vector<Animation*>::Iterator iter = mAnimations.Begin(); iter != mAnimations.End(); ++iter)
  {
    Animation* animation = *iter;

    if((animation->GetSceneObject()) == sceneGraphAnimation)
    {
      // Store handles to animations that need signals emitted in the case of an animation being cleared in-between emits
      notifyProgressAnimations.push_back(Dali::Animation(animation));
    }
  }

  for(std::vector<Dali::Animation>::iterator iter = notifyProgressAnimations.begin(); iter != notifyProgressAnimations.end(); ++iter)
  {
    Dali::Animation& handle = *iter;

    GetImplementation(handle).EmitSignalProgressReached();
  }
}

uint32_t AnimationPlaylist::GetAnimationCount()
{
  return mAnimations.Size();
}

Dali::Animation AnimationPlaylist::GetAnimationAt(uint32_t index)
{
  if(index >= mAnimations.Size())
  {
    DALI_LOG_ERROR("Animation index is out of bounds.\n");
    return Dali::Animation();
  }
  return Dali::Animation(mAnimations[index]);
}

} // namespace Internal

} // namespace Dali
