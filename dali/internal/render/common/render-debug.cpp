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

// CLASS HEADER
#include <dali/internal/render/common/render-debug.h>

// EXTERNAL INCLUDES
#include <sstream>

// INTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/internal/render/common/render-item.h>
#include <dali/internal/render/common/render-list.h>
#include <dali/internal/render/common/render-instruction.h>

using Dali::Internal::SceneGraph::RenderList;

namespace Dali
{

namespace Internal
{

namespace Render
{

void PrintFrameStart( BufferIndex bufferIndex )
{
  DALI_LOG_RENDER_INFO( "RENDER START - bufferIndex: %d\n", bufferIndex );
}

void PrintFrameEnd()
{
  DALI_LOG_RENDER_INFO( "RENDER END\n\n" );
}

void PrintRenderInstruction( const SceneGraph::RenderInstruction& instruction )
{
  const char* target = (0 != instruction.mOffscreenTextureId) ? "FrameBuffer" : "Screen";

  std::stringstream debugStream;
  debugStream << "Rendering to " << target << ", View: " << *(instruction.mViewMatrix) << " Projection: " << *(instruction.mProjectionMatrix);

  if( instruction.mIsViewportSet )
  {
    debugStream << " Viewport: " << instruction.mViewport.x << "," << instruction.mViewport.y << " " << instruction.mViewport.width << "x" << instruction.mViewport.height;
  }

  if( instruction.mIsClearColorSet )
  {
    debugStream << " ClearColor: " << instruction.mClearColor;
  }

  std::string debugString( debugStream.str() );
  DALI_LOG_RENDER_INFO( "   %s\n", debugString.c_str() );
}

void PrintRenderList( const RenderList& list )
{
  unsigned int flags = list.GetFlags();

  std::stringstream debugStream;
  debugStream << "Rendering items";

  if( flags )
  {
    debugStream << " with:";

    if( flags & RenderList::DEPTH_TEST )
    {
      debugStream << " DEPTH_TEST";
    }

    if( flags & RenderList::DEPTH_WRITE )
    {
      debugStream << " DEPTH_WRITE";
    }

    if( flags & RenderList::DEPTH_CLEAR )
    {
      debugStream << " DEPTH_CLEAR";
    }

    if( flags & RenderList::STENCIL_TEST )
    {
      debugStream << " STENCIL_TEST";
    }

    if( flags & RenderList::STENCIL_WRITE )
    {
      debugStream << " STENCIL_WRITE";
    }

    if( flags & RenderList::STENCIL_CLEAR )
    {
      debugStream << " STENCIL_CLEAR";
    }
  }
  else
  {
    debugStream << " without any DEPTH_TEST, DEPTH_WRITE etc";
  }

  if( list.IsClipping() )
  {
    debugStream << ", ClippingBox: " << list.GetClippingBox().x << "," << list.GetClippingBox().y << " " << list.GetClippingBox().width << "x" << list.GetClippingBox().height;
  }

  std::string debugString( debugStream.str() );
  DALI_LOG_RENDER_INFO( "      %s\n", debugString.c_str() );
}

void PrintRenderItem( const SceneGraph::RenderItem& item )
{
  std::stringstream debugStream;
  debugStream << "Rendering item, ModelView: " << item.GetModelViewMatrix();

  std::string debugString( debugStream.str() );
  DALI_LOG_RENDER_INFO( "         %s\n", debugString.c_str() );
}

} // Render

} // Internal

} // Dali
