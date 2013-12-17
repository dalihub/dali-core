#define tet_infoline printf
#define tet_printf printf
#include "test-application.h"

bool BlendEnabled(const TraceCallStack& callStack)
{
  std::stringstream out;
  out << GL_BLEND;
  bool blendEnabled = callStack.FindMethodAndParams("Enable", out.str());
  return blendEnabled;
}

bool BlendDisabled(const TraceCallStack& callStack)
{
  std::stringstream out;
  out << GL_BLEND;
  bool blendEnabled = callStack.FindMethodAndParams("Disable", out.str());
  return blendEnabled;
}


