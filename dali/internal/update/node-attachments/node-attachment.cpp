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
#include <dali/internal/update/node-attachments/node-attachment.h>

// INTERNAL INCLUDES
#include <dali/internal/update/nodes/node.h>
#include <dali/public-api/common/dali-common.h>

namespace Dali
{

namespace Internal
{

namespace SceneGraph
{

NodeAttachment::NodeAttachment()
: mParent(NULL)
{
}

NodeAttachment::~NodeAttachment()
{
}

void NodeAttachment::SetParent( Node& parent )
{
  DALI_ASSERT_DEBUG(mParent == NULL);

  mParent = &parent;
}

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali
