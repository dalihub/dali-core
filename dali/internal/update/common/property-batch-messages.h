#ifndef DALI_INTERNAL_COMMON_PROPERTY_BATCH_MESSAGES_H
#define DALI_INTERNAL_COMMON_PROPERTY_BATCH_MESSAGES_H

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
 */

// INTERNAL INCLUDES
#include <dali/internal/common/message.h>
#include <dali/internal/event/common/event-thread-services.h>
#include <dali/internal/update/common/animatable-property.h>
#include <dali/internal/update/common/property-batch.h>
#include <dali/internal/update/common/property-owner.h>
#include <dali/internal/update/common/property-resetter.h>
#include <dali/internal/update/manager/update-manager.h>

namespace Dali::Internal
{

/**
 * @brief Message that applies a batch of property updates in the update thread.
 *
 * Deep-copies the entries it needs out of the PropertyBatch - it does not copy
 * PropertyBatch itself (which is non-copyable) and does NOT share any heap
 * pointers with it. This is required because the Object's batch buffer is
 * cleared and returned to the free-list immediately after this message is
 * constructed, so anything the message still needs at Process() time must already
 * be its own.
 *
 * Matches MessageBakeReset's pattern (animatable-property-messages.h) -- takes
 * UpdateManager& needed in Process() to register a BakerResetter per property.
 */
class PropertyBatchMessage : public MessageBase
{
public:
  /**
   * @brief Constructor - deep-copies entries from the batch.
   *
   * @param[in] updateManager Reference to UpdateManager (for RegisterResetter)
   * @param[in] owner The PropertyOwner that owns the properties being updated
   * @param[in] batch The PropertyBatch to copy entries from
   */
  PropertyBatchMessage(SceneGraph::UpdateManager&       updateManager,
                       const SceneGraph::PropertyOwner& owner,
                       const PropertyBatch&             batch)
  : mUpdateManager(updateManager), mOwner(owner), mCount(batch.Count())
  {
    // Heap allocation for the entries array.
    // Worth revisiting with an inline buffer if Phase 4 profiling shows it matters.
    mEntries = new PropertyBatchEntry[mCount];
    for(uint16_t i = 0; i < mCount; ++i)
    {
      const PropertyBatchEntry& src = batch[i];
      mEntries[i]                   = src; // POD copy of property/index/type/value bits -
                                           // `property` is a non-owning observer pointer, so
                                           // copying it here is just a pointer copy, not a
                                           // new ownership relationship

      if(src.type == Property::MATRIX || src.type == Property::MATRIX3)
      {
        // src.value.ptr is owned by the Object's batch and will be freed by
        // its Clear() right after this constructor returns - duplicate the
        // bytes so this message owns an independent copy.
        size_t size           = (src.type == Property::MATRIX) ? 64 : 36;
        mEntries[i].value.ptr = new uint8_t[size];
        memcpy(mEntries[i].value.ptr, src.value.ptr, size);
      }
    }
  }

  /**
   * @brief Destructor - frees heap-allocated MATRIX/MATRIX3 entry values and the entries array.
   */
  ~PropertyBatchMessage() override
  {
    for(uint16_t i = 0; i < mCount; ++i)
    {
      if(mEntries[i].type == Property::MATRIX || mEntries[i].type == Property::MATRIX3)
      {
        delete[] static_cast<uint8_t*>(mEntries[i].value.ptr);
      }
    }
    delete[] mEntries;
  }

