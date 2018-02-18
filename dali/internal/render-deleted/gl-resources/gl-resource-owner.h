#ifndef __DALI_INTERNAL_GL_RESOURCE_OWNER_H__
#define __DALI_INTERNAL_GL_RESOURCE_OWNER_H__

/*
 * Copyright (c) 2018 Samsung Electronics Co., Ltd.
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

/**
 * Abstract interface for objects which own GL resources.
 * These objects must release their GL resources in the render-thread.
 */
class GlResourceOwner
{
public:

  /**
   * Virtual destructor.
   */
  virtual ~GlResourceOwner()
  { }

  /**
   * Reset all GL resources.
   * This method is called when context is or has been deleted.
   * Context cannot be called from this method.
   * @pre This method can only be called from the render-thread.
   */
  virtual void GlContextDestroyed() = 0;

  /**
   * Release all GL resources.
   * This means releasing buffer objects, textures. etc
   * Context can be used, unless GlContextDestroyed has been called
   * @pre This method can only be called from the render-thread.
   */
  virtual void GlCleanup() = 0;

};

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_GL_RESOURCE_OWNER_H__
