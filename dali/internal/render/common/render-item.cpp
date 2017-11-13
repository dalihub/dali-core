/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <dali/internal/render/common/render-item.h>

// INTERNAL INCLUDES
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/common/math.h>

namespace
{
//Memory pool used to allocate new RenderItems. Memory used by this pool will be released when shutting down DALi
Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::RenderItem> gRenderItemPool;
}
namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

RenderItem* RenderItem::New()
{
  return new ( gRenderItemPool.AllocateRaw() ) RenderItem();
}

RenderItem::RenderItem()
: mModelMatrix( false ),
  mModelViewMatrix( false ),
  mSize(),
  mRenderer( NULL ),
  mNode( NULL ),
  mTextureSet( NULL ),
  mDepthIndex( 0 ),
  mIsOpaque( true )
{
}

RenderItem::~RenderItem()
{
}


ClippingBox RenderItem::CalculateViewportSpaceAABB( const int viewportWidth, const int viewportHeight ) const
{
  // Calculate extent vector of the AABB:
  const float halfActorX = mSize.x * 0.5f;
  const float halfActorY = mSize.y * 0.5f;

  // To transform the actor bounds to screen-space, We do a fast, 2D version of a matrix multiply optimized for 2D quads.
  // This reduces float multiplications from 64 (16 * 4) to 12 (4 * 3).
  // We create an array of 4 corners and directly initialize the first 3 with the matrix multiplication result of the respective corner.
  // This causes the construction of the vector arrays contents in-place for optimization.
  // We place the coords into the array in clockwise order, so we know opposite corners are always i + 2 from corner i.
  // We skip the 4th corner here as we can calculate that from the other 3, bypassing matrix multiplication.
  // Note: The below transform methods use a fast (2D) matrix multiply (only 4 multiplications are done).
  Vector2 corners[4]{ Transform2D( mModelViewMatrix, -halfActorX, -halfActorY ),
                      Transform2D( mModelViewMatrix,  halfActorX, -halfActorY ),
                      Transform2D( mModelViewMatrix,  halfActorX,  halfActorY ) };

  // As we are dealing with a rectangle, we can do a fast calculation to get the 4th corner from knowing the other 3 (even if rotated).
  corners[3] = Vector2( corners[0] + ( corners[2] - corners[1] ) );

  // Calculate the AABB:
  // We use knowledge that opposite corners will be the max/min of each other. Doing this reduces the normal 12 branching comparisons to 3.
  // The standard equivalent min/max code of the below would be:
  //       Vector2 AABBmax( std::max( corners[0].x, std::max( corners[1].x, std::max( corners[3].x, corners[2].x ) ) ),
  //                        std::max( corners[0].y, std::max( corners[1].y, std::max( corners[3].y, corners[2].y ) ) ) );
  //       Vector2 AABBmin( std::min( corners[0].x, std::min( corners[1].x, std::min( corners[3].x, corners[2].x ) ) ),
  //                        std::min( corners[0].y, std::min( corners[1].y, std::min( corners[3].y, corners[2].y ) ) ) );
  unsigned int smallestX = 0u;
  // Loop 3 times to find the index of the smallest X value.
  // Note: We deliberately do NOT unroll the code here as this hampers the compilers output.
  for( unsigned int i = 1u; i < 4u; ++i )
  {
    if( corners[i].x < corners[smallestX].x )
    {
      smallestX = i;
    }
  }

  // As we are dealing with a rectangle, we can assume opposite corners are the largest.
  // So without doing min/max branching, we can fetch the min/max values of all the remaining X/Y coords from this one index.
  Vector4 aabb( corners[smallestX].x, corners[( smallestX + 3u ) % 4].y, corners[( smallestX + 2u ) % 4].x, corners[( smallestX + 1u ) % 4].y );

  // Convert maximums to extents.
  aabb.z -= aabb.x;
  aabb.w -= aabb.y;

  // Return the AABB in screen-space pixels (x, y, width, height).
  // Note: This is a algebraic simplification of: ( viewport.x - aabb.width ) / 2 - ( ( aabb.width / 2 ) + aabb.x ) per axis.
  return ClippingBox( ( viewportWidth / 2 ) - aabb.z - aabb.x, ( viewportHeight / 2 ) - aabb.w - aabb.y, aabb.z, aabb.w );
}

void RenderItem::operator delete( void* ptr )
{
  gRenderItemPool.Free( static_cast<RenderItem*>( ptr ) );
}


} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
