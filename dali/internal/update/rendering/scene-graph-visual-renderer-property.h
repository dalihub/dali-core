#ifndef DALI_INTERNAL_SCENE_GRAPH_VISUAL_RENDERER_PROPERTY_H
#define DALI_INTERNAL_SCENE_GRAPH_VISUAL_RENDERER_PROPERTY_H

/*
 * Copyright (c) 2023 Samsung Electronics Co., Ltd.
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
 * @brief Base class for VisualRender properties coefficient.
 * It will mark update flag as dirty if some properties are changed.
 * By that update flag, we can determine that we need to re-calculate
 * coefficient values or not.
 */
struct VisualRendererCoefficientCacheBase
{
  VisualRendererCoefficientCacheBase()
  : mUpdated(true)
  {
  }

  virtual ~VisualRendererCoefficientCacheBase() = default;

  /**
   * @brief Check whether this cache need to be update.
   * After call this API, update flag will be reset.
   *
   * @return True if this coefficient updated. False otherwise.
   */
  bool IsUpdated()
  {
    bool ret = mUpdated;
    mUpdated = false;
    return ret;
  }

  /**
   * @brief Mark update flag as true.
   */
  void Update()
  {
    mUpdated = true;
  }

private:
  bool mUpdated; ///< Updated flag for this coefficient cache.
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
    GetCacheBaseData()->Update();
    AnimatablePropertyBase::OnSet();
  }

  /**
   * @copydoc Dali::Internal::SceneGraph::AnimatablePropertyBase::OnBake
   */
  void OnBake() override
  {
    GetCacheBaseData()->Update();
    AnimatablePropertyBase::OnBake();
  }
};

} // namespace Dali::Internal::SceneGraph::VisualRenderer

#endif // DALI_INTERNAL_SCENE_GRAPH_VISUAL_RENDERER_PROPERTY_H
