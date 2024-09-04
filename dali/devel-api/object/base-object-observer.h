#ifndef DALI_BASE_OBJECT_OBSERVER_H
#define DALI_BASE_OBJECT_OBSERVER_H

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

// INTERNAL INCLUDES
#include <dali/public-api/object/base-handle.h>
#include <memory>

namespace Dali
{
/**
 * @brief Base class to provide observer interface for BaseOject destruction  store a weak pointer to an internal DALi object.
 */
class DALI_CORE_API BaseObjectObserver
{
public:
  /**
   * @brief The constructor initializes observer for given DALi object.
   * @param [in] handle A reference to the handle of the DALi object
   */
  explicit BaseObjectObserver(const BaseHandle& handle);
  virtual ~BaseObjectObserver();

  /**
   * @brief Starts observing destruction of the registered object.
   */
  void StartObservingDestruction();

  /**
   * @brief Stops observing destruction of the registered object.
   */
  void StopObservingDestruction();

  /**
   * @brief Called shortly before the object itself is destroyed.
   */
  virtual void ObjectDestroyed() = 0;

  // deleted constructors and operators
  BaseObjectObserver(const BaseObjectObserver& handle) = delete;
  BaseObjectObserver& operator=(const BaseObjectObserver& rhs) = delete;
  BaseObjectObserver(BaseObjectObserver&& rhs) = delete;
  BaseObjectObserver& operator=(BaseObjectObserver&& rhs) = delete;

private:
  /// @cond internal
  struct Impl;
  std::unique_ptr<Impl> mImpl;
  /// @endcond
};

} // namespace Dali

#endif // DALI_BASE_OBJECT_OBSERVER_H
