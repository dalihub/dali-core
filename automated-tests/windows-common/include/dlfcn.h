#pragma once

#include <windows.h>

#define RTLD_LAZY 0
#define RTLD_DEEPBIND 0
#define RTLD_NEXT nullptr

struct Dl_info
{
  const char* dli_sname;
};

inline void* dlopen(const char* path, int)
{
  return reinterpret_cast<void*>(LoadLibraryA(path));
}

inline void* dlsym(void* handle, const char* name)
{
  return reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(handle), name));
}

inline int dlclose(void* handle)
{
  return FreeLibrary(reinterpret_cast<HMODULE>(handle)) ? 0 : -1;
}

inline int dladdr(const void*, Dl_info*)
{
  return 0;
}
