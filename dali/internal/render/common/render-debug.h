#ifndef __DALI_INTERNAL_RENDER_DEBUG_H__
#define __DALI_INTERNAL_RENDER_DEBUG_H__

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
#include <dali/internal/common/buffer-index.h>

// define this to print information about the items rendered each frame
#undef DALI_PRINT_RENDER_INFO

#ifdef DALI_PRINT_RENDER_INFO

#define DALI_PRINT_RENDER_START(x) Render::PrintFrameStart(x);
#define DALI_PRINT_RENDER_END() Render::PrintFrameEnd();
#define DALI_PRINT_RENDER_INSTRUCTION(x) Render::PrintRenderInstruction(x);
#define DALI_PRINT_RENDER_LIST(x) Render::PrintRenderList(x);
#define DALI_PRINT_RENDER_ITEM(x) Render::PrintRenderItem(x);

#else // DALI_PRINT_RENDER_INFO

#define DALI_PRINT_RENDER_START(x)
#define DALI_PRINT_RENDER_END()
#define DALI_PRINT_RENDER_INSTRUCTION(x)
#define DALI_PRINT_RENDER_LIST(x)
#define DALI_PRINT_RENDER_ITEM(x)

#endif // DALI_PRINT_RENDER_INFO

namespace Dali
{

namespace Internal
{

class PropertyInputImpl;

namespace SceneGraph
{
class RenderInstruction;
class RenderList;
class RenderItem;
}

namespace Render
{

/**
 * Print a debug message at the start of the render-thread.
 * @param[in] buffer The current render buffer index (previous update buffer)
 */
void PrintFrameStart( BufferIndex bufferIndex );

/**
 * Print a debug message at the end of the render-thread.
 */
void PrintFrameEnd();

/**
 * Print some information about a render-instruction.
 * @param[in] instruction The render-instruction.
 */
void PrintRenderInstruction( const SceneGraph::RenderInstruction& instruction );

/**
 * Print some information about a render-list.
 * @param[in] instruction The render-list.
 */
void PrintRenderList( const SceneGraph::RenderList& list );

/**
 * Print some information about a render-item.
 * @param[in] instruction The render-item.
 */
void PrintRenderItem( const SceneGraph::RenderItem& item );

} // Render

} // Internal

} // Dali

#endif // __DALI_INTERNAL_RENDER_DEBUG_H__
