#ifndef DALI_INTERNAL_UPDATE_RENDERING_STENCIL_PARAMETERS_H
#define DALI_INTERNAL_UPDATE_RENDERING_STENCIL_PARAMETERS_H
/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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
 */

#include <dali/public-api/rendering/renderer.h>

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{

/**
 * @brief Struct to encapsulate stencil parameters required for control of the stencil buffer.
 */
struct StencilParameters
{
  StencilParameters( RenderMode::Type renderMode, StencilFunction::Type stencilFunction, int stencilFunctionMask,
                     int stencilFunctionReference, int stencilMask, StencilOperation::Type stencilOperationOnFail,
                     StencilOperation::Type stencilOperationOnZFail, StencilOperation::Type stencilOperationOnZPass )
  : stencilFunctionMask      ( stencilFunctionMask      ),
    stencilFunctionReference ( stencilFunctionReference ),
    stencilMask              ( stencilMask              ),
    renderMode               ( renderMode               ),
    stencilFunction          ( stencilFunction          ),
    stencilOperationOnFail   ( stencilOperationOnFail   ),
    stencilOperationOnZFail  ( stencilOperationOnZFail  ),
    stencilOperationOnZPass  ( stencilOperationOnZPass  )
  {
  }

  int stencilFunctionMask;                          ///< The stencil function mask
  int stencilFunctionReference;                     ///< The stencil function reference
  int stencilMask;                                  ///< The stencil mask
  RenderMode::Type       renderMode:3;              ///< The render mode
  StencilFunction::Type  stencilFunction:3;         ///< The stencil function
  StencilOperation::Type stencilOperationOnFail:3;  ///< The stencil operation for stencil test fail
  StencilOperation::Type stencilOperationOnZFail:3; ///< The stencil operation for depth test fail
  StencilOperation::Type stencilOperationOnZPass:3; ///< The stencil operation for depth test pass
};


} // namespace SceneGraph

} // namespace Interanl

} // namespace Dali

#endif //DALI_INTERNAL_UPDATE_RENDERING_STENCIL_PARAMETERS_H
