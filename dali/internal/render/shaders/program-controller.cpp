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
#include <dali/internal/render/shaders/program-controller.h>

// INTERNAL INCLUDES
#include <dali/integration-api/gl-defines.h>
#include <dali/internal/common/shader-saver.h>
#include <dali/internal/render/common/texture-uploaded-dispatcher.h>
#include <dali/internal/render/gl-resources/gl-call-debug.h>
#include <dali/internal/render/shaders/program.h>
#include <dali/internal/update/resources/resource-manager-declarations.h>

namespace Dali
{

namespace Internal
{

ProgramController::ProgramController( Integration::GlAbstraction& glAbstraction )
: mShaderSaver( 0 ),
  mGlAbstraction( glAbstraction ),
  mCurrentProgram( NULL ),
  mProgramBinaryFormat( 0 ),
  mNumberOfProgramBinaryFormats( 0 )
{
  // we have 17 default programs so make room for those and a few custom ones as well
  mProgramCache.Reserve( 32 );
}

ProgramController::~ProgramController()
{
}

void ProgramController::ResetProgramMatrices()
{
  const ProgramIterator end = mProgramCache.End();
  for ( ProgramIterator iter = mProgramCache.Begin(); iter != end; ++iter )
  {
    Program* program = (*iter)->GetProgram();
    program->SetProjectionMatrix( NULL );
    program->SetViewMatrix( NULL );
  }
}

void ProgramController::GlContextCreated()
{
  // reset any potential previous errors
  LOG_GL( "GetError()\n" );
  CHECK_GL( mGlAbstraction, mGlAbstraction.GetError() );

  // find out if program binaries are supported and the format enum as well
  Dali::Vector<GLint> programBinaryFormats;

  CHECK_GL( mGlAbstraction, mGlAbstraction.GetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS_OES, &mNumberOfProgramBinaryFormats ) );
  LOG_GL("GetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS_OES) = %d\n", mNumberOfProgramBinaryFormats );

  if( GL_NO_ERROR == mGlAbstraction.GetError() && 0 < mNumberOfProgramBinaryFormats )
  {
    programBinaryFormats.Resize( mNumberOfProgramBinaryFormats );
    CHECK_GL( mGlAbstraction, mGlAbstraction.GetIntegerv(GL_PROGRAM_BINARY_FORMATS_OES, &programBinaryFormats[0] ) );
    LOG_GL("GetIntegerv(GL_PROGRAM_BINARY_FORMATS_OES) = %d\n", programBinaryFormats[0] );
    mProgramBinaryFormat = programBinaryFormats[0];
  }
}

void ProgramController::GlContextDestroyed()
{
  mNumberOfProgramBinaryFormats = 0;
  mProgramBinaryFormat = 0;

  SetCurrentProgram( NULL );
  // Inform programs they are no longer valid
  const ProgramIterator end = mProgramCache.End();
  for ( ProgramIterator iter = mProgramCache.Begin(); iter != end; ++iter )
  {
    (*iter)->GetProgram()->GlContextDestroyed();
  }
}

Integration::GlAbstraction& ProgramController::GetGlAbstraction()
{
  return mGlAbstraction;
}

Program* ProgramController::GetProgram( size_t shaderHash )
{
  Program* program = NULL;
  const ProgramIterator end = mProgramCache.End();
  for ( ProgramIterator iter = mProgramCache.Begin(); iter != end; ++iter )
  {
    size_t hash = (*iter)->GetHash();
    if( shaderHash == hash )
    {
      program = (*iter)->GetProgram();
      break;
    }
  }
  return program;
}

void ProgramController::AddProgram( size_t shaderHash, Program* program )
{
  // we expect unique hash values so its event thread sides job to guarantee that
  // AddProgram is only called after program checks that GetProgram returns NULL
  mProgramCache.PushBack( new ProgramPair( program, shaderHash ) );
}

Program* ProgramController::GetCurrentProgram()
{
  return mCurrentProgram;
}

void ProgramController::SetCurrentProgram( Program* program )
{
  mCurrentProgram = program;
}

bool ProgramController::IsBinarySupported()
{
  return mNumberOfProgramBinaryFormats > 0;
}

unsigned int ProgramController::ProgramBinaryFormat()
{
  return mProgramBinaryFormat;
}

void ProgramController::StoreBinary( Internal::ShaderDataPtr programData )
{
  DALI_ASSERT_DEBUG( programData->GetBufferSize() > 0 );
  DALI_ASSERT_DEBUG( mShaderSaver && "SetShaderSaver() should have been called during startup." );

  if( mShaderSaver != NULL )
  {
    mShaderSaver->SaveBinary( programData );
  }
}

void ProgramController::SetShaderSaver( ShaderSaver& shaderSaver )
{
  mShaderSaver = &shaderSaver;
}

} // namespace Internal

} // namespace Dali
