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

// CLASS HEADER
#include <dali/internal/render/shaders/program-controller.h>

// INTERNAL INCLUDES
#include <dali/integration-api/gl-defines.h>
#include <dali/internal/common/shader-saver.h>
#include <dali/internal/render/shaders/program.h>

namespace Dali
{
namespace Internal
{
ProgramController::ProgramController(Graphics::Controller& graphicsController)
: mGraphicsController(graphicsController)
{
  mProgramCache.Reserve(32);
}

ProgramController::~ProgramController() = default;

void ProgramController::ResetProgramMatrices()
{
  const ProgramIterator end = mProgramCache.End();
  for(ProgramIterator iter = mProgramCache.Begin(); iter != end; ++iter)
  {
    Program* program = (*iter)->GetProgram();
    program->SetProjectionMatrix(nullptr);
    program->SetViewMatrix(nullptr);
  }
}

Program* ProgramController::GetProgram(size_t shaderHash)
{
  Program*              program = nullptr;
  const ProgramIterator end     = mProgramCache.End();
  for(ProgramIterator iter = mProgramCache.Begin(); iter != end; ++iter)
  {
    size_t hash = (*iter)->GetHash();
    if(shaderHash == hash)
    {
      program = (*iter)->GetProgram();
      break;
    }
  }
  return program;
}

void ProgramController::AddProgram(size_t shaderHash, Program* program)
{
  // we expect unique hash values so it is event thread side's job to guarantee that
  // AddProgram is only called after program checks that GetProgram returns NULL
  mProgramCache.PushBack(new ProgramPair(program, shaderHash));
}

} // namespace Internal

} // namespace Dali
