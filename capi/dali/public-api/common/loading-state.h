#ifndef __DALI_LOADING_STATE_H__
#define __DALI_LOADING_STATE_H__

//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/**
 * @addtogroup CAPI_DALI_COMMON_MODULE
 * @{
 */

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>

namespace Dali DALI_IMPORT_API
{

/**
 * @brief The status during resource loading operations.
 */
enum LoadingState
{
  ResourceLoading,          ///< The resource is loading
  ResourceLoadingSucceeded, ///< The resource loaded successfully
  ResourceLoadingFailed     ///< The resource failed to load
};

} // namespace Dali

/**
 * @}
 */
#endif // __DALI_LOADING_STATE_H__
