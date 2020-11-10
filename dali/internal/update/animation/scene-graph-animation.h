#ifndef DALI_INTERNAL_SCENE_GRAPH_ANIMATION_H
#define DALI_INTERNAL_SCENE_GRAPH_ANIMATION_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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

#include <dali/internal/common/buffer-index.h>
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/animation/scene-graph-animator.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

/**
 * Animations are used to change the properties of scene graph objects, as part of a scene
 * managers "update" phase. An animation is a container of Animator objects; the actual setting
 * of object values is done by the animators.
 */
class Animation
{
public:

  using EndAction = Dali::Animation::EndAction;

  enum State
  {
    Stopped,
    Playing,
    Paused,
    Destroyed
  };

  /**
   * Construct a new Animation.
   * @param[in] durationSeconds The duration of the animation in seconds.
   * @param[in] speedFactor Multiplier to the animation velocity.
   * @param[in] playRange Minimum and maximum progress between which the animation will play.
   * @param[in] loopCount The number of times the animation will loop. ( See SetLoopCount() )
   * @param[in] endAction The action to perform when the animation ends.
   * @param[in] disconnectAction The action to perform when the property owner of an animator is disconnected.
   * @return A new Animation
   */
  static Animation* New( float durationSeconds, float speedFactor, const Vector2& playRange, int32_t loopCount, EndAction endAction, EndAction disconnectAction );

  /**
   * Virtual destructor
   */
  virtual ~Animation();

  /**
   * Overriden delete operator
   * Deletes the animation from its global memory pool
   */
  void operator delete( void* ptr );

  /**
   * Set the duration of an animation.
   * @pre durationSeconds must be greater than zero.
   * @param[in] durationSeconds The duration in seconds.
   */
  void SetDuration(float durationSeconds);

  /**
   * Set the progress marker to trigger notification
   * @param[in] progress percent of progress to trigger notification, 0.0f < progress <= 1.0f
   */
  void SetProgressNotification( float progress );

  /**
   * Retrieve the duration of the animation.
   * @return The duration in seconds.
   */
  float GetDuration() const
  {
    return mDurationSeconds;
  }

  /**
   * Retrieve the current progress of the animation.
   * @return The current progress as a normalized value between [0,1].
   */
  float GetCurrentProgress() const
  {
    if( mDurationSeconds > 0.0f )
    {
      return mElapsedSeconds / mDurationSeconds;
    }

    return 0.0f;
  }

  /**
   * Sets the progress of the animation.
   * @param[in] The new progress as a normalized value between [0,1]
   */
  void SetCurrentProgress( float progress )
  {
    mElapsedSeconds = mDurationSeconds * progress;
  }

  /**
   * Specifies a speed factor for the animation.
   * @param[in] factor A value which will multiply the velocity
   */
  void SetSpeedFactor( float factor )
  {
    mSpeedFactor = factor;
  }

  /**
   * Set the animation loop count.
   * 0 is loop forever, N loop play N times
   * @param[in] loopCount The loop count
   */
  void SetLoopCount(int32_t loopCount);

  /**
   * Query whether the animation will loop.
   * @return True if the animation will loop.
   */
  bool IsLooping() const
  {
    return mLoopCount != 1;
  }

  /**
   * Get the loop count
   * @return the loop count
   */
  int32_t GetLoopCount() const
  {
    return mLoopCount;
  }

  /**
   * Set the end action of the animation.
   * @param[in] action The end action.
   */
  void SetEndAction(EndAction action);

  /**
   * Retrieve the action performed when the animation ends.
   * @return The end action.
   */
  EndAction GetEndAction()
  {
    return mEndAction;
  }

  /**
   * Set the disconnect action of the animation when connected objects are disconnected.
   * This action is performed during the next update when
   * the connected object is disconnected.
   * @param[in] action The disconnect action.
   */
  void SetDisconnectAction(EndAction action);

  /**
   * Retrieve the action performed when the animation is destroyed.
   * @return The destroy action.
   */
  EndAction GetDisconnectAction()
  {
    return mDisconnectAction;
  }

