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

// EXTERNAL INCLUDES
#include <dali/integration-api/debug.h>
#include <dali/integration-api/trace.h>

// INTERNAL INCLUDES
#include <dali/internal/update/manager/update-manager.h>
#include <dali/internal/update/manager/update-proxy-property-modifier.h>

namespace Dali
{
namespace Internal
{
// NOTE : Temporal duration checker for issue tracking. Should be removed after issue resolved.
namespace
{
DALI_INIT_TRACE_FILTER(gTraceFilter, DALI_TRACE_UPDATE_PROCESS, false);

DALI_INIT_TIME_CHECKER_FILTER_WITH_DEFAULT_THRESHOLD(gTimeCheckerFilter, DALI_UPDATE_PROCESS_THRESHOLD_TIME, 48);
} // namespace

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
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool                    success = false;
  const SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "GetPosition");
    const SceneGraph::TransformManager& transformManager = mTransformManager; // To ensure we call the const getter
    position                                             = transformManager.GetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_POSITION);
    success                                              = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "GetPosition[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::SetPosition(uint32_t id, const Vector3& position)
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "SetPosition");
    mTransformManager.SetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_POSITION, position);
    success = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "SetPosition[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::BakePosition(uint32_t id, const Vector3& position)
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "BakePosition");
    mTransformManager.BakeVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_POSITION, position);
    success = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "BakePosition[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::GetOrientation(uint32_t id, Quaternion& orientation) const
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool                    success = false;
  const SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "GetOrientation");
    const SceneGraph::TransformManager& transformManager = mTransformManager; // To ensure we call the const getter

    orientation = transformManager.GetQuaternionPropertyValue(node->GetTransformId());
    success     = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "GetOrientation[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::SetOrientation(uint32_t id, const Quaternion& orientation)
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "SetOrientation");
    mTransformManager.SetQuaternionPropertyValue(node->GetTransformId(), orientation);
    success = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "SetOrientation[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::BakeOrientation(uint32_t id, const Quaternion& orientation)
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "BakeOrientation");
    mTransformManager.BakeQuaternionPropertyValue(node->GetTransformId(), orientation);
    success = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "BakeOrientation[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::GetSize(uint32_t id, Vector3& size) const
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool                    success = false;
  const SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "GetSize");
    const SceneGraph::TransformManager& transformManager = mTransformManager; // To ensure we call the const getter
    size                                                 = transformManager.GetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_SIZE);
    success                                              = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "GetSize[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::SetSize(uint32_t id, const Vector3& size)
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "SetSize");
    mTransformManager.SetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_SIZE, size);
    success = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "SetSize[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::BakeSize(uint32_t id, const Vector3& size)
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "BakeSize");
    mTransformManager.BakeVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_SIZE, size);
    success = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "BakeSize[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::GetPositionAndSize(uint32_t id, Vector3& position, Vector3& size) const
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool                    success = false;
  const SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "GetPositionAndSize");
    const SceneGraph::TransformManager& transformManager = mTransformManager; // To ensure we call the const getter
    position                                             = transformManager.GetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_POSITION);
    size                                                 = transformManager.GetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_SIZE);
    success                                              = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "GetPositionAndSize[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::GetWorldPositionScaleAndSize(uint32_t id, Vector3& position, Vector3& scale, Vector3& size) const
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool                    success = false;
  const SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "GetWorldPositionScaleAndSize");
    const SceneGraph::TransformManager& transformManager = mTransformManager; // To ensure we call the const getter
    const Matrix&                       worldMatrix      = transformManager.GetWorldMatrix(node->GetTransformId());

    Quaternion orientation;
    worldMatrix.GetTransformComponents(position, orientation, scale);

    size    = transformManager.GetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_SIZE);
    success = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "GetWorldPositionScaleAndSize[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::GetWorldTransformAndSize(uint32_t id, Vector3& position, Vector3& scale, Quaternion& orientation, Vector3& size) const
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool                    success = false;
  const SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "GetWorldTransformAndSize");
    const SceneGraph::TransformManager& transformManager = mTransformManager; // To ensure we call the const getter
    const Matrix&                       worldMatrix      = transformManager.GetWorldMatrix(node->GetTransformId());

    worldMatrix.GetTransformComponents(position, orientation, scale);
    size    = transformManager.GetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_SIZE);
    success = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "GetWorldTransformAndSize[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::GetScale(uint32_t id, Vector3& scale) const
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool                    success = false;
  const SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "GetScale");
    const SceneGraph::TransformManager& transformManager = mTransformManager; // To ensure we call the const getter
    scale                                                = transformManager.GetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_SCALE);
    success                                              = true;
  }

  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "GetScale[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::SetScale(uint32_t id, const Vector3& scale)
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "SetScale");
    mTransformManager.SetVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_SCALE, scale);
    success = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "SetScale[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::BakeScale(uint32_t id, const Vector3& scale)
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "BakeScale");
    mTransformManager.BakeVector3PropertyValue(node->GetTransformId(), SceneGraph::TRANSFORM_PROPERTY_SCALE, scale);
    success = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "BakeScale[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::GetColor(uint32_t id, Vector4& color) const
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool                    success = false;
  const SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "GetColor");
    color   = node->mColor.Get(mCurrentBufferIndex);
    success = true;
  }

  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "GetColor[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::SetColor(uint32_t id, const Vector4& color)
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "SetColor");
    node->mColor.Set(mCurrentBufferIndex, color);
    node->SetDirtyFlag(SceneGraph::NodePropertyFlags::COLOR);
    mDirtyNodes.push_back(id);
    AddResetter(*node, node->mColor);
    success = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "SetColor[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::BakeColor(uint32_t id, const Vector4& color)
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "BakeColor");
    node->mColor.Bake(mCurrentBufferIndex, color);
    success = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "BakeColor[" << id << "] [" << success << "]";
  });
  return success;
}

