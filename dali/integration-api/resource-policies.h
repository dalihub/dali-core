#ifndef DALI_INTEGRATION_RESOURCE_POLICIES_H
#define DALI_INTEGRATION_RESOURCE_POLICIES_H

/*
 * Copyright (c) 2019 Samsung Electronics Co., Ltd.
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
  DALI_RETAINS_ALL_DATA,  // retains all data e.g. bitmaps
  DALI_DISCARDS_ALL_DATA, // discards all data (expects application to regenerate UI on context loss)
};

/**
 * The discardable policy determines if a resource can be discarded.
 * Discarded means that it can be released after uploading to GPU.
 */
enum Discardable
{
  OWNED_DISCARD,
  OWNED_RETAIN,
};

} // namespace ResourcePolicy
} // namespace Dali

#endif // DALI_INTEGRATION_RESOURCE_POLICIES_H
