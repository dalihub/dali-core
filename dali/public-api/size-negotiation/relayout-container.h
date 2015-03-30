#ifndef __DALI_RELAYOUT_CONTAINER_H__
#define __DALI_RELAYOUT_CONTAINER_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/actors/actor.h>
#include <dali/public-api/math/vector2.h>

namespace Dali
{

class RelayoutContainer;
typedef RelayoutContainer* RelayoutContainerPtr;


/**
 * @brief Interface to encapsulate information required for relayout
 */
class RelayoutContainer
{
public:

  /**
   * @brief Default constructor
   *
   */
  RelayoutContainer() {}

  /**
   * @brief Virtual destructor
   */
  virtual ~RelayoutContainer() {}

  /**
   * @brief Add relayout information to the container if it does'nt already exist
   *
   * @param actor The actor to relayout
   * @param size The size to relayout
   */
  virtual void Add( const Actor& actor, const Vector2& size ) = 0;

};

} // namespace Dali

#endif // __DALI_RELAYOUT_CONTAINER_H__
