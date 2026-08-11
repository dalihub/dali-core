#pragma once

#include <chrono>
#include <thread>

inline int usleep(unsigned int microseconds)
{
  std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
  return 0;
}
