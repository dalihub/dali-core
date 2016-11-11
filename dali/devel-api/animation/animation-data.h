#ifndef __DALI_ANIMATION_DATA_H__
#define __DALI_ANIMATION_DATA_H__

/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <string>

// INTERNAL INCLUDES
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/animation/alpha-function.h>
#include <dali/public-api/animation/animation.h>
#include <dali/public-api/common/dali-vector.h>
#include <dali/public-api/object/property-value.h>

namespace Dali
{

/**
 * @brief This object stores description data that can be used to generate an Animation.
 * This data can be produced from passing JSON.
 *
 * This then allows the same description data to be used to repeatedly create an animation multiple times.
 */
class DALI_IMPORT_API AnimationData
{
public:

  AnimationData();

  ~AnimationData();

  /**
   * @brief AnimationDataElement Describes one part of an animation.
   */
  struct AnimationDataElement
  {
    std::string actor;
    std::string property;
    Property::Value value;
    AlphaFunction::BuiltinFunction alphaFunction;
    float timePeriodDelay;
    float timePeriodDuration;

    AnimationDataElement()
    : alphaFunction( AlphaFunction::DEFAULT ),
      timePeriodDelay( 0.0f ),
      timePeriodDuration( 1.0f )
    {
    }
  };


  /**
   * @brief AnimationData holds the required data required to define an
   * animation to be performed on an actor or actors.
   */
  typedef Dali::Vector< AnimationDataElement* > AnimationDataList;


  /**
   * @brief Adds one AnimationDataElement to the list to describe one animation.
   * @param[in] animationDataElement A pre-populated struct to add
   * @note This class takes ownership of animationDataElement
   */
  void Add( AnimationDataElement* animationDataElement );


  /**
   * @brief Creates a Dali::Animation from this AnimationData object.
   * The AnimationData object can describe multiple individual property animations.
   * Each one will be added to a created animation.
   *
   * If there is no animation data defined, an invalid Dali::Animation handle is returned.
   * @param[in] targetActor The actor to target the property animations to.
   * @param[in] duration The duration of the animation to create.
   * @return    A ready-to-go Dali::Animation, or an invalid handle if there was no data.
   */
  Dali::Animation CreateAnimation( Dali::Actor targetActor, float duration );


  /**
   * @brief Empties this AnimationData object (and frees all memory).
   */
  void Clear();

private:

  AnimationDataList mAnimationDataList; ///< A vector of individual property animations from which to generate a Dali::Animation.

};


} // namespace Dali

#endif // __DALI_ANIMATION_DATA_H__

