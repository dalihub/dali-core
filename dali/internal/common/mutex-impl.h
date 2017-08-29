#ifndef __DALI_INTERNAL_MUTEX_H__
#define __DALI_INTERNAL_MUTEX_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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
#include <pthread.h>

namespace Dali
{

namespace Internal
{

/**
 * @brief Namespace to ensure mutex locking is done correctly.
 *
 * Displays warnings if two mutex locks are not held by the same thread at any given time which can lead to deadlock.
 * This can lead to deadlock and should be avoided.
 *
 * @note lock backtrace needs to be enabled to see the warnings.
 */
namespace Mutex
{

/**
 * @brief Locks the given mutex.
 *
 * Increments a thread-local storage counter.
 *
 * @param A pointer to the mutex that should be locked.
 *
 * @note If the counter is > 1 and lock backtrace is enabled, then the backtrace for all locks will be shown as a warning.
 */
void Lock( pthread_mutex_t* mutex );

/**
 * @brief Unlocks the given mutex.
 *
 * @param A pointer to the mutex that should be unlocked.
 *
 * Decrements a thread-local storage counter.
 */
void Unlock( pthread_mutex_t* mutex );

} // namespace Mutex

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_MUTEX_H__
