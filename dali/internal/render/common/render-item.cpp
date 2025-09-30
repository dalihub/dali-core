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

// CLASS HEADER
#include <dali/internal/render/common/render-item.h>

// INTERNAL INCLUDES
#include <dali/internal/common/math.h>
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/render/renderers/render-renderer.h>
#include <dali/internal/update/common/scene-graph-memory-pool-collection.h>

namespace
{
Dali::Internal::SceneGraph::MemoryPoolCollection*                                 gMemoryPoolCollection = nullptr;
static constexpr Dali::Internal::SceneGraph::MemoryPoolCollection::MemoryPoolType gMemoryPoolType       = Dali::Internal::SceneGraph::MemoryPoolCollection::MemoryPoolType::RENDER_ITEM;
} // namespace

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
RenderItemKey RenderItem::NewKey()
{
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "RenderItem::RegisterMemoryPoolCollection not called!");
  void* ptr = gMemoryPoolCollection->AllocateRaw(gMemoryPoolType);
  auto  key = gMemoryPoolCollection->GetKeyFromPtr(gMemoryPoolType, ptr);
  new(ptr) RenderItem();
  return RenderItemKey(key);
}

void RenderItem::RegisterMemoryPoolCollection(MemoryPoolCollection& memoryPoolCollection)
{
  gMemoryPoolCollection = &memoryPoolCollection;
}

void RenderItem::UnregisterMemoryPoolCollection()
{
  gMemoryPoolCollection = nullptr;
}

RenderItem::RenderItem()
: mModelViewMatrix(false),
  mRenderer{},
  mNode(nullptr),
  mTextureSet(nullptr),
  mDepthIndex(0),
  mIsOpaque(true),
  mIsUpdated(false)
{
}

RenderItem::~RenderItem() = default;

void RenderItem::operator delete(void* ptr)
{
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "RenderItem::RegisterMemoryPoolCollection not called!");
  gMemoryPoolCollection->Free(gMemoryPoolType, ptr);
}

RenderItem* RenderItem::Get(RenderItemKey::KeyType key)
{
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "RenderItem::RegisterMemoryPoolCollection not called!");
  return static_cast<RenderItem*>(gMemoryPoolCollection->GetPtrFromKey(gMemoryPoolType, key));
}

ClippingBox RenderItem::CalculateTransformSpaceAABB(const Matrix& transformMatrix, const Vector3& position, const Vector3& size)
{
  // Calculate extent vector of the AABB:
  const float halfActorX = size.x * 0.5f;
  const float halfActorY = size.y * 0.5f;

  // To transform the actor bounds to the transformed space, We do a fast, 2D version of a matrix multiply optimized for 2D quads.
  // This reduces float multiplications from 64 (16 * 4) to 12 (4 * 3).
  // We create an array of 4 corners and directly initialize the first 3 with the matrix multiplication result of the respective corner.
  // This causes the construction of the vector arrays contents in-place for optimization.
  // We place the coords into the array in clockwise order, so we know opposite corners are always i + 2 from corner i.
  // We skip the 4th corner here as we can calculate that from the other 3, bypassing matrix multiplication.
  // Note: The below transform methods use a fast (2D) matrix multiply (only 4 multiplications are done).
  Vector2 corners[4]{Transform2D(transformMatrix, -halfActorX + position.x, -halfActorY + position.y),
                     Transform2D(transformMatrix, halfActorX + position.x, -halfActorY + position.y),
                     Transform2D(transformMatrix, halfActorX + position.x, halfActorY + position.y)};

  // As we are dealing with a rectangle, we can do a fast calculation to get the 4th corner from knowing the other 3 (even if rotated).
  corners[3] = Vector2(corners[0] + (corners[2] - corners[1]));

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
  for(unsigned int i = 1u; i < 4u; ++i)
  {
    if(corners[i].x < corners[smallestX].x)
    {
      smallestX = i;
    }
  }

  // As we are dealing with a rectangle, we can assume opposite corners are the largest.
  // So without doing min/max branching, we can fetch the min/max values of all the remaining X/Y coords from this one index.
  Vector4 aabb(corners[smallestX].x, corners[(smallestX + 3u) % 4].y, corners[(smallestX + 2u) % 4].x, corners[(smallestX + 1u) % 4].y);

  // Round outwards from center
  int x = static_cast<int>(floor(aabb.x));
  int y = static_cast<int>(floor(aabb.y));
  int z = static_cast<int>(ceilf(aabb.z));
  int w = static_cast<int>(ceilf(aabb.w));

  return ClippingBox(x, y, z - x, fabsf(w - y));
}

