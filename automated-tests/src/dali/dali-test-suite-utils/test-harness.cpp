/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

#include "test-harness.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <map>
#include <cstring>
#include <testcase.h>

namespace TestHarness
{

typedef std::map<int, TestCase> RunningTestCases;

const char* basename(const char* path)
{
  const char* ptr=path;
  const char* slash=NULL;
  for( ; *ptr != '\0' ; ++ptr )
  {
    if(*ptr == '/') slash=ptr;
  }
  if(slash != NULL) ++slash;
  return slash;
}

int RunTestCase( struct ::testcase_s& testCase )
{
  int result = EXIT_STATUS_TESTCASE_FAILED;

// dont want to catch exception as we want to be able to get
// gdb stack trace from the first error
// by default tests should all always pass with no exceptions
  if( testCase.startup )
  {
    testCase.startup();
  }
  result = testCase.function();
  if( testCase.cleanup )
  {
    testCase.cleanup();
  }

  return result;
}


int RunTestCaseInChildProcess( struct ::testcase_s& testCase, bool suppressOutput )
{
  int testResult = EXIT_STATUS_TESTCASE_FAILED;

  int pid = fork();
  if( pid == 0 ) // Child process
  {
    if( suppressOutput )
    {
      close(STDOUT_FILENO);
      close(STDERR_FILENO);
    }
    else
    {
      printf("\n");
      for(int i=0; i<80; ++i) printf("#");
      printf("\nTC: %s\n", testCase.name);
      fflush(stdout);
    }

    int status = RunTestCase( testCase );

    if( ! suppressOutput )
    {
      fflush(stdout);
      fflush(stderr);
      fclose(stdout);
      fclose(stderr);
    }
    exit( status );
  }
  else if(pid == -1)
  {
    perror("fork");
    exit(EXIT_STATUS_FORK_FAILED);
  }
  else // Parent process
  {
    int status = 0;
    int childPid = waitpid(pid, &status, 0);
    if( childPid == -1 )
    {
      perror("waitpid");
      exit(EXIT_STATUS_WAITPID_FAILED);
    }
    if( WIFEXITED(status) )
    {
      if( childPid > 0 )
      {
        testResult = WEXITSTATUS(status);
        if( testResult )
        {
          printf("Test case %s failed: %d\n", testCase.name, testResult);
        }
      }
    }
    else if(WIFSIGNALED(status) )
    {
      int signal = WTERMSIG(status);
      testResult = EXIT_STATUS_TESTCASE_ABORTED;
      if( signal == SIGABRT )
      {
        printf("Test case %s failed: test case asserted\n", testCase.name );
      }
      else
      {
        printf("Test case %s failed: exit with signal %s\n", testCase.name, strsignal(WTERMSIG(status)));
      }
    }
    else if(WIFSTOPPED(status))
    {
      printf("Test case %s failed: stopped with signal %s\n", testCase.name, strsignal(WSTOPSIG(status)));
    }
  }
  fflush(stdout);
  fflush(stderr);
  return testResult;
}

void OutputStatistics( const char* processName, int numPasses, int numFailures )
{
  FILE* fp=fopen("summary.xml", "a");
  if( fp != NULL )
  {
    fprintf( fp,
             "  <suite name=\"%s\">\n"
             "    <total_case>%d</total_case>\n"
             "    <pass_case>%d</pass_case>\n"
             "    <pass_rate>%5.2f</pass_rate>\n"
             "    <fail_case>%d</fail_case>\n"
             "    <fail_rate>%5.2f</fail_rate>\n"
             "    <block_case>0</block_case>\n"
             "    <block_rate>0.00</block_rate>\n"
             "    <na_case>0</na_case>\n"
             "    <na_rate>0.00</na_rate>\n"
             "  </suite>\n",
             basename(processName),
             numPasses+numFailures,
             numPasses,
             (float)numPasses/(numPasses+numFailures),
             numFailures,
             (float)numFailures/(numPasses+numFailures) );
    fclose(fp);
  }
}

int RunAll( const char* processName, ::testcase tc_array[] )
{
  int numFailures = 0;
  int numPasses = 0;

  // Run test cases in child process( to kill output/handle signals ), but run serially.
  for( unsigned int i=0; tc_array[i].name; i++)
  {
    int result = RunTestCaseInChildProcess( tc_array[i], false );
    if( result == 0 )
    {
      numPasses++;
    }
    else
    {
      numFailures++;
    }
  }

  OutputStatistics( processName, numPasses, numFailures);

  return numFailures;
}

// Constantly runs up to MAX_NUM_CHILDREN processes
int RunAllInParallel(  const char* processName, ::testcase tc_array[], bool reRunFailed)
{
  int numFailures = 0;
  int numPasses = 0;

  RunningTestCases children;
  std::vector<int> failedTestCases;

  // Fork up to MAX_NUM_CHILDREN processes, then
  // wait. As soon as a proc completes, fork the next.

  int nextTestCase = 0;
  int numRunningChildren = 0;

  while( tc_array[nextTestCase].name || numRunningChildren > 0)
  {
    // Create more children (up to the max number or til the end of the array)
    while( numRunningChildren < MAX_NUM_CHILDREN && tc_array[nextTestCase].name )
    {
      int pid = fork();
      if( pid == 0 ) // Child process
      {
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
        exit( RunTestCase( tc_array[nextTestCase] ) );
      }
      else if(pid == -1)
      {
        perror("fork");
        exit(EXIT_STATUS_FORK_FAILED);
      }
      else // Parent process
      {
        TestCase tc(nextTestCase, tc_array[nextTestCase].name);
        children[pid] = tc;
        nextTestCase++;
        numRunningChildren++;
      }
    }

    // Wait for the next child to finish

    int status=0;
    int childPid = waitpid(-1, &status, 0);
    if( childPid == -1 )
    {
      perror("waitpid");
      exit(EXIT_STATUS_WAITPID_FAILED);
    }

    if( WIFEXITED(status) )
    {
      if( childPid > 0 )
      {
        int testResult = WEXITSTATUS(status);
        if( testResult )
        {
          printf("Test case %s failed: %d\n", children[childPid].testCaseName, testResult);
          failedTestCases.push_back(children[childPid].testCase);
          numFailures++;
        }
        else
        {
          numPasses++;
        }
        numRunningChildren--;
      }
    }

    else if( WIFSIGNALED(status) || WIFSTOPPED(status))
    {
      status = WIFSIGNALED(status)?WTERMSIG(status):WSTOPSIG(status);

      if( childPid > 0 )
      {
        RunningTestCases::iterator iter = children.find(childPid);
        if( iter != children.end() )
        {
          printf("Test case %s exited with signal %s\n", iter->second.testCaseName, strsignal(status));
          failedTestCases.push_back(iter->second.testCase);
        }
        else
        {
          printf("Unknown child process: %d signaled %s\n", childPid, strsignal(status));
        }

        numFailures++;
        numRunningChildren--;
      }
    }
  }

  OutputStatistics( processName, numPasses, numFailures );

  if( reRunFailed )
  {
    for( unsigned int i=0; i<failedTestCases.size(); i++)
    {
      char* testCaseStrapline;
      int numChars = asprintf(&testCaseStrapline, "Test case %s", tc_array[failedTestCases[i]].name );
      printf("\n%s\n", testCaseStrapline);
      for(int j=0; j<numChars; j++)
      {
        printf("=");
      }
      printf("\n");
      RunTestCaseInChildProcess( tc_array[failedTestCases[i] ], false );
    }
  }

  return numFailures;
}



int FindAndRunTestCase(::testcase tc_array[], const char* testCaseName)
{
  int result = EXIT_STATUS_TESTCASE_NOT_FOUND;

  for( int i = 0; tc_array[i].name; i++ )
  {
    if( !strcmp(testCaseName, tc_array[i].name) )
    {
      return RunTestCase( tc_array[i] );
    }
  }

  printf("Unknown testcase name: \"%s\"\n", testCaseName);
  return result;
}

void Usage(const char* program)
{
  printf("Usage: \n"
         "   %s <testcase name>\t\t Execute a test case\n"
         "   %s \t\t Execute all test cases in parallel\n"
         "   %s -r\t\t Execute all test cases in parallel, rerunning failed test cases\n"
         "   %s -s\t\t Execute all test cases serially\n",
         program, program, program, program);
}

} // namespace
