#ifndef DALI_INTERNAL_CONTEXT_RECOVERY_INTERFACE_H
#define DALI_INTERNAL_CONTEXT_RECOVERY_INTERFACE_H

/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
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
 * Abstract interface for Context Recovery
 *
 */
class ContextRecoveryInterface
{
public:
  /**
   * Restore the object after context loss
   */
  virtual void RecoverFromContextLoss() = 0;

protected:
  /**
   * Constructor
   */
  ContextRecoveryInterface()
  {
  }
  /**
   * Destructor.
   */
  virtual ~ContextRecoveryInterface()
  {
  }

private:
  // Undefined copy constructor.
  ContextRecoveryInterface(const ContextRecoveryInterface&);

  // Undefined assignment operator.
  ContextRecoveryInterface& operator=(const ContextRecoveryInterface&);
};

} // namespace Internal

} // namespace Dali

#endif // DALI_INTERNAL_CONTEXT_RECOVERY_INTERFACE_H
