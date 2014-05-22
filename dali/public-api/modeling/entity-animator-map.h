#ifndef __DALI_ENTITY_ANIMATOR_MAP_H__
#define __DALI_ENTITY_ANIMATOR_MAP_H__

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
#include <dali/public-api/animation/key-frames.h>
#include <dali/public-api/modeling/entity.h>

namespace Dali DALI_IMPORT_API
{

class Entity;

class EntityAnimatorMap;
typedef std::vector<EntityAnimatorMap>             EntityAnimatorMapContainer; ///< Container of Entity - Animator mappings
typedef EntityAnimatorMapContainer::const_iterator EntityAnimatorMapIter; ///< Iterator for @ref Dali::EntityAnimatorMapContainer


/**
 * @brief The entity animator map class is a mapping from an entity name to raw animation
 * data for that entity, stored in 3 KeyFrames for position, scale and rotation.
 */
class EntityAnimatorMap
{
public:
  /**
   * @brief Constructor.
   */
  EntityAnimatorMap(const std::string& name);

  /**
   * @brief Destructor.
   */
  virtual ~EntityAnimatorMap();

  /**
   * @brief Set the entity name.
   *
   * @param[in] name - the entity name
   */
  void SetEntityName(const std::string&  name)
  {
    mEntityName = name;
  }

  /**
   * @brief Get the entity name.
   *
   * @return the name of the entity to which this animator applies
   */
  const std::string& GetEntityName() const
  {
    return mEntityName;
  }

  /**
   * @brief Set the animated entity.
   *
   * @param[in] animatedEntity The animated entity
   */
  void SetAnimatedEntity(Entity animatedEntity)
  {
    mAnimatedEntity = animatedEntity;
  }

  /**
   * @brief Get the animated entity.
   *
   * @return the animated entity
   */
  Entity GetAnimatedEntity(void)
  {
    return mAnimatedEntity;
  }

  /**
   * @brief Set the key frames for animating the entity position.
   *
   * @param[in] keyFrames The keyframe data
   */
  void SetPositionKeyFrames(KeyFrames keyFrames)
  {
    mPositionKeyFrames = keyFrames;
  }

  /**
   * @brief Set the key frames for animating the entity scale.
   *
   * @param[in] keyFrames The keyframe data
   */
  void SetScaleKeyFrames(KeyFrames keyFrames)
  {
    mScaleKeyFrames = keyFrames;
  }

  /**
   * @brief Set the key frames for animating the entity rotation.
   *
   * @param[in] keyFrames The keyframe data
   */
  void SetRotationKeyFrames(KeyFrames keyFrames)
  {
    mRotationKeyFrames = keyFrames;
  }

  /**
   * @brief Get the keyframes for animating the entity position.
   *
   * @return A handle to the keyframes
   */
  KeyFrames GetPositionKeyFrames() const
  {
    return mPositionKeyFrames;
  }

  /**
   * @brief Get the keyframes for animating the entity scale.
   *
   * @return A handle to the keyframes
   */
  KeyFrames GetScaleKeyFrames() const
  {
    return mScaleKeyFrames;
  }

  /**
   * @brief Get the keyframes for animating the entity rotation.
   *
   * @return A handle to the keyframes
   */
  KeyFrames GetRotationKeyFrames() const
  {
    return mRotationKeyFrames;
  }

  /**
   * @brief Set the duration of the animation on this entity.
   *
   * @param[in] duration - the duration in seconds.
   */
  void SetDuration(float duration) { mDuration = duration; }

  /**
   * @brief Get the duration of the animation on this entity.
   *
   * @return the duration in seconds
   */
  const float GetDuration() const { return mDuration; }

private:
  std::string   mEntityName;          ///< The entity name
  Entity        mAnimatedEntity;      ///< Unused
  float         mDuration;            ///< Duration of this animator

  KeyFrames     mPositionKeyFrames;   ///< Position key frames (may be null)
  KeyFrames     mScaleKeyFrames;      ///< Scale key frames (may be null)
  KeyFrames     mRotationKeyFrames;   ///< Rotation key frames (may be null)
};


} // Dali

#endif // __DALI_ENTITY_ANIMATOR_MAP_H__
