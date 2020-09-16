#ifndef TEST_GL_CONTEXT_HELPER_ABSTRACTION_H
#define TEST_GL_CONTEXT_HELPER_ABSTRACTION_H

/*
 * Copyright (c) 2020 Samsung Electronics Co., Ltd.
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
#include <dali/integration-api/gl-context-helper-abstraction.h>

namespace Dali
{
/**
 * Class to emulate the GL context helper
 */
class DALI_CORE_API TestGlContextHelperAbstraction : public Integration::GlContextHelperAbstraction
{
public:
  /**
   * Constructor
   */
  TestGlContextHelperAbstraction(){};

  /**
   * Destructor
   */
  ~TestGlContextHelperAbstraction() override{};

  /**
   * @brief Switch to the surfaceless GL context
   */
  void MakeSurfacelessContextCurrent() override{};

  /**
   * @brief Clear the GL context
   */
  void MakeContextNull() override{};

  /**
   * @brief Wait until all GL rendering calls for the current GL context are executed
   */
  void WaitClient() override{};

private:
  TestGlContextHelperAbstraction(const TestGlContextHelperAbstraction&);            ///< Undefined
  TestGlContextHelperAbstraction& operator=(const TestGlContextHelperAbstraction&); ///< Undefined
};

} // namespace Dali

#endif // TEST_GL_CONTEXT_HELPER_ABSTRACTION_H
