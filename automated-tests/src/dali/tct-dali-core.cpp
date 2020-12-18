#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <test-harness.h>

#include "tct-dali-core.h"

int main(int argc, char* const argv[])
{
  return TestHarness::RunTests(argc, argv, tc_array);
}
