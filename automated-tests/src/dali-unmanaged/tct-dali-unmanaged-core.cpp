#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <test-harness.h>
#include "tct-dali-unmanaged-core.h"

int main(int argc, char * const argv[])
{
  int result = TestHarness::EXIT_STATUS_BAD_ARGUMENT;

  const char* optString = "r";
  bool optRerunFailed(false);

  int nextOpt = 0;
  do
  {
    nextOpt = getopt( argc, argv, optString );
    switch(nextOpt)
    {
      case 'r':
        optRerunFailed = true;
        break;
      case '?':
        TestHarness::Usage(argv[0]);
        exit(TestHarness::EXIT_STATUS_BAD_ARGUMENT);
        break;
    }
  } while( nextOpt != -1 );

  if( optind == argc ) // no testcase name in argument list
  {
    result = TestHarness::RunAllInParallel(argv[0], tc_array, optRerunFailed);
  }
  else
  {
    // optind is index of next argument - interpret as testcase name
    result = TestHarness::FindAndRunTestCase(tc_array, argv[optind]);
  }
  return result;
}
