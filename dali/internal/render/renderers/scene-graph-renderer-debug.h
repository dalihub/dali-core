#ifndef __DALI_INTERNAL_SCENEGRAPH_RENDERER_DEBUG_H__
#define __DALI_INTERNAL_SCENEGRAPH_RENDERER_DEBUG_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

#include <dali/public-api/math/matrix.h>
#include <dali/public-api/math/rect.h>

// Uncomment to debug bounding boxes of objects
//#define DEBUG_DISPLAY_BOUNDING_BOX 1

namespace Dali
{
namespace Internal
{
class Context;

namespace SceneGraph
{
class NodeDataProvider;

void DebugBoundingBox(Context& context, Rect<float> boundingBox, const Matrix& mvp);

#if defined(DEBUG_ENABLED) && defined(DEBUG_DISPLAY_BOUNDING_BOX)
#define DEBUG_BOUNDING_BOX( context, boundingBox, mvp )                     \
  DebugBoundingBox( context, boundingBox, mvp )
#else
#define DEBUG_BOUNDING_BOX( context, boundingBox, mvp )
#endif

} // SceneGraph
} // Internal
} // Dali


#endif // __DALI_INTERNAL_SCENEGRAPH_RENDERER_DEBUG_H__