  /**
   * @brief Process the message in the update thread.
   *
   * Applies each batch entry's value via the property's Bake() method,
   * then registers a BakerResetter for each property.
   *
   * No object-validity check here, matching the actual precedent: every existing
   * property-set message goes through MessageBakeReset, and its Process() calls
   * (object->*memberFunction)(param) directly with no IsValidObject() check -
   * this is different from message.h's *generic* templates (Message<T>
   * MessageValue1-3, MessageDoubleBuffered0-4), which do call IsValidObject().
   * PropertyBatchMessage is the direct analog of MessageBakeReset, not of
   * the generic templates, so it follows MessageBakeReset's precedent here.
   */
  void Process() override
  {
    for(uint16_t i = 0; i < mCount; ++i)
    {
      const PropertyBatchEntry& entry = mEntries[i];
      switch(entry.type)
      {
        case Property::BOOLEAN:
        {
          auto* property = const_cast<SceneGraph::AnimatableProperty<bool>*>(
            static_cast<const SceneGraph::AnimatableProperty<bool>*>(entry.property));
          property->Bake(entry.value.b);
          RegisterResetter(property);
          break;
        }
        case Property::FLOAT:
        {
          auto* property = const_cast<SceneGraph::AnimatableProperty<float>*>(
            static_cast<const SceneGraph::AnimatableProperty<float>*>(entry.property));
          property->Bake(entry.value.f);
          RegisterResetter(property);
          break;
        }
        case Property::INTEGER:
        {
          auto* property = const_cast<SceneGraph::AnimatableProperty<int32_t>*>(
            static_cast<const SceneGraph::AnimatableProperty<int32_t>*>(entry.property));
          property->Bake(entry.value.i);
          RegisterResetter(property);
          break;
        }
        case Property::VECTOR2:
        {
          auto* property = const_cast<SceneGraph::AnimatableProperty<Vector2>*>(
            static_cast<const SceneGraph::AnimatableProperty<Vector2>*>(entry.property));
          property->Bake(Vector2(entry.value.v2));
          RegisterResetter(property);
          break;
        }
        case Property::VECTOR3:
        {
          auto* property = const_cast<SceneGraph::AnimatableProperty<Vector3>*>(
            static_cast<const SceneGraph::AnimatableProperty<Vector3>*>(entry.property));
          property->Bake(Vector3(entry.value.v3));
          RegisterResetter(property);
          break;
        }
        case Property::VECTOR4:
        {
          auto* property = const_cast<SceneGraph::AnimatableProperty<Vector4>*>(
            static_cast<const SceneGraph::AnimatableProperty<Vector4>*>(entry.property));
          property->Bake(Vector4(entry.value.v4));
          RegisterResetter(property);
          break;
        }
        case Property::ROTATION:
        {
          // ROTATION (Quaternion) is never set via SetXComponentMessage/Y/Z/W
          // (those only exist for vector types), so it always arrives
          // here as a full-property set and uses the v4 slot of the union -
          // (x, y, z, w),
          auto* property = const_cast<SceneGraph::AnimatableProperty<Quaternion>*>(
            static_cast<const SceneGraph::AnimatableProperty<Quaternion>*>(entry.property));
          property->Bake(Quaternion(entry.value.v4[0], entry.value.v4[1], entry.value.v4[2], entry.value.v4[3]));
          RegisterResetter(property);
          break;
        }
        case Property::MATRIX:
        {
          auto* property = const_cast<SceneGraph::AnimatableProperty<Matrix>*>(
            static_cast<const SceneGraph::AnimatableProperty<Matrix>*>(entry.property));
          property->Bake(Matrix(static_cast<const float*>(entry.value.ptr)));
          RegisterResetter(property);
          break;
        }
        case Property::MATRIX3:
        {
          auto* property = const_cast<SceneGraph::AnimatableProperty<Matrix3>*>(
            static_cast<const SceneGraph::AnimatableProperty<Matrix3>*>(entry.property));
          const float* m = static_cast<const float*>(entry.value.ptr);
          property->Bake(Matrix3(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8]));
          RegisterResetter(property);
          break;
        }
        default:
        {
          // Every animatable Property::Type is handled above (BOOLEAN, FLOAT,
          // INTEGER, VECTOR2/3/4, MATRIX, MATRIX3, ROTATION).
          // Reaching here means either a non-animatable type was incorrectly
          // routed into the batch, or a new animatable type was added without
          // updating this switch.
          DALI_ASSERT_DEBUG(false && "Unsupported property type in PropertyBatchMessage");
          break;
        }
      }
    }
  }

private:
  /**
   * @brief Registers a BakerResetter for a property (template helper).
   *
   * Mirrors MessageBakeReset::Process()'s resetter creation (animatable-property-messages.h)
   * - one BakerResetter per baked property, registered with the update manager so the
   * bake gets reset correctly between frames. A template member defined in-class like
   * this has no ODR concerns either way (template definitions are exempt from the
   * one-definition rule in the same way inline functions are), but it's kept here for
   * locality with Process(), its only caller.
   *
   * @tparam T The property value type
   * @param[in] property The property to register a resetter for
   */
  template<typename T>
  void RegisterResetter(const SceneGraph::AnimatableProperty<T>* property)
  {
    OwnerPointer<SceneGraph::PropertyResetterBase> resetter(
      new SceneGraph::BakerResetter(const_cast<SceneGraph::PropertyOwner*>(&mOwner),
                                    const_cast<SceneGraph::AnimatableProperty<T>*>(property),
                                    SceneGraph::BakerResetter::Lifetime::BAKE));
    mUpdateManager.AddPropertyResetter(resetter);
  }

  // Reference, not pointer: matches MessageBakeReset (animatable-property-messages.h)
  // - the actual class underlying every existing property-set message (BakeMessage,
  // SetXComponentMessage, etc.) -- which stores const SceneGraph::PropertyOwner&
  // mPropertyOwner and SceneGraph::UpdateManager& mUpdateManager as reference members,
  // constructed directly from reference parameters.
  SceneGraph::UpdateManager&       mUpdateManager;
  const SceneGraph::PropertyOwner& mOwner;
  PropertyBatchEntry*              mEntries;
  uint16_t                         mCount;
};

/**
 * @brief Helper to send a PropertyBatchMessage via the message queue.
 *
 * Matches the pattern of BakeMessage() and other message-sending helpers
 * in animatable-property-messages.h.
 *
 * @param[in] services EventThreadServices for ReserveMessageSlot()
 * @param[in] owner The PropertyOwner that owns the properties being updated
 * @param[in] batch The PropertyBatch to send
 */
inline void FlushPropertyBatchMessage(EventThreadServices&             services,
                                      const SceneGraph::PropertyOwner& owner,
                                      const PropertyBatch&             batch)
{
  using LocalType = PropertyBatchMessage;
  uint32_t* slot  = services.ReserveMessageSlot(sizeof(LocalType));
  new(slot) LocalType(services.GetUpdateManager(), owner, batch);
}

} // namespace Dali::Internal

#endif // DALI_INTERNAL_COMMON_PROPERTY_BATCH_MESSAGES_H