ClippingBox RenderItem::CalculateViewportSpaceAABB(const Matrix& modelViewMatrix, const Vector3& position, const Vector3& size, const int viewportWidth, const int viewportHeight, const Vector2& scaleFactor)
{
  // Calculate extent vector of the AABB:
  const float halfActorX = size.x * 0.5f;
  const float halfActorY = size.y * 0.5f;

  // To transform the actor bounds to screen-space, We do a fast, 2D version of a matrix multiply optimized for 2D quads.
  // This reduces float multiplications from 64 (16 * 4) to 12 (4 * 3).
  // We create an array of 4 corners and directly initialize the first 3 with the matrix multiplication result of the respective corner.
  // This causes the construction of the vector arrays contents in-place for optimization.
  // We place the coords into the array in clockwise order, so we know opposite corners are always i + 2 from corner i.
  // We skip the 4th corner here as we can calculate that from the other 3, bypassing matrix multiplication.
  // Note: The below transform methods use a fast (2D) matrix multiply (only 4 multiplications are done).
  Vector2 corners[4]{Transform2D(modelViewMatrix, -halfActorX + position.x, -halfActorY + position.y) * scaleFactor,
                     Transform2D(modelViewMatrix, halfActorX + position.x, -halfActorY + position.y) * scaleFactor,
                     Transform2D(modelViewMatrix, halfActorX + position.x, halfActorY + position.y) * scaleFactor};

  // As we are dealing with a rectangle, we can do a fast calculation to get the 4th corner from knowing the other 3 (even if rotated).
  corners[3] = Vector2(corners[0] + (corners[2] - corners[1]));

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
  for(unsigned int i = 1u; i < 4u; ++i)
  {
    if(corners[i].x < corners[smallestX].x)
    {
      smallestX = i;
    }
  }

  // As we are dealing with a rectangle, we can assume opposite corners are the largest.
  // So without doing min/max branching, we can fetch the min/max values of all the remaining X/Y coords from this one index.
  Vector4 aabb(corners[smallestX].x, corners[(smallestX + 3u) % 4].y, corners[(smallestX + 2u) % 4].x, corners[(smallestX + 1u) % 4].y);

  // Return the AABB in screen-space pixels (x, y, width, height).
  Vector4 aabbInScreen(static_cast<float>(viewportWidth) * 0.5f - aabb.z,
                       static_cast<float>(viewportHeight) * 0.5f - aabb.w,
                       static_cast<float>(viewportWidth) * 0.5f - aabb.x,
                       static_cast<float>(viewportHeight) * 0.5f - aabb.y);

  int x = static_cast<int>(floor(aabbInScreen.x));
  int y = static_cast<int>(floor(aabbInScreen.y));
  int z = static_cast<int>(roundf(aabbInScreen.z));
  int w = static_cast<int>(roundf(aabbInScreen.w));

  return ClippingBox(x, y, z - x, w - y);
}

bool RenderItem::UsesDepthBuffer(bool depthTestEnabled)
{
  const DepthWriteMode::Type depthWriteMode = mRenderer->GetDepthWriteMode();
  const DepthTestMode::Type  depthTestMode  = mRenderer->GetDepthTestMode();

  const bool enableDepthWrite = ((depthWriteMode == DepthWriteMode::AUTO) && depthTestEnabled && mIsOpaque) ||
                                (depthWriteMode == DepthWriteMode::ON);
  const bool enableDepthTest = ((depthTestMode == DepthTestMode::AUTO) && depthTestEnabled) ||
                               (depthTestMode == DepthTestMode::ON);
  return enableDepthTest || enableDepthWrite;
}

bool RenderItem::UsesStencilBuffer() const
{
  RenderMode::Type renderMode  = RenderMode::AUTO;
  bool             usesStencil = false;
  if(mRenderer)
  {
    renderMode = mRenderer->GetRenderMode();
  }

  switch(renderMode)
  {
    case RenderMode::AUTO:
    {
      if(mNode->GetClippingId() != 0u) // If there is a clipping node, then we are either reading/writing the stencil buffer.
      {
        usesStencil = true;
      }
      break;
    }

    case RenderMode::NONE:
    case RenderMode::COLOR:
    {
      usesStencil = false;
      break;
    }

    case RenderMode::STENCIL:
    case RenderMode::COLOR_STENCIL:
    {
      usesStencil = true;
      break;
    }
  }
  return usesStencil;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
