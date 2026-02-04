/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
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
#include <dali/internal/update/nodes/node.h>

// INTERNAL INCLUDES
#include <dali/internal/common/internal-constants.h>
#include <dali/internal/common/memory-pool-object-allocator.h>
#include <dali/internal/update/common/discard-queue.h>
#include <dali/internal/update/common/scene-graph-memory-pool-collection.h>
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/dali-common.h>

#include <dali/internal/update/common/resetter-manager.h> ///< For AddInitializeResetter

namespace
{
Dali::Internal::SceneGraph::MemoryPoolCollection*                                 gMemoryPoolCollection = nullptr;
static constexpr Dali::Internal::SceneGraph::MemoryPoolCollection::MemoryPoolType gMemoryPoolType       = Dali::Internal::SceneGraph::MemoryPoolCollection::MemoryPoolType::NODE;

#ifdef DEBUG_ENABLED
// keep track of nodes alive, to ensure we have 0 when the process exits or DALi library is unloaded
int32_t gNodeCount = 0;

// Called when the process is about to exit, Node count should be zero at this point.
void __attribute__((destructor)) ShutDown(void)
{
  DALI_ASSERT_DEBUG((gNodeCount == 0) && "Node memory leak");
}
#endif
} // Unnamed namespace

namespace Dali
{
namespace Internal
{
namespace SceneGraph
{
const ColorMode Node::DEFAULT_COLOR_MODE(USE_OWN_MULTIPLY_PARENT_ALPHA);

uint32_t Node::mNodeCounter = 0; ///< A counter to provide unique node ids, up-to 4 billion

Node* Node::New()
{
  DALI_ASSERT_DEBUG(gMemoryPoolCollection && "Node::RegisterMemoryPoolCollection not called!");
  return new(gMemoryPoolCollection->AllocateRawThreadSafe(gMemoryPoolType)) Node;
}

void Node::Delete(Node* node)
{
  // check we have a node not a derived node
  if(!node->mIsLayer && !node->mIsCamera)
  {
    // Manually call the destructor
    node->~Node();

    DALI_ASSERT_DEBUG(gMemoryPoolCollection && "Node::RegisterMemoryPoolCollection not called!");
    // Mark the memory it used as free in the memory pool
    gMemoryPoolCollection->FreeThreadSafe(gMemoryPoolType, static_cast<void*>(node));
  }
  else
  {
    // not in the pool, just delete it.
    delete node;
  }
}

void Node::RegisterMemoryPoolCollection(MemoryPoolCollection& memoryPoolCollection)
{
  gMemoryPoolCollection = &memoryPoolCollection;
}

void Node::UnregisterMemoryPoolCollection()
{
  gMemoryPoolCollection = nullptr;
}

Node::Node()
: mTransformManagerData(),                                                      // Initialized to use invalid id by default
  mOrientation(),                                                               // Initialized to identity by default
  mWorldPosition(TRANSFORM_PROPERTY_WORLD_POSITION, Vector3(0.0f, 0.0f, 0.0f)), // Zero initialized by default
  mWorldScale(TRANSFORM_PROPERTY_WORLD_SCALE, Vector3(1.0f, 1.0f, 1.0f)),
  mWorldOrientation(), // Initialized to identity by default
  mWorldMatrix(),
  mVisible(true),
  mCulled(false),
  mColor(Color::WHITE),
  mWorldColor(Color::WHITE),
  mUpdateAreaHint(Vector4::ZERO),
  mClippingSortModifier(0u),
  mId(++mNodeCounter),
  mParent(nullptr),
  mChildren(),
  mClippingDepth(0u),
  mScissorDepth(0u),
  mDepthIndex(0u),
  mDirtyFlags(NodePropertyFlags::ALL),
  mDrawMode(DrawMode::NORMAL),
  mColorMode(DEFAULT_COLOR_MODE),
  mClippingMode(ClippingMode::DISABLED),
  mIsRoot(false),
  mIsLayer(false),
  mIsCamera(false),
  mPositionUsesAnchorPoint(true),
  mTransparent(false),
  mUpdateAreaChanged(false),
  mUpdateAreaUseSize(true),
  mUseTextureUpdateArea(false)
{
#ifdef DEBUG_ENABLED
  gNodeCount++;
#endif
}

Node::~Node()
{
  if(DALI_LIKELY(TransformManager::IsValidTransformId(mTransformManagerData.Id())))
  {
    mTransformManagerData.Manager()->RemoveTransform(mTransformManagerData.Id());
  }

#ifdef DEBUG_ENABLED
  gNodeCount--;
#endif
}

void Node::OnDestroy()
{
  // Animators, Constraints etc. should be disconnected from the child's properties.
  PropertyOwner::Destroy();
}

uint32_t Node::GetId() const
{
  return mId;
}

void Node::CreateTransform(SceneGraph::TransformManager* transformManager)
{
  // Create a new transform
  mTransformManagerData.mManager = transformManager;
  TransformId createdTransformId = transformManager->CreateTransform();

  // Set whether the position should use the anchor point
  transformManager->SetPositionUsesAnchorPoint(createdTransformId, mPositionUsesAnchorPoint);
  if(DALI_UNLIKELY(mIsRoot))
  {
    transformManager->SetParent(createdTransformId, PARENT_OF_ROOT_NODE_TRANSFORM_ID);
  }

  // Set TransformId after initialize done.
  mTransformManagerData.mId = createdTransformId;
}

void Node::SetRoot(bool isRoot)
{
  DALI_ASSERT_DEBUG(!isRoot || mParent == NULL); // Root nodes cannot have a parent

  mIsRoot = isRoot;
  if(DALI_LIKELY(mIsRoot && TransformManager::IsValidTransformId(mTransformManagerData.Id())))
  {
    mTransformManagerData.Manager()->SetParent(mTransformManagerData.Id(), PARENT_OF_ROOT_NODE_TRANSFORM_ID);
  }
}

bool Node::IsAnimationPossible() const
{
  return mIsConnectedToSceneGraph;
}

void Node::AddInitializeResetter(ResetterManager& manager) const
{
  manager.AddNodeResetter(*this);
}

void Node::ConnectChild(Node* childNode)
{
  DALI_ASSERT_ALWAYS(this != childNode);
  DALI_ASSERT_ALWAYS(IsRoot() || nullptr != mParent);                            // Parent should be connected first
  DALI_ASSERT_ALWAYS(!childNode->IsRoot() && nullptr == childNode->GetParent()); // Child should be disconnected

  childNode->SetParent(*this);

  // Everything should be reinherited when reconnected to scene-graph
  childNode->SetAllDirtyFlags();

  // Make sure the partial rendering data is invalidated.
  childNode->GetPartialRenderingData().MakeExpired();

  // Add the node to the end of the child list.
  mChildren.PushBack(childNode);

  // Inform property observers of new connection
  childNode->ConnectToSceneGraph();
}

void Node::DisconnectChild(Node& childNode)
{
  DALI_ASSERT_ALWAYS(this != &childNode);
  DALI_ASSERT_ALWAYS(childNode.GetParent() == this);

  // Find the childNode and remove it
  Node* found(nullptr);

  const NodeIter endIter = mChildren.End();
  for(NodeIter iter = mChildren.Begin(); iter != endIter; ++iter)
  {
    Node* current = *iter;
    if(current == &childNode)
    {
      found = current;
      mChildren.Erase(iter); // order matters here
      break;                 // iter is no longer valid
    }
  }
  DALI_ASSERT_ALWAYS(nullptr != found);

  found->RecursiveDisconnectFromSceneGraph();
}

void Node::AddRenderer(const RendererKey& renderer)
{
  // If it is the first renderer added, make sure the world transform will be calculated
  // in the next update as world transform is not computed if node has no renderers.
  if(mRenderers.Empty())
  {
    mDirtyFlags |= NodePropertyFlags::TRANSFORM;
  }
  else
  {
    // Check that it has not been already added.
    for(auto&& existingRenderer : mRenderers)
    {
      if(existingRenderer == renderer)
      {
        // Renderer is already in the list.
        return;
      }
    }
  }

  SetUpdated(true);

  mRenderers.PushBack(renderer);

  renderer->AttachToNode(*this);
}

void Node::RemoveRenderer(const RendererKey& renderer)
{
  RendererContainer::SizeType rendererCount(mRenderers.Size());
  for(RendererContainer::SizeType i = 0; i < rendererCount; ++i)
  {
    if(mRenderers[i] == renderer)
    {
      renderer->DetachFromNode(*this);

      SetUpdated(true);
      mRenderers.Erase(mRenderers.Begin() + i);
      return;
    }
  }
}

void Node::AddCacheRenderer(const RendererKey& renderer)
{
  for(auto&& existingRenderer : mCacheRenderers)
  {
    if(existingRenderer == renderer)
    {
      return;
    }
  }

  SetUpdated(true);
  SetDirtyFlag(NodePropertyFlags::COLOR);

  mCacheRenderers.PushBack(renderer);

  renderer->AttachToNode(*this);
}

void Node::RemoveCacheRenderer(const RendererKey& renderer)
{
  RendererContainer::SizeType rendererCount(mCacheRenderers.Size());
  for(RendererContainer::SizeType i = 0; i < rendererCount; ++i)
  {
    if(mCacheRenderers[i] == renderer)
    {
      renderer->DetachFromNode(*this);

      SetUpdated(true);
      SetDirtyFlag(NodePropertyFlags::COLOR);
      mCacheRenderers.Erase(mCacheRenderers.Begin() + i);
      return;
    }
  }
}

NodePropertyFlags Node::GetDirtyFlags() const
{
  // get initial dirty flags, they are reset ResetDefaultProperties, but setters may have made the node dirty already
  NodePropertyFlags flags = mDirtyFlags;

  // Check whether the visible property has changed
  if(!mVisible.IsClean())
  {
    flags |= NodePropertyFlags::VISIBLE;
  }

  // Check whether the color property has changed
  if(!mColor.IsClean())
  {
    flags |= NodePropertyFlags::COLOR;
  }

  return flags;
}

NodePropertyFlags Node::GetInheritedDirtyFlags(NodePropertyFlags parentFlags) const
{
  // Size is not inherited. VisibleFlag is inherited
  static const NodePropertyFlags InheritedDirtyFlags = NodePropertyFlags::TRANSFORM | NodePropertyFlags::VISIBLE | NodePropertyFlags::COLOR;
  using UnderlyingType                               = typename std::underlying_type<NodePropertyFlags>::type;

  return static_cast<NodePropertyFlags>(static_cast<UnderlyingType>(mDirtyFlags) |
                                        (static_cast<UnderlyingType>(parentFlags) & static_cast<UnderlyingType>(InheritedDirtyFlags)));
}

void Node::SetParent(Node& parentNode)
{
  DALI_ASSERT_ALWAYS(this != &parentNode);
  DALI_ASSERT_ALWAYS(!mIsRoot);
  DALI_ASSERT_ALWAYS(mParent == nullptr);

  mParent = &parentNode;

  if(DALI_LIKELY(TransformManager::IsValidTransformId(mTransformManagerData.Id())))
  {
    mTransformManagerData.Manager()->SetParent(mTransformManagerData.Id(), parentNode.GetTransformId());
  }
}

void Node::RecursiveDisconnectFromSceneGraph()
{
  DALI_ASSERT_ALWAYS(!mIsRoot);
  DALI_ASSERT_ALWAYS(mParent != nullptr);

  const NodeIter endIter = mChildren.End();
  for(NodeIter iter = mChildren.Begin(); iter != endIter; ++iter)
  {
    (*iter)->RecursiveDisconnectFromSceneGraph();
  }

  // Animators, Constraints etc. should be disconnected from the child's properties.
  PropertyOwner::DisconnectFromSceneGraph();

  // Remove back-pointer to parent
  mParent = nullptr;

  // Remove all child pointers
  mChildren.Clear();

  if(DALI_LIKELY(TransformManager::IsValidTransformId(mTransformManagerData.Id())))
  {
    mTransformManagerData.Manager()->SetParent(mTransformManagerData.Id(), PARENT_OF_OFF_SCENE_TRANSFORM_ID);
  }
}

void Node::UpdatePartialRenderingData(bool isLayer3d, bool canSkipInfomationUpdate)
{
  if(mPartialRenderingData.mUpdateDecay == PartialRenderingData::Decay::UPDATED_CURRENT_FRAME)
  {
    // Fast-out if we're already updated this frame
    return;
  }

  if(Updated())
  {
    // If the node was updated, then mark the partial rendering data as expired
    // So we can skip data comparision.
    mPartialRenderingData.MakeExpired();
  }

  // If we can ensure to skip node info update, just change decay
  if(canSkipInfomationUpdate && mPartialRenderingData.MakeUpdatedCurrentFrame())
  {
    return;
  }

  const Vector4& worldColor = GetWorldColor();

  // TODO : Can't we get modelMatrix and size as const l-value at onces?
  const auto&    transformId = mTransformManagerData.Id();
  const Matrix&  modelMatrix = DALI_LIKELY(TransformManager::IsValidTransformId(transformId)) ? mWorldMatrix.Get() : Matrix::IDENTITY;
  const Vector3& size        = DALI_LIKELY(TransformManager::IsValidTransformId(transformId)) ? mSize.Get() : Vector3::ZERO;

  const Vector4& updatedPositionSize = CalculateNodeUpdateArea(isLayer3d, modelMatrix, size);

  mPartialRenderingData.UpdateNodeInfomations(modelMatrix, worldColor, updatedPositionSize, size);
}

Vector4 Node::CalculateNodeUpdateArea(bool isLayer3d, const Matrix& nodeWorldMatrix, const Vector3& nodeSize) const
{
  if(DALI_LIKELY(mUpdateAreaUseSize))
  {
    if(isLayer3d)
    {
      return Vector4::ZERO;
    }
    // RenderItem::CalculateViewportSpaceAABB cannot cope with z transform
    // I don't use item.mModelMatrix.GetTransformComponents() for z transform, would be too slow
    // Instead, use [8] and [9] of world matrix, which are z-axis's x and z-axis's y value.
    if(EqualsZero(nodeWorldMatrix.AsFloat()[8]) && EqualsZero(nodeWorldMatrix.AsFloat()[9]))
    {
      return Vector4(0.0f, 0.0f, nodeSize.width, nodeSize.height);
    }
    // Keep nodeUpdateArea as Vector4::ZERO.
    return Vector4::ZERO;
  }
  else
  {
    return GetUpdateAreaHint();
  }
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
