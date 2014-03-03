#ifndef __DALI_INTERNAL_CONTEXT_OBSERVER_H__
#define __DALI_INTERNAL_CONTEXT_OBSERVER_H__

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

/**
 * Context observers are notified when the OpenGL context is created or destroyed.
 */
class ContextObserver
{
public:

  /**
   * Called when the OpenGL context has been created.
   */
   virtual void GlContextCreated() = 0;

  /**
   * Called when the OpenGL context is about to be destroyed.
   * The observer should free any GL resources held.
   */
   virtual void GlContextToBeDestroyed() = 0;
};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_CONTEXT_OBSERVER_H__
