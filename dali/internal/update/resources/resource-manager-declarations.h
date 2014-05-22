#ifndef __DALI_INTERNAL_RESOURCE_MANAGER_DECLARATIONS_H__
#define __DALI_INTERNAL_RESOURCE_MANAGER_DECLARATIONS_H__

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

// INTERNAL INCLUDES
#include <dali/integration-api/resource-cache.h>
#include <dali/internal/update/common/double-buffered.h>

namespace Dali
{

namespace Internal
{

typedef Integration::ResourceId ResourceId;

/**
 * Post processing required on the resource after the resource has been modified by the render thread
 * Should only be handled by update thread
 */
struct ResourcePostProcessRequest
{
  enum PostProcess
  {
    UPLOADED,
    SAVE,
    DELETED
  };

  ResourceId   id;
  PostProcess  postProcess;

  ResourcePostProcessRequest( ResourceId anId, PostProcess aPostProcess )
  : id(anId), postProcess(aPostProcess)
  {
  }
};

typedef SceneGraph::DoubleBuffered<std::vector< ResourcePostProcessRequest> > ResourcePostProcessList;

} // Internal
} // Dali

#endif // __DALI_INTERNAL_RESOURCE_MANAGER_DECLARATIONS_H__