  /**
   * Set the playing range. The animation will only play between the minimum and maximum progress
   * speficied.
   *
   * @param[in] range Two values between [0,1] to specify minimum and maximum progress.
   */
  void SetPlayRange( const Vector2& range );

  /**
   * Play the animation.
   */
  void Play();

  /**
   * Play the animation from a given point
   * @param[in] progress A value between [0,1] form where the animation should start playing
   */
  void PlayFrom( float progress );

  /**
   * @brief Play the animation after a given delay time.
   * @param[in] delaySeconds The delay time
   */
  void PlayAfter( float delaySeconds );

  /**
   * Pause the animation.
   */
  void Pause();

  /**
   * Stop the animation.
   * @param[in] bufferIndex The buffer to update when mEndAction == Bake.
   * @return True if the animation has finished (otherwise it wasn't playing)
   */
  bool Stop(BufferIndex bufferIndex);

  /**
   * Called shortly before the animation is destroyed.
   * @param[in] bufferIndex The buffer to update when mEndAction == Bake.
   */
  void OnDestroy(BufferIndex bufferIndex);

  /**
   * Query whether the animation is playing, paused or stopped.
   * Note that even when paused, the Update() method should be called,
   * since the current progress must be reapplied each frame.
   */
  State GetState() const
  {
    return mState;
  }

  /**
   * Retrive a count of the number of times the animation has been played to completion.
   * This can be used to emit "Finised" signals from the public-api
   */
  int32_t GetPlayedCount() const
  {
    return mPlayedCount;
  }

  /**
   * Get the current loop count from zero to GetLoopCount().
   */
  int32_t GetCurrentLoop() const
  {
    return mCurrentLoop;
  }

  /**
   * @brief Sets the looping mode.
   *
   * Animation plays forwards and then restarts from the beginning or runs backwards again.
   * @param[in] loopingMode True when the looping mode is AUTO_REVERSE
   */
  void SetLoopingMode( bool loopingMode );

  /**
   * Add a newly created animator.
   * Animators are automatically removed, when orphaned from an animatable scene object.
   * @param[in] animator The animator to add.
   * @param[in] propertyOwner The scene-object that owns the animatable property.
   * @post The animator is owned by this animation.
   */
  void AddAnimator( OwnerPointer<AnimatorBase>& animator );

  /**
   * This causes the animators to change the properties of objects in the scene graph.
   * @pre The animation is playing or paused.
   * @param[in] bufferIndex The buffer to update.
   * @param[in] elapsedSeconds The time elapsed since the previous frame.
   * @param[out] looped True if the animation looped
   * @param[out] finished True if the animation has finished.
   * @param[out] progressReached True if progress marker reached
   */
  void Update(BufferIndex bufferIndex, float elapsedSeconds, bool& looped, bool& finished, bool& progressReached );


protected:

  /**
   * Protected constructor. See New()
   */
  Animation( float durationSeconds, float speedFactor, const Vector2& playRange, int32_t loopCount, EndAction endAction, EndAction disconnectAction );


private:

  /**
   * Helper for Update, also used to bake when the animation is stopped or destroyed.
   * @param[in] bufferIndex The buffer to update.
   * @param[in] bake True if the final result should be baked.
   * @param[in] animationFinished True if the animation has finished.
   */
  void UpdateAnimators( BufferIndex bufferIndex, bool bake, bool animationFinished );

  /**
   * Helper function to bake the result of the animation when it is stopped or
   * destroyed.
   * @param[in] bufferIndex The buffer to update.
   * @param[in] action The end action specified.
   */
  void Bake(BufferIndex bufferIndex, EndAction action );

  /**
   * Helper function to set active state of animators.
   * @param[in] active Every animator is set to this state
   */
  void SetAnimatorsActive( bool active );

  // Undefined
  Animation(const Animation&);

  // Undefined
  Animation& operator=(const Animation& rhs);

protected:

  OwnerContainer< AnimatorBase* > mAnimators;

  Vector2 mPlayRange;

  float mDurationSeconds;
  float mDelaySeconds;
  float mElapsedSeconds;
  float mSpeedFactor;
  float mProgressMarker;         // Progress marker to trigger a notification

