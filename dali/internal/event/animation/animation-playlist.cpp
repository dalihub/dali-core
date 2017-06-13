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

// CLASS HEADER
#include <dali/internal/event/animation/animation-playlist.h>


// INTERNAL INCLUDES
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/internal/event/animation/animation-impl.h>


namespace Dali
{

namespace Internal
{

AnimationPlaylist* AnimationPlaylist::New()
{
  return new AnimationPlaylist();
}

AnimationPlaylist::AnimationPlaylist()
{
}

AnimationPlaylist::~AnimationPlaylist()
{
}

void AnimationPlaylist::AnimationCreated( Animation& animation )
{
  mAnimations.PushBack( &animation );
}

void AnimationPlaylist::AnimationDestroyed( Animation& animation )
{
  Dali::Vector< Animation* >::Iterator iter = std::find( mAnimations.Begin(), mAnimations.End(), &animation );
  DALI_ASSERT_ALWAYS( iter != mAnimations.End() && "Animation not found" );

  mAnimations.Remove( iter );
}

void AnimationPlaylist::OnPlay( Animation& animation )
{
  mPlaylist.push_back( Dali::Animation(&animation) );
}

void AnimationPlaylist::OnClear( Animation& animation )
{
  std::vector< Dali::Animation >::iterator iter = std::find( mPlaylist.begin(), mPlaylist.end(), Dali::Animation(&animation) );
  std::vector< Dali::Animation >::iterator last = mPlaylist.end();
  if( iter != last )
  {
    --last; // move to real last
    std::swap( *iter, *last ); // swap
    mPlaylist.resize( mPlaylist.size() - 1u );
  }
}

void AnimationPlaylist::NotifyCompleted()
{
  std::vector< Dali::Animation > finishedAnimations;

  // Since animations can be unreferenced during the signal emissions, iterators into animationPointers may be invalidated.
  // First copy and reference the finished animations, then emit signals
  for ( Dali::Vector< Animation* >::Iterator iter = mAnimations.Begin(); iter != mAnimations.End(); ++iter )
  {
    Animation* animation = *iter;

    if ( animation->HasFinished() )
    {
      finishedAnimations.push_back( Dali::Animation(animation) );

      // The animation may be present in mPlaylist - remove if necessary
      // Note that the animation "Finish" signal is emitted after Stop() has been called
      std::vector< Dali::Animation >::iterator iter = std::find( mPlaylist.begin(), mPlaylist.end(), Dali::Animation(animation) );
      DALI_ASSERT_DEBUG(iter != mPlaylist.end());
      mPlaylist.erase( iter );
    }
  }

  // Now it's safe to emit the signals
  for ( std::vector< Dali::Animation >::iterator iter = finishedAnimations.begin(); iter != finishedAnimations.end(); ++iter )
  {
    Dali::Animation& handle = *iter;

    GetImplementation(handle).EmitSignalFinish();
  }
}

void AnimationPlaylist::NotifyProgressReached( const SceneGraph::Animation* sceneGraphAnimation )
{
  std::vector< Dali::Animation > notifyProgressAnimations; // Will own animations until all emits have been done

  for ( Dali::Vector< Animation* >::Iterator iter = mAnimations.Begin(); iter != mAnimations.End(); ++iter )
  {
    Animation* animation = *iter;

    if ( ( animation->GetSceneObject() ) == sceneGraphAnimation )
    {
      // Store handles to animations that need signals emitted in the case of an animation being cleared in-between emits
      notifyProgressAnimations.push_back(  Dali::Animation( animation ) );
    }
  }

  for ( std::vector< Dali::Animation >::iterator iter = notifyProgressAnimations.begin(); iter != notifyProgressAnimations.end(); ++iter )
  {
    Dali::Animation& handle = *iter;

    GetImplementation(handle).EmitSignalProgressReached();
  }
}

} // namespace Internal

} // namespace Dali
