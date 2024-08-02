#ifndef DALI_INTERNAL_SCENE_GRAPH_VISUAL_RENDERER_PROPERTY_H
#define DALI_INTERNAL_SCENE_GRAPH_VISUAL_RENDERER_PROPERTY_H

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
 */

#include <dali/internal/update/common/animatable-property.h>

namespace Dali::Internal::SceneGraph::VisualRenderer
{
/**
 * @brief Interface to notify updated flags to the owner.
 * @note  This is a pure virtual class.
 */
class VisualRendererPropertyObserver
{
protected:
  /**
   * Destructor
   */
  virtual ~VisualRendererPropertyObserver() = default;

public:
  /**
   * @brief Called when some visual properties are changed.
   */
  virtual void OnVisualRendererPropertyUpdated(bool bake) = 0;

  /**
   * @brief Get the current owner's updated flags as visual properties.
   */
  virtual uint8_t GetUpdatedFlag() const = 0;
};

/**
 * @brief Base class for VisualRender properties coefficient.
 * It will mark update flag as dirty if some properties are changed.
 * By that update flag, we can determine that we need to re-calculate
 * coefficient values or not.
 */
struct VisualRendererCoefficientCacheBase
{
  VisualRendererCoefficientCacheBase(VisualRendererPropertyObserver& owner)
  : mOwner(owner),
    mUpdateCurrentFrame(true),
    mCoefficientCalculated(false)
  {
  }

  virtual ~VisualRendererCoefficientCacheBase() = default;

  /**
   * @brief Check whether this cache is updated or not.
   *
   */
  bool IsUpdated() const
  {
    return mUpdateCurrentFrame;
  }

  /**
   * @brief Mark update flag as true.
   * @param[in] bake Whether this coefficient updated by OnBake API, or not.
   */
  void Update(bool bake)
  {
    mUpdateCurrentFrame = true;
    mOwner.OnVisualRendererPropertyUpdated(bake);
  }

  /**
   * @brief Get whether we already calculate coefficient at this frame or not.
   * @return True if we already calculate coefficient at this frame, false otherwise.
   */
  bool IsCoefficientCalculated() const
  {
    return mCoefficientCalculated;
  }

  /**
   * @brief Mark as we calculate coefficient already at this frame.
   */
  void MarkCoefficientCalculated()
  {
    mCoefficientCalculated = true;
  }

  /**
   * @brief Reset update flag.
   */
  void ResetFlag()
  {
    mUpdateCurrentFrame = (mOwner.GetUpdatedFlag() != Dali::Internal::SceneGraph::CLEAN_FLAG); ///< Keep the flag whether it was updated or not.
    mCoefficientCalculated &= (!mUpdateCurrentFrame);                                          ///< Re-calculate coefficient only if previous update flag was not clean.
  }

private:
  VisualRendererPropertyObserver& mOwner; ///< Owner of this cache. It will be used to Update dirty flag for this coefficient cache.

  bool mUpdateCurrentFrame : 1;    ///< Whether we need to update this frame or not.
  bool mCoefficientCalculated : 1; ///< Whether we need to re-calculate coefficient or not.
};

/**
 * @brief Special AnimatableProperty class for VisualRenderer properties
 * that will be used for coefficient calculation.
 * It will be used to avoid useless coefficient update.
 *
 * @tparam T Type of animatable property
 */
template<typename T, size_t CacheBaseDataOffset>
struct VisualRendererProperty : public AnimatableProperty<T>
{
  enum
  {
    VISUAL_RENDERER_CACHE_BASE_DATA_OFFSET = CacheBaseDataOffset
  };
  VisualRendererCoefficientCacheBase* GetCacheBaseData()
  {
    return reinterpret_cast<VisualRendererCoefficientCacheBase*>(
      reinterpret_cast<uint8_t*>(this) - VISUAL_RENDERER_CACHE_BASE_DATA_OFFSET);
  }
  const VisualRendererCoefficientCacheBase* GetCacheBaseData() const
  {
    return reinterpret_cast<const VisualRendererCoefficientCacheBase*>(
      reinterpret_cast<const uint8_t*>(this) - VISUAL_RENDERER_CACHE_BASE_DATA_OFFSET);
  }

public:
  /**
   * Constructor, initialize the dirty flag
   */
  VisualRendererProperty(const T& initialValue)
  : AnimatableProperty<T>(initialValue)
  {
  }

  /**
   * Virtual destructor.
   */
  ~VisualRendererProperty() override = default;

  /**
   * @copydoc Dali::Internal::SceneGraph::AnimatablePropertyBase::OnSet
   */
  void OnSet() override
  {
    GetCacheBaseData()->Update(false);
    AnimatableProperty<T>::OnSet();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::AnimatablePropertyBase::OnBake
   */
  void OnBake() override
  {
    GetCacheBaseData()->Update(true);
    AnimatableProperty<T>::OnBake();
  }
};

} // namespace Dali::Internal::SceneGraph::VisualRenderer

#endif // DALI_INTERNAL_SCENE_GRAPH_VISUAL_RENDERER_PROPERTY_H
