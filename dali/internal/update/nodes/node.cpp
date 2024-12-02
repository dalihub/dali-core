/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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
#include <dali/public-api/common/constants.h>
#include <dali/public-api/common/dali-common.h>

#include <dali/internal/update/common/resetter-manager.h> ///< For AddInitializeResetter

namespace
{
// Memory pool used to allocate new nodes. Memory used by this pool will be released when process dies
//  or DALI library is unloaded
Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::Node>& GetNodeMemoryPool()
{
  static Dali::Internal::MemoryPoolObjectAllocator<Dali::Internal::SceneGraph::Node> gNodeMemoryPool;
  return gNodeMemoryPool;
}
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
  return new(GetNodeMemoryPool().AllocateRawThreadSafe()) Node;
}

void Node::Delete(Node* node)
{
  // check we have a node not a derived node
  if(!node->mIsLayer && !node->mIsCamera)
  {
    // Manually call the destructor
    node->~Node();

    // Mark the memory it used as free in the memory pool
    GetNodeMemoryPool().FreeThreadSafe(node);
  }
  else
  {
    // not in the pool, just delete it.
    delete node;
  }
}

void Node::ResetMemoryPool()
{
  GetNodeMemoryPool().ResetMemoryPool();
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
  mUseTextureUpdateArea(false)
{
#ifdef DEBUG_ENABLED
  gNodeCount++;
#endif
}

Node::~Node()
{
  if(mTransformManagerData.Id() != INVALID_TRANSFORM_ID)
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

  // Set TransformId after initialize done.
  mTransformManagerData.mId = createdTransformId;
}

void Node::SetRoot(bool isRoot)
{
  DALI_ASSERT_DEBUG(!isRoot || mParent == NULL); // Root nodes cannot have a parent

  mIsRoot = isRoot;
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

void Node::DisconnectChild(BufferIndex updateBufferIndex, Node& childNode)
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

  found->RecursiveDisconnectFromSceneGraph(updateBufferIndex);
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
}

void Node::RemoveRenderer(const RendererKey& renderer)
{
  RendererContainer::SizeType rendererCount(mRenderers.Size());
  for(RendererContainer::SizeType i = 0; i < rendererCount; ++i)
  {
    if(mRenderers[i] == renderer)
    {
      renderer->DetachFromNodeDataProvider(*this);

      SetUpdated(true);
      mRenderers.Erase(mRenderers.Begin() + i);
      return;
    }
  }
}

void Node::SetCacheRenderer(const RendererKey& renderer)
{
  if(DALI_UNLIKELY(mCacheRenderer))
  {
    mCacheRenderer->DetachFromNodeDataProvider(*this);
  }
  SetUpdated(true);
  mCacheRenderer = renderer;
}

void Node::RemoveCacheRenderer()
{
  if(DALI_LIKELY(mCacheRenderer))
  {
    mCacheRenderer->DetachFromNodeDataProvider(*this);
    SetUpdated(true);
    mCacheRenderer = RendererKey{};
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

void Node::ResetDirtyFlags(BufferIndex updateBufferIndex)
{
  mDirtyFlags = NodePropertyFlags::NOTHING;

  mUpdateAreaChanged = false;
}

void Node::SetParent(Node& parentNode)
{
  DALI_ASSERT_ALWAYS(this != &parentNode);
  DALI_ASSERT_ALWAYS(!mIsRoot);
  DALI_ASSERT_ALWAYS(mParent == nullptr);

  mParent = &parentNode;

  if(mTransformManagerData.Id() != INVALID_TRANSFORM_ID)
  {
    mTransformManagerData.Manager()->SetParent(mTransformManagerData.Id(), parentNode.GetTransformId());
  }
}

void Node::RecursiveDisconnectFromSceneGraph(BufferIndex updateBufferIndex)
{
  DALI_ASSERT_ALWAYS(!mIsRoot);
  DALI_ASSERT_ALWAYS(mParent != nullptr);

  const NodeIter endIter = mChildren.End();
  for(NodeIter iter = mChildren.Begin(); iter != endIter; ++iter)
  {
    (*iter)->RecursiveDisconnectFromSceneGraph(updateBufferIndex);
  }

  // Animators, Constraints etc. should be disconnected from the child's properties.
  PropertyOwner::DisconnectFromSceneGraph(updateBufferIndex);

  // Remove back-pointer to parent
  mParent = nullptr;

  // Remove all child pointers
  mChildren.Clear();

  if(mTransformManagerData.Id() != INVALID_TRANSFORM_ID)
  {
    mTransformManagerData.Manager()->SetParent(mTransformManagerData.Id(), INVALID_TRANSFORM_ID);
  }
}

uint32_t Node::GetMemoryPoolCapacity()
{
  return GetNodeMemoryPool().GetCapacity();
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
