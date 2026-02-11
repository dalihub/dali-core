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
#include <dali/internal/update/manager/update-proxy-impl.h>

// INTERNAL INCLUDES
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/manager/update-proxy-property-modifier.h>

namespace Dali
{
namespace Internal
{
UpdateProxy::UpdateProxy(SceneGraph::UpdateManager& updateManager, SceneGraph::TransformManager& transformManager, SceneGraphTravelerInterfacePtr traveler)
: mLastCachedIdNodePair({0u, nullptr}),
  mDirtyNodes(),
  mCurrentBufferIndex(0u),
  mUpdateManager(updateManager),
  mTransformManager(transformManager),
  mSceneGraphTraveler(traveler),
  mPropertyModifier(nullptr)
{
}

UpdateProxy::~UpdateProxy() = default;

bool UpdateProxy::GetPosition(uint32_t id, Vector3& position) const
{
  bool                    success = false;
  const SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    const SceneGraph::TransformManager& transformManager = mTransformManager; // To ensure we call the const getter
    position                                             = transformManager.GetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_POSITION);
    success                                              = true;
  }
  return success;
}

bool UpdateProxy::SetPosition(uint32_t id, const Vector3& position)
{
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    mTransformManager.SetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_POSITION, position);
    success = true;
  }
  return success;
}

bool UpdateProxy::BakePosition(uint32_t id, const Vector3& position)
{
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    mTransformManager.BakeVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_POSITION, position);
    success = true;
  }
  return success;
}

bool UpdateProxy::GetOrientation(uint32_t id, Quaternion& orientation) const
{
  bool                    success = false;
  const SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    const SceneGraph::TransformManager& transformManager = mTransformManager; // To ensure we call the const getter

    orientation = transformManager.GetQuaternionPropertyValue(node->GetTransformId());
    success     = true;
  }
  return success;
}

bool UpdateProxy::SetOrientation(uint32_t id, const Quaternion& orientation)
{
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    mTransformManager.SetQuaternionPropertyValue(node->GetTransformId(), orientation);
    success = true;
  }
  return success;
}

bool UpdateProxy::BakeOrientation(uint32_t id, const Quaternion& orientation)
{
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    mTransformManager.BakeQuaternionPropertyValue(node->GetTransformId(), orientation);
    success = true;
  }
  return success;
}

bool UpdateProxy::GetSize(uint32_t id, Vector3& size) const
{
  bool                    success = false;
  const SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    const SceneGraph::TransformManager& transformManager = mTransformManager; // To ensure we call the const getter
    size                                                 = transformManager.GetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_SIZE);
    success                                              = true;
  }
  return success;
}

bool UpdateProxy::SetSize(uint32_t id, const Vector3& size)
{
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    mTransformManager.SetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_SIZE, size);
    success = true;
  }
  return success;
}

bool UpdateProxy::BakeSize(uint32_t id, const Vector3& size)
{
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    mTransformManager.BakeVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_SIZE, size);
    success = true;
  }
  return success;
}

bool UpdateProxy::GetPositionAndSize(uint32_t id, Vector3& position, Vector3& size) const
{
  bool                    success = false;
  const SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    const SceneGraph::TransformManager& transformManager = mTransformManager; // To ensure we call the const getter
    position                                             = transformManager.GetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_POSITION);
    size                                                 = transformManager.GetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_SIZE);
    success                                              = true;
  }
  return success;
}

bool UpdateProxy::GetWorldPositionScaleAndSize(uint32_t id, Vector3& position, Vector3& scale, Vector3& size) const
{
  bool                    success = false;
  const SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    const SceneGraph::TransformManager& transformManager = mTransformManager; // To ensure we call the const getter
    const Matrix&                       worldMatrix      = transformManager.GetWorldMatrix(node->GetTransformId());

    Quaternion orientation;
    worldMatrix.GetTransformComponents(position, orientation, scale);

    size    = transformManager.GetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_SIZE);
    success = true;
  }
  return success;
}

bool UpdateProxy::GetWorldTransformAndSize(uint32_t id, Vector3& position, Vector3& scale, Quaternion& orientation, Vector3& size) const
{
  bool                    success = false;
  const SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    const SceneGraph::TransformManager& transformManager = mTransformManager; // To ensure we call the const getter
    const Matrix&                       worldMatrix      = transformManager.GetWorldMatrix(node->GetTransformId());

    worldMatrix.GetTransformComponents(position, orientation, scale);
    size    = transformManager.GetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_SIZE);
    success = true;
  }
  return success;
}