  int32_t mPlayedCount;              // Incremented at end of animation or completion of all loops
                                 // Never incremented when looping forever. Event thread tracks to signal end.
  int32_t mLoopCount;                // N loop setting
  int32_t mCurrentLoop;              // Current loop number

  EndAction mEndAction;
  EndAction mDisconnectAction;

  State mState;

  bool mProgressReachedSignalRequired;  // Flag to indicate the progress marker was hit
  bool mAutoReverseEnabled;             // Flag to identify that the looping mode is auto reverse.
};

}; //namespace SceneGraph

// value types used by messages
template <> struct ParameterType< Dali::Animation::EndAction > : public BasicType< Dali::Animation::EndAction > {};

namespace SceneGraph
{

// Messages for Animation

inline void SetDurationMessage( EventThreadServices& eventThreadServices, const Animation& animation, float durationSeconds )
{
  using LocalType = MessageValue1< Animation, float >;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::SetDuration, durationSeconds );
}

inline void SetProgressNotificationMessage( EventThreadServices& eventThreadServices, const Animation& animation, float progress )
{
  using LocalType = MessageValue1< Animation, float >;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::SetProgressNotification, progress );
}


inline void SetLoopingMessage( EventThreadServices& eventThreadServices, const Animation& animation, int32_t loopCount )
{
  using LocalType = MessageValue1< Animation, int32_t >;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::SetLoopCount, loopCount );
}

inline void SetEndActionMessage( EventThreadServices& eventThreadServices, const Animation& animation, Dali::Animation::EndAction action )
{
  using LocalType = MessageValue1< Animation, Dali::Animation::EndAction >;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::SetEndAction, action );
}

inline void SetDisconnectActionMessage( EventThreadServices& eventThreadServices, const Animation& animation, Dali::Animation::EndAction action )
{
  using LocalType = MessageValue1< Animation, Dali::Animation::EndAction >;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::SetDisconnectAction, action );
}

inline void SetCurrentProgressMessage( EventThreadServices& eventThreadServices, const Animation& animation, float progress )
{
  using LocalType = MessageValue1< Animation, float >;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::SetCurrentProgress, progress );
}

inline void SetSpeedFactorMessage( EventThreadServices& eventThreadServices, const Animation& animation, float factor )
{
  using LocalType = MessageValue1< Animation, float >;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::SetSpeedFactor, factor );
}

inline void SetPlayRangeMessage( EventThreadServices& eventThreadServices, const Animation& animation, const Vector2& range )
{
  using LocalType = MessageValue1< Animation, Vector2 >;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::SetPlayRange, range );
}

inline void PlayAnimationMessage( EventThreadServices& eventThreadServices, const Animation& animation )
{
  using LocalType = Message< Animation >;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::Play );
}

inline void PlayAnimationFromMessage( EventThreadServices& eventThreadServices, const Animation& animation, float progress )
{
  using LocalType = MessageValue1< Animation, float >;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::PlayFrom, progress );
}

inline void PauseAnimationMessage( EventThreadServices& eventThreadServices, const Animation& animation )
{
  using LocalType = Message< Animation >;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::Pause );
}

inline void AddAnimatorMessage( EventThreadServices& eventThreadServices, const Animation& animation, AnimatorBase& animator )
{
  using LocalType = MessageValue1< Animation, OwnerPointer<AnimatorBase> >;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  OwnerPointer<AnimatorBase> parameter( &animator );
  new (slot) LocalType( &animation, &Animation::AddAnimator, parameter );
}

inline void PlayAfterMessage( EventThreadServices& eventThreadServices, const Animation& animation, float delaySeconds )
{
  using LocalType = MessageValue1< Animation, float >;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::PlayAfter, delaySeconds );
}

inline void SetLoopingModeMessage( EventThreadServices& eventThreadServices, const Animation& animation, bool loopingMode )
{
  using LocalType = MessageValue1< Animation, bool >;

  // Reserve some memory inside the message queue
  uint32_t* slot = eventThreadServices.ReserveMessageSlot( sizeof( LocalType ) );

  // Construct message in the message queue memory; note that delete should not be called on the return value
  new (slot) LocalType( &animation, &Animation::SetLoopingMode, loopingMode );
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_ANIMATION_H
