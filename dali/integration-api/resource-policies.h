#ifndef __DALI_INTEGRATION_RESOURCE_POLICIES_H__
#define __DALI_INTEGRATION_RESOURCE_POLICIES_H__

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
 */

namespace Dali
{
namespace ResourcePolicy
{

/**
 * The data retention policy describes how dali should retain resource data.
 */
enum DataRetention
{
  DALI_RETAINS_ALL_DATA,  // retains Meshes and bitmaps
  DALI_RETAINS_MESH_DATA, // retains meshes, but discards bitmaps loaded from files
  DALI_DISCARDS_ALL_DATA, // discards all data (expects application to regenerate UI on context loss)
};

/**
 * The discardable policy describes whether a resource can be discarded (i.e. it is throw-away
 * or recoverable)
 */
enum Discardable
{
  DISCARD, // @todo Change to IS_RECOVERABLE?
  RETAIN
};

} // namespace ResourcePolicy
} // namespace Dali

#endif // __DALI_INTEGRATION_DISCARD_POLICY_H__
