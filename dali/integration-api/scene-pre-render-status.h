#ifndef DALI_SCENE_PRE_RENDER_STATUS_H
#define DALI_SCENE_PRE_RENDER_STATUS_H

/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <memory>

// INTERNAL INCLUDES
#include <dali/graphics-api/graphics-controller.h>
#include <dali/graphics-api/graphics-render-target-create-info.h>
#include <dali/public-api/common/vector-wrapper.h>
#include <dali/public-api/events/gesture-enumerations.h>
#include <dali/public-api/math/vector2.h>
#include <dali/public-api/math/vector4.h>
#include <dali/public-api/object/handle.h>

namespace Dali::Integration
{
/**
* The status of the Core::PreRenderScene operation.
*/
class DALI_CORE_API ScenePreRenderStatus
{
public:
  /**
  * Constructor
  */
  ScenePreRenderStatus()
  : hasRenderInstructionToScene(false),
    hadRenderInstructionToScene(false)
  {
  }

  void SetHasRenderInstructionToScene(bool renderInstructionExist)
  {
    hasRenderInstructionToScene = renderInstructionExist;
  }

  bool HasRenderInstructionToScene() const
  {
    return hasRenderInstructionToScene;
  }

  void SetHadRenderInstructionToScene(bool renderInstructionExisted)
  {
    hadRenderInstructionToScene = renderInstructionExisted;
  }

  bool HadRenderInstructionToScene() const
  {
    return hadRenderInstructionToScene;
  }

private:
  bool hasRenderInstructionToScene : 1; ///< True if has render instruction to the scene.
  bool hadRenderInstructionToScene : 1; ///< True if had render instruction to the scene.
};

} // namespace Dali::Integration

#endif // DALI_SCENE_PRE_RENDER_STATUS_H
