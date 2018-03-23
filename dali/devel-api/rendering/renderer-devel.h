#ifndef DALI_RENDERER_DEVEL_H
#define DALI_RENDERER_DEVEL_H

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
 *
 */

// INTERNAL INCLUDES
#include <dali/public-api/rendering/renderer.h>

namespace Dali
{

namespace DevelRenderer
{

namespace Property
{

  enum Type
  {
    DEPTH_INDEX                 = Dali::Renderer::Property::DEPTH_INDEX,
    FACE_CULLING_MODE           = Dali::Renderer::Property::FACE_CULLING_MODE,
    BLEND_MODE                  = Dali::Renderer::Property::BLEND_MODE,
    BLEND_EQUATION_RGB          = Dali::Renderer::Property::BLEND_EQUATION_RGB,
    BLEND_EQUATION_ALPHA        = Dali::Renderer::Property::BLEND_EQUATION_ALPHA,
    BLEND_FACTOR_SRC_RGB        = Dali::Renderer::Property::BLEND_FACTOR_SRC_RGB,
    BLEND_FACTOR_DEST_RGB       = Dali::Renderer::Property::BLEND_FACTOR_DEST_RGB,
    BLEND_FACTOR_SRC_ALPHA      = Dali::Renderer::Property::BLEND_FACTOR_SRC_ALPHA,
    BLEND_FACTOR_DEST_ALPHA     = Dali::Renderer::Property::BLEND_FACTOR_DEST_ALPHA,
    BLEND_COLOR                 = Dali::Renderer::Property::BLEND_COLOR,
    BLEND_PRE_MULTIPLIED_ALPHA  = Dali::Renderer::Property::BLEND_PRE_MULTIPLIED_ALPHA,
    INDEX_RANGE_FIRST           = Dali::Renderer::Property::INDEX_RANGE_FIRST,
    INDEX_RANGE_COUNT           = Dali::Renderer::Property::INDEX_RANGE_COUNT,
    DEPTH_WRITE_MODE            = Dali::Renderer::Property::DEPTH_WRITE_MODE,
    DEPTH_FUNCTION              = Dali::Renderer::Property::DEPTH_FUNCTION,
    DEPTH_TEST_MODE             = Dali::Renderer::Property::DEPTH_TEST_MODE,
    RENDER_MODE                 = Dali::Renderer::Property::RENDER_MODE,
    STENCIL_FUNCTION            = Dali::Renderer::Property::STENCIL_FUNCTION,
    STENCIL_FUNCTION_MASK       = Dali::Renderer::Property::STENCIL_FUNCTION_MASK,
    STENCIL_FUNCTION_REFERENCE  = Dali::Renderer::Property::STENCIL_FUNCTION_REFERENCE,
    STENCIL_MASK                = Dali::Renderer::Property::STENCIL_MASK,
    STENCIL_OPERATION_ON_FAIL   = Dali::Renderer::Property::STENCIL_OPERATION_ON_FAIL,
    STENCIL_OPERATION_ON_Z_FAIL = Dali::Renderer::Property::STENCIL_OPERATION_ON_Z_FAIL,
    STENCIL_OPERATION_ON_Z_PASS = Dali::Renderer::Property::STENCIL_OPERATION_ON_Z_PASS,

    /**
     * @brief The opacity of the renderer.
     * @details Name "opacity", type Property::FLOAT.
     */
    OPACITY = STENCIL_OPERATION_ON_Z_PASS + 1,
  };
} // namespace Property

} // namespace DevelRenderer

} // namespace Dali

#endif // DALI_RENDERER_DEVEL_H