void UpdateProxy::NodeHierarchyChanged()
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  mLastCachedIdNodePair = {0u, nullptr};
  mPropertyModifier.reset();
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  { oss << "NodeHierarchyChanged"; });
}

void UpdateProxy::Notify(Dali::UpdateProxy::NotifySyncPoint syncPoint)
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  mSyncPoints.push_back(syncPoint);
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  { oss << "Notify [" << syncPoint << "]"; });
}

Dali::UpdateProxy::NotifySyncPoint UpdateProxy::PopSyncPoint()
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  if(!mSyncPoints.empty())
  {
    auto syncPoint = mSyncPoints.front();
    mSyncPoints.pop_front();
    DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
    { oss << "PopSyncPoint [" << syncPoint << "]"; });
    return syncPoint;
  }

  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  { oss << "PopSyncPoint failed"; });
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
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "GetUpdateArea");
    updateArea = node->GetUpdateAreaHint();
    success    = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "GetUpdateArea[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::SetUpdateArea(uint32_t id, const Vector4& updateArea)
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "SetUpdateArea");
    node->SetUpdateAreaHint(updateArea);
    success = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "SetUpdateArea[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::SetIgnored(uint32_t id, bool ignored)
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "SetIgnored");
    node->SetIgnored(ignored);
    success = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "SetIgnored[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::GetIgnored(uint32_t id, bool& ignored) const
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool                    success = false;
  const SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "GetIgnored");
    ignored = node->IsIgnored();
    success = true;
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "GetIgnored[" << id << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::GetCustomProperty(uint32_t id, ConstString propertyName, Property::Value& value) const
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "GetCustomProperty");
    const auto& uniformMap = node->GetUniformMap();

    const PropertyInputImpl* propertyInputImpl = uniformMap.Find(propertyName);
    if(propertyInputImpl)
    {
      auto propertyValue = propertyInputImpl->GetPropertyValue(mCurrentBufferIndex);
      if(propertyValue.GetType() != Property::Type::NONE)
      {
        value   = std::move(propertyValue);
        success = true;
      }
    }
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "GetCustomProperty[" << id << "] [" << propertyName.GetStringView() << "] [" << success << "]";
  });
  return success;
}