bool UpdateProxy::GetScale(uint32_t id, Vector3& scale) const
{
  bool                    success = false;
  const SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    const SceneGraph::TransformManager& transformManager = mTransformManager; // To ensure we call the const getter
    scale                                                = transformManager.GetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_SCALE);
    success                                              = true;
  }

  return success;
}

bool UpdateProxy::SetScale(uint32_t id, const Vector3& scale)
{
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    mTransformManager.SetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_SCALE, scale);
    success = true;
  }
  return success;
}

bool UpdateProxy::BakeScale(uint32_t id, const Vector3& scale)
{
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    mTransformManager.BakeVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_SCALE, scale);
    success = true;
  }
  return success;
}

bool UpdateProxy::GetColor(uint32_t id, Vector4& color) const
{
  bool                    success = false;
  const SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    color   = node->mColor.Get();
    success = true;
  }

  return success;
}

bool UpdateProxy::SetColor(uint32_t id, const Vector4& color)
{
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    node->mColor.Set(color);
    node->SetDirtyFlag(SceneGraph::NodePropertyFlags::COLOR);
    mDirtyNodes.push_back(id);
    AddResetter(*node, node->mColor);
    success = true;
  }
  return success;
}

bool UpdateProxy::BakeColor(uint32_t id, const Vector4& color)
{
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    node->mColor.Bake(color);
    success = true;
  }
  return success;
}

void UpdateProxy::NodeHierarchyChanged()
{
  mLastCachedIdNodePair = {0u, nullptr};
  mPropertyModifier.reset();
}

void UpdateProxy::Notify(Dali::UpdateProxy::NotifySyncPoint syncPoint)
{
  mSyncPoints.push_back(syncPoint);
}

Dali::UpdateProxy::NotifySyncPoint UpdateProxy::PopSyncPoint()
{
  if(!mSyncPoints.empty())
  {
    auto syncPoint = mSyncPoints.front();
    mSyncPoints.pop_front();
    return syncPoint;
  }

  return Dali::UpdateProxy::INVALID_SYNC;
}

SceneGraph::Node* UpdateProxy::GetNodeWithId(uint32_t id) const
{
  SceneGraph::Node* node = nullptr;

  // Cache the last accessed node so we don't have to traverse
  if(mLastCachedIdNodePair.node && mLastCachedIdNodePair.id == id)
  {
    node = mLastCachedIdNodePair.node;
  }
  else
  {
    node = mSceneGraphTraveler->FindNode(id);

    if(node)
    {
      mLastCachedIdNodePair = {id, node};
    }
  }

  return node;
}

void UpdateProxy::AddResetter(SceneGraph::Node& node, SceneGraph::PropertyBase& propertyBase)
{
  if(!mPropertyModifier)
  {
    mPropertyModifier = PropertyModifierPtr(new PropertyModifier(mUpdateManager));
  }
  mPropertyModifier->AddResetter(node, propertyBase);
}

void UpdateProxy::AddNodeResetters()
{
  for(auto&& id : mDirtyNodes)
  {
    SceneGraph::Node* node = GetNodeWithId(id);
    if(node)
    {
      mUpdateManager.AddNodeResetter(*node);
    }
  }
}

bool UpdateProxy::GetUpdateArea(uint32_t id, Vector4& updateArea) const
{
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    updateArea = node->GetUpdateAreaHint();
    success    = true;
  }
  return success;
}

bool UpdateProxy::SetUpdateArea(uint32_t id, const Vector4& updateArea)
{
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    node->SetUpdateAreaHint(updateArea);
    success = true;
  }
  return success;
}

bool UpdateProxy::SetIgnored(uint32_t id, bool ignored)
{
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    node->SetIgnored(ignored);
    success = true;
  }
  return success;
}

bool UpdateProxy::GetIgnored(uint32_t id, bool& ignored) const
{
  bool                    success = false;
  const SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    ignored = node->IsIgnored();
    success = true;
  }
  return success;
}

} // namespace Internal

} // namespace Dali
