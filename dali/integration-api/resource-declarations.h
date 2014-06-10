#ifndef __DALI_INTEGRATION_RESOURCE_DECLARATIONS_H__
#define __DALI_INTEGRATION_RESOURCE_DECLARATIONS_H__

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

namespace Dali
{

namespace Integration
{

/**
 * Used to identify a loading operation.
 * This begins with a call to PlatformAbstraction::LoadResource().
 */
typedef unsigned int ResourceId;

/**
 * Used to inform the current loading status
 */
enum LoadStatus
{
  RESOURCE_LOADING,            ///< There are missing resources, being loaded
  RESOURCE_PARTIALLY_LOADED,   ///< Enough resource has been loaded to start low quality rendering
  RESOURCE_COMPLETELY_LOADED,  ///< The resource has been completely loaded
};

} // namespace Integration

} // namespace Dali

#endif // __DALI_INTEGRATION_RESOURCE_DECLARATIONS_H__
