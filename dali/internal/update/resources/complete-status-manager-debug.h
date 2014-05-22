#ifndef __DALI_INTERNAL_UPDATE_COMPLETE_STATUS_MANAGER_DEBUG_H__
#define __DALI_INTERNAL_UPDATE_COMPLETE_STATUS_MANAGER_DEBUG_H__
/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
namespace Dali
{
namespace Internal
{
#if defined(DEBUG_ENABLED)
Debug::Filter* gCompleteStatusLogFilter = Debug::Filter::New(Debug::Concise, false, "LOG_COMPLETE_STATUS_MANAGER");
#define TRACKER_LOG(level)                                              \
  DALI_LOG_INFO(gCompleteStatusLogFilter, level, "CompleteStatusManager::%s: this:%p\n", __FUNCTION__, this)
#define TRACKER_LOG_FMT(level, format, args...) \
  DALI_LOG_INFO(gCompleteStatusLogFilter, level, "CompleteStatusManager::%s: this:%p " format, __FUNCTION__, this, ## args)
#else
#define TRACKER_LOG(level)
#define TRACKER_LOG_FMT(level, format, args...)
#endif

}//Internal
}//Dali

#endif // __DALI_INTERNAL_UPDATE_COMPLETE_STATUS_MANAGER_DEBUG_H__
