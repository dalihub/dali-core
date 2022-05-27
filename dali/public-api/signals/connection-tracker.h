#ifndef DALI_CONNECTION_TRACKER_H
#define DALI_CONNECTION_TRACKER_H

/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
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
#include <cstddef> // for std::size_t

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/public-api/signals/connection-tracker-interface.h>
#include <dali/public-api/signals/signal-slot-observers.h>

namespace Dali
{
/**
 * @addtogroup dali_core_signals
 * @{
 */

class CallbackBase;
class SlotObserver;

/**
 * @brief Connection tracker concrete implementation.
 * @SINCE_1_0.0
 */
class DALI_CORE_API ConnectionTracker : public ConnectionTrackerInterface
{
public:
  /**
   * @brief Constructor.
   * @SINCE_1_0.0
   */
  ConnectionTracker();

  /**
   * @brief Virtual destructor.
   * @SINCE_1_0.0
   */
  ~ConnectionTracker() override;

  /**
   * @brief Disconnects all signals from this object.
   * @SINCE_1_0.0
   */
  void DisconnectAll();

  /**
   * @copydoc ConnectionTrackerInterface::SignalConnected
   */
  void SignalConnected(SlotObserver* slotObserver, CallbackBase* callback) override;

  /**
   * @copydoc ConnectionTrackerInterface::SignalDisconnected
   */
  void SignalDisconnected(SlotObserver* slotObserver, CallbackBase* callback) override;

  /**
   * @brief Returns the connection count.
   * @SINCE_1_0.0
   * @return The connection count
   */
  std::size_t GetConnectionCount() const;

private:
  ConnectionTracker(const ConnectionTracker&) = delete;            ///< Deleted copy constructor. @SINCE_1_0.0
  ConnectionTracker(ConnectionTracker&&)      = delete;            ///< Deleted move constructor. @SINCE_1_9.25
  ConnectionTracker& operator=(const ConnectionTracker&) = delete; ///< Deleted copy assignment operator. @SINCE_1_0.0
  ConnectionTracker& operator=(ConnectionTracker&&) = delete;      ///< Deleted move assignment operator. @SINCE_1_9.25

private:
  struct DALI_INTERNAL Impl;
  Impl*                mCacheImpl; ///< Private internal extra data.
};

/**
 * @}
 */
} // namespace Dali

#endif // DALI_CONNECTION_TRACKER_H
