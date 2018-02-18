#ifndef __DALI_INTERNAL_RENDER_DEBUG_H__
#define __DALI_INTERNAL_RENDER_DEBUG_H__

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
#include <dali/internal/common/buffer-index.h>

// define this to print information about the items rendered each frame
#undef DALI_PRINT_RENDER_INFO

#ifdef DALI_PRINT_RENDER_INFO

#define DALI_PRINT_RENDER_START(x) Render::PrintFrameStart(x);
#define DALI_PRINT_RENDER_END() Render::PrintFrameEnd();
#define DALI_PRINT_RENDER_INSTRUCTION(x,index) Render::PrintRenderInstruction(x,index);
#define DALI_PRINT_RENDER_LIST(x) Render::PrintRenderList(x);
#define DALI_PRINT_RENDER_ITEM(x) Render::PrintRenderItem(x);

#else // DALI_PRINT_RENDER_INFO

#define DALI_PRINT_RENDER_START(x)
#define DALI_PRINT_RENDER_END()
#define DALI_PRINT_RENDER_INSTRUCTION(x,index)
#define DALI_PRINT_RENDER_LIST(x)
#define DALI_PRINT_RENDER_ITEM(x)

#endif // DALI_PRINT_RENDER_INFO

#undef DALI_PRINT_RENDERERS

// Turn this on to see a snapshot of # renderers every 2 seconds
//#define DALI_PRINT_RENDERERS 1

#ifdef DALI_PRINT_RENDERERS
#define DALI_PRINT_RENDERER_COUNT(x, y)  Render::PrintRendererCount(x, y)
#else // DALI_PRINT_RENDERERS
#define DALI_PRINT_RENDERER_COUNT(x, y)
#endif // DALI_PRINT_RENDERERS


namespace Dali
{

namespace Internal
{

class PropertyInputImpl;

namespace SceneGraph
{
class Node;
class RenderInstruction;
struct RenderList;
struct RenderItem;
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
 * @param[in] index to use
 */
void PrintRenderInstruction( const SceneGraph::RenderInstruction& instruction, BufferIndex index );

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

/**
 * Print the number of image renderers
 * @param[in] frameCount The frame counter
 * @param[in] rendererCount The number of image renderers
 */
void PrintRendererCount( unsigned int frameCount, unsigned int rendererCount );

/**
 * Recursively dumps a Node tree.
 * @param[in] node The starting node to dump from
 * @param[in] indent Optional. Leave unset for default indent (used internally while recursing)
 */
void DumpNode( const SceneGraph::Node* node, unsigned int indent = 0 );

} // Render

} // Internal

} // Dali

#endif // __DALI_INTERNAL_RENDER_DEBUG_H__
