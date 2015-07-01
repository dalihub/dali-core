#ifndef __DALI_RENDERABLE_ACTOR_H__
#define __DALI_RENDERABLE_ACTOR_H__

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
#include <dali/public-api/shader-effects/shader-effect.h>
#include <dali/public-api/actors/blending.h>
#include <dali/public-api/actors/sampling.h>

namespace Dali
{
namespace Internal DALI_INTERNAL
{
class RenderableActor;
}

/**
 * @brief Face culling modes.
 */
enum CullFaceMode
{
  CullNone,                 ///< Face culling disabled
  CullFront,                ///< Cull front facing polygons
  CullBack,                 ///< Cull back facing polygons
  CullFrontAndBack          ///< Cull front and back facing polygons
};



/**
 * @brief A base class for renderable actors.
 */
class DALI_IMPORT_API RenderableActor : public Actor
{
public:

  static const BlendingMode::Type DEFAULT_BLENDING_MODE; ///< default value is BlendingMode::AUTO

  /**
   * @brief Create an uninitialized actor.
   *
   * Calling member functions with an uninitialized Dali::Object is not allowed.
   */
  RenderableActor();

  /**
   * @brief Downcast an Object handle to RenderableActor.
   *
   * If handle points to a RenderableActor the
   * downcast produces valid handle. If not the returned handle is left uninitialized.
   * @param[in] handle to An object
   * @return handle to a RenderableActor or an uninitialized handle
   */
  static RenderableActor DownCast( BaseHandle handle );

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   */
  ~RenderableActor();

  /**
   * @brief Copy constructor
   *
   * @param [in] copy The actor to copy.
   */
  RenderableActor(const RenderableActor& copy);

  /**
   * @brief Assignment operator
   *
   * @param [in] rhs The actor to copy.
   */
  RenderableActor& operator=(const RenderableActor& rhs);

public: // Not intended for application developers

  /**
   * @brief This constructor is used by Dali New() methods
   *
   * @param [in] actor A pointer to a newly allocated Dali resource
   */
  explicit DALI_INTERNAL RenderableActor(Internal::RenderableActor* actor);
};


} // namespace Dali

#endif // __DALI_RENDERABLE_ACTOR_H__
