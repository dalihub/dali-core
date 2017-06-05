#ifndef __DALI_INTEGRATION_CONTEXT_NOTIFIER_H__
#define __DALI_INTEGRATION_CONTEXT_NOTIFIER_H__

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
namespace Integration
{

/**
 * Interface to inform dali core of context loss and regain
 */
class ContextNotifierInterface
{
public:
  /**
   * Notify that the GL context has been lost, e.g. during ReplaceSurface
   * or Pause.
   *
   * Multi-threading note: this method should be called from the main thread
   */
  virtual void NotifyContextLost() = 0;

  /**
   * Notify that the GL context has been re-created, e.g. after ReplaceSurface
   * or Context loss.
   *
   * In the case of ReplaceSurface, both ContextDestroyed() and ContextCreated() will have
   * been called on the render thread before this is called on the event thread.
   *
   * Multi-threading note: this method should be called from the main thread
   */
  virtual void NotifyContextRegained() = 0;

protected:

  /**
   * Virtual destructor, no deletion through this interface
   */
  virtual ~ContextNotifierInterface() {}

};

} // namespace Integration
} // namespace Dali


#endif // __DALI_INTEGRATION_CONTEXT_NOTIFICATION_H__
