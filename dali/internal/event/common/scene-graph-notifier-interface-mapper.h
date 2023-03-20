#ifndef DALI_INTERNAL_SCENE_GRAPH_NOTIFIER_INTERFACE_MAPPER_H
#define DALI_INTERNAL_SCENE_GRAPH_NOTIFIER_INTERFACE_MAPPER_H

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
 *
 */

// EXTERNAL INCLUDES
#include <memory> // for std::unique_ptr
#include <unordered_map>

// INTERNAL INCLUDES
#include <dali/internal/event/common/notifier-interface.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{
namespace Internal
{
/**
 * @brief Abstract object mapper between event object and scenegraph notifier interface object.
 * @note This class is not thread safe. Please call API only for event thread.
 *
 * @tparam EventObject Mainly controlled object. Usually in Dali::Internal namespace.
 */
template<class EventObject>
class SceneGraphNotifierInterfaceMapper
{
protected:
  /**
   * Constructor, not to be directly instantiated.
   */
  SceneGraphNotifierInterfaceMapper() = default;

  /**
   * Virtual destructor.
   */
  virtual ~SceneGraphNotifierInterfaceMapper() = default;

public:
  /**
   * Called when a NotifierInterface is mapping by EventObject.
   */
  void MapNotifier(const NotifierInterface* notifierInterface, EventObject& eventObject)
  {
    auto id = notifierInterface->GetNotifyId();

    auto iter = mNotifierMap.find(id);
    DALI_ASSERT_DEBUG(iter == mNotifierMap.end());

    mNotifierMap.insert(iter, {id, &eventObject});
  }

  /**
   * Called when a NotifierInterface is unmaped from EventObject.
   */
  void UnmapNotifier(const NotifierInterface* notifierInterface)
  {
    auto id = notifierInterface->GetNotifyId();

    auto iter = mNotifierMap.find(id);
    DALI_ASSERT_DEBUG(iter != mNotifierMap.end());

    mNotifierMap.erase(iter);
  }

protected:
  /**
   * @brief Get EventObject pointer by NotifierInterface notifyId.
   *
   * @return Mapped EventObject pointer, or nullptr if not exist.
   */
  EventObject* GetEventObject(const NotifierInterface::NotifyId notifyId)
  {
    EventObject* result = nullptr;

    const auto iter = mNotifierMap.find(notifyId);
    if(iter != mNotifierMap.end())
    {
      result = iter->second;
    }

    return result;
  }

private:
  using NotifyIdConverter = std::unordered_map<NotifierInterface::NotifyId, EventObject*>;

  NotifyIdConverter mNotifierMap{}; ///< Converter from NotifyId to EventObject pointer.
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_SCENE_GRAPH_OBJECT_MAPPER_H
