#ifndef __DALI_SCENE_GRAPH_IMAGE_ATTACHMENT_DEBUG_H__
#define __DALI_SCENE_GRAPH_IMAGE_ATTACHMENT_DEBUG_H__

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
namespace Internal
{
namespace SceneGraph
{

#if defined(DEBUG_ENABLED)
Debug::Filter* gImageAttachmentLogFilter = Debug::Filter::New(Debug::NoLogging, false, "LOG_SCENE_GRAPH_IMAGE_ATTACHMENT");

#define ATTACHMENT_LOG(level)                                                 \
  DALI_LOG_INFO(gImageAttachmentLogFilter, level, "SceneGraph::ImageAttachment::%s: this:%p\n", __FUNCTION__, this)
#define ATTACHMENT_LOG_FMT(level, format, args...) \
  DALI_LOG_INFO(gImageAttachmentLogFilter, level, "SceneGraph::ImageAttachment::%s: this:%p " format, __FUNCTION__, this, ## args)

#else

#define ATTACHMENT_LOG(level)
#define ATTACHMENT_LOG_FMT(level, format, args...)

#endif

} // namespace SceneGraph

} // namespace Internal

} // namespace Dali

#endif // __DALI_SCENE_GRAPH_IMAGE_ATTACHMENT_DEBUG_H__
