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

// CLASS HEADER
#include <dali/internal/update/rendering/render-instruction.h>

// INTERNAL INCLUDES
#include <dali/public-api/common/constants.h> // for Color::BLACK
#include <dali/internal/update/rendering/render-tracker.h>
#include <dali/integration-api/debug.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

RenderInstruction::RenderInstruction()
: mClearColor(),
  mIsViewportSet( false ),
  mIsClearColorSet( false ),
  mIgnoreRenderToFbo( false ),
  mFrameBuffer( 0 ),
  mCamera( 0 ),
  mNextFreeRenderList( 0 )
{
  // reserve 6 lists, which is enough for three layers with opaque and transparent things on
  mRenderLists.Reserve( 6 );
}

RenderInstruction::~RenderInstruction()
{ // pointer container releases the renderlists
}

RenderList& RenderInstruction::GetNextFreeRenderList( size_t capacityRequired )
{
  // check if we have enough lists, we can only be one behind at worst
  if( mRenderLists.Count() <= mNextFreeRenderList )
  {
    mRenderLists.PushBack( new RenderList ); // Push a new empty render list
  }

  // check capacity of the list and reserve if not big enough
  if( mRenderLists[ mNextFreeRenderList ]->Capacity() < capacityRequired )
  {
    mRenderLists[ mNextFreeRenderList ]->Reserve( capacityRequired );
  }

  // return the list mNextFreeRenderList points to and increase by one
  return *mRenderLists[ mNextFreeRenderList++ ];
}

void RenderInstruction::UpdateCompleted()
{
  // lets do some housekeeping, remove any lists that were not needed because
  // application might have removed a layer permanently
  RenderListContainer::Iterator iter = mRenderLists.Begin();
  RenderListContainer::ConstIterator end = mRenderLists.End();
  for( ;iter != end; ++iter )
  {
    // tell the list to do its housekeeping
    (*iter)->ReleaseUnusedItems();
  }

  // release any extra lists
  if( mRenderLists.Count() > mNextFreeRenderList )
  {
    mRenderLists.Resize( mNextFreeRenderList );
  }
}

RenderListContainer::SizeType RenderInstruction::RenderListCount() const
{
  return mNextFreeRenderList;
}

const RenderList* RenderInstruction::GetRenderList( RenderListContainer::SizeType index ) const
{
  DALI_ASSERT_DEBUG( (index < mNextFreeRenderList ) && (index < mRenderLists.Size()) && "Renderlist index out of container bounds" );

  // Return null if the caller has passed an invalid index:
  if( index >= std::min( mNextFreeRenderList, mRenderLists.Size() ) )
  {
    return 0;
  }

  return mRenderLists[ index ];
}

void RenderInstruction::Reset( Camera*                  camera,
                               SceneGraph::FrameBuffer* frameBuffer,
                               const Viewport*          viewport,
                               const Vector4*           clearColor )
{
  mCamera = camera;
  mViewport = viewport ? *viewport : Viewport();
  mIsViewportSet = NULL != viewport;
  mClearColor = clearColor ? *clearColor : Color::BLACK;
  mIsClearColorSet = NULL != clearColor;
  mNextFreeRenderList = 0;
  mFrameBuffer = frameBuffer;

  RenderListContainer::Iterator iter = mRenderLists.Begin();
  RenderListContainer::ConstIterator end = mRenderLists.End();
  for( ;iter != end; ++iter )
  {
    // since mRenderLists is a vector of RenderLists we dont want to clear it
    // as it ends up releasing and later reallocating loads of vectors
    // reset the renderlist
    (*iter)->Reset();
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
