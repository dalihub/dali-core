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
typedef std::vector<EntityAnimatorMap>             EntityAnimatorMapContainer;
typedef EntityAnimatorMapContainer::const_iterator EntityAnimatorMapIter;


/**
 * The entity animator map class holds the keyframe channels associated
 * with a named entity.
 * (It only holds key frame data, it cannot be used by the animation system)
 */
class EntityAnimatorMap
{
public:
  /**
   * Constructor
   */
  EntityAnimatorMap(const std::string& name);

  /**
   * Destructor
   */
  virtual ~EntityAnimatorMap();

  /**
   * Set the entity name
   * @param[in] name - the entity name
   */
  void SetEntityName(const std::string&  name)
  {
    mEntityName = name;
  }

  /**
   * Get the entity name
   * @return the name of the entity to which this animator applies
   */
  const std::string& GetEntityName() const
  {
    return mEntityName;
  }

  /**
   * Set the animated entity
   * @param[in] animatedEntity The animated entity
   */
  void SetAnimatedEntity(Entity animatedEntity)
  {
    mAnimatedEntity = animatedEntity;
  }

  /**
   * Get the animated entity.
   * @return the animated entity
   */
  Entity GetAnimatedEntity(void)
  {
    return mAnimatedEntity;
  }

  void SetPositionKeyFrames(KeyFrames keyFrames)
  {
    mPositionKeyFrames = keyFrames;
  }

  void SetScaleKeyFrames(KeyFrames keyFrames)
  {
    mScaleKeyFrames = keyFrames;
  }

  void SetRotationKeyFrames(KeyFrames keyFrames)
  {
    mRotationKeyFrames = keyFrames;
  }


  KeyFrames GetPositionKeyFrames() const
  {
    return mPositionKeyFrames;
  }

  KeyFrames GetScaleKeyFrames() const
  {
    return mScaleKeyFrames;
  }

  KeyFrames GetRotationKeyFrames() const
  {
    return mRotationKeyFrames;
  }

  /**
   * Set the duration of the animation on this entity.
   * @param[in] duration - the duration in seconds.
   */
  void SetDuration(float duration) { mDuration = duration; }

  /**
   * Get the duration of the animation on this entity.
   * @return the duration in seconds
   */
  const float GetDuration() const { return mDuration; }

private:
  std::string   mEntityName;
  Entity        mAnimatedEntity;
  float         mDuration;

  KeyFrames     mPositionKeyFrames;
  KeyFrames     mScaleKeyFrames;
  KeyFrames     mRotationKeyFrames;
};


} // Dali

#endif // __DALI_ENTITY_ANIMATOR_MAP_H__
