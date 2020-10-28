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

// CLASS HEADER
#include <dali/internal/update/nodes/node-messages.h>
#include <dali/internal/update/manager/update-manager.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

// Messages for Node

NodePropertyMessageBase::NodePropertyMessageBase(UpdateManager& updateManager)
: mUpdateManager(updateManager)
{
}

NodePropertyMessageBase::~NodePropertyMessageBase() = default;

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