bool UpdateProxy::BakeCustomProperty(uint32_t id, ConstString propertyName, const Property::Value& value)
{
  DALI_TIME_CHECKER_BEGIN(gTimeCheckerFilter);
  bool              success = false;
  SceneGraph::Node* node    = GetNodeWithId(id);
  if(node)
  {
    DALI_TIME_CHECKER_SCOPE(gTimeCheckerFilter, "BakeCustomProperty");
    const auto& uniformMap = node->GetUniformMap();

    const PropertyInputImpl* propertyInputImpl = uniformMap.Find(propertyName);
    if(propertyInputImpl)
    {
      SceneGraph::PropertyBase* propertyBase = dynamic_cast<SceneGraph::PropertyBase*>(const_cast<PropertyInputImpl*>(propertyInputImpl));
      if(propertyBase)
      {
        Property::Value convertedValue = value;
        if(DALI_UNLIKELY(value.GetType() != propertyBase->GetType()))
        {
          if(!convertedValue.ConvertType(propertyBase->GetType()))
          {
            return false;
          }
        }
        switch(propertyBase->GetType())
        {
          case Property::BOOLEAN:
          {
            if(SceneGraph::AnimatableProperty<bool>* property = dynamic_cast<SceneGraph::AnimatableProperty<bool>*>(propertyBase))
            {
              property->Bake(mCurrentBufferIndex, convertedValue.Get<bool>());
              success = true;
            }
            break;
          }

          case Property::INTEGER:
          {
            if(SceneGraph::AnimatableProperty<int32_t>* property = dynamic_cast<SceneGraph::AnimatableProperty<int32_t>*>(propertyBase))
            {
              property->Bake(mCurrentBufferIndex, convertedValue.Get<int32_t>());
              success = true;
            }
            break;
          }

          case Property::FLOAT:
          {
            if(SceneGraph::AnimatableProperty<float>* property = dynamic_cast<SceneGraph::AnimatableProperty<float>*>(propertyBase))
            {
              property->Bake(mCurrentBufferIndex, convertedValue.Get<float>());
              success = true;
            }
            break;
          }

          case Property::VECTOR2:
          {
            if(SceneGraph::AnimatableProperty<Vector2>* property = dynamic_cast<SceneGraph::AnimatableProperty<Vector2>*>(propertyBase))
            {
              property->Bake(mCurrentBufferIndex, convertedValue.Get<Vector2>());
              success = true;
            }
            break;
          }

          case Property::VECTOR3:
          {
            if(SceneGraph::AnimatableProperty<Vector3>* property = dynamic_cast<SceneGraph::AnimatableProperty<Vector3>*>(propertyBase))
            {
              property->Bake(mCurrentBufferIndex, convertedValue.Get<Vector3>());
              success = true;
            }
            break;
          }

          case Property::VECTOR4:
          {
            if(SceneGraph::AnimatableProperty<Vector4>* property = dynamic_cast<SceneGraph::AnimatableProperty<Vector4>*>(propertyBase))
            {
              property->Bake(mCurrentBufferIndex, convertedValue.Get<Vector4>());
              success = true;
            }
            break;
          }

          case Property::ROTATION:
          {
            if(SceneGraph::AnimatableProperty<Quaternion>* property = dynamic_cast<SceneGraph::AnimatableProperty<Quaternion>*>(propertyBase))
            {
              property->Bake(mCurrentBufferIndex, convertedValue.Get<Quaternion>());
              success = true;
            }
            break;
          }

          case Property::MATRIX:
          {
            if(SceneGraph::AnimatableProperty<Matrix>* property = dynamic_cast<SceneGraph::AnimatableProperty<Matrix>*>(propertyBase))
            {
              property->Bake(mCurrentBufferIndex, convertedValue.Get<Matrix>());
              success = true;
            }
            break;
          }

          case Property::MATRIX3:
          {
            if(SceneGraph::AnimatableProperty<Matrix3>* property = dynamic_cast<SceneGraph::AnimatableProperty<Matrix3>*>(propertyBase))
            {
              property->Bake(mCurrentBufferIndex, convertedValue.Get<Matrix3>());
              success = true;
            }
            break;
          }
          default:
            break;
        }
      }
    }
  }
  DALI_TIME_CHECKER_END_WITH_MESSAGE_GENERATOR(gTimeCheckerFilter, [&](std::ostringstream& oss)
  {
    oss << "BakeCustomProperty[" << id << "] [" << propertyName.GetStringView() << "] [" << success << "]";
  });
  return success;
}

} // namespace Internal

} // namespace Dali
