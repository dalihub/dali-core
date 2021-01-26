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
 */

#include "test-harness.h"

#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <testcase.h>

#include <getopt.h>
#include <unistd.h>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>

using std::chrono::steady_clock;
using std::chrono::system_clock;

namespace TestHarness
{
typedef std::map<int32_t, TestCase> RunningTestCases;

const double MAXIMUM_CHILD_LIFETIME(60.0f); // 1 minute

const char* basename(const char* path)
{
  const char* ptr   = path;
  const char* slash = NULL;
  for(; *ptr != '\0'; ++ptr)
  {
    if(*ptr == '/') slash = ptr;
  }
  if(slash != NULL) ++slash;
  return slash;
}

std::vector<std::string> Split(const std::string& aString, char delimiter)
{
  std::vector<std::string> tokens;
  std::string              token;
  std::istringstream       tokenStream(aString);
  while(std::getline(tokenStream, token, delimiter))
  {
    tokens.push_back(token);
  }
  return tokens;
}

std::string Join(const std::vector<std::string>& tokens, char delimiter)
{
  std::ostringstream oss;

  unsigned int delimiterCount = 0;
  for(auto& token : tokens)
  {
    oss << token;
    if(delimiterCount < tokens.size() - 1)
    {
      oss << delimiter;
    }
    ++delimiterCount;
  }
  return oss.str();
}

std::string ChildOutputFilename(int pid)
{
  std::ostringstream os;
  os << "/tmp/tct-child." << pid;
  return os.str();
}

std::string TestModuleFilename(const char* processName)
{
  auto pathComponents = Split(processName, '/');
  auto aModule        = pathComponents.back();
  aModule += "-tests.xml";
  return aModule;
}

std::string TestModuleName(const char* processName)
{
  auto pathComponents   = Split(processName, '/');
  auto aModule          = pathComponents.back();
  auto moduleComponents = Split(aModule, '-');

  moduleComponents[1][0] = std::toupper(moduleComponents[1][0]);
  moduleComponents[2][0] = std::toupper(moduleComponents[2][0]);

  std::ostringstream oss;
  for(unsigned int i = 1; i < moduleComponents.size() - 1; ++i) // [0]=tct, [n-1]=core
  {
    oss << moduleComponents[i];

    if(i > 1 && i < moduleComponents.size() - 2) // skip first and last delimiter
    {
      oss << '-';
    }
  }

  return oss.str();
}

std::string ReadAndEscape(std::string filename)
{
  std::ostringstream os;
  std::ifstream      ifs;
  ifs.open(filename, std::ifstream::in);
  while(ifs.good())
  {
    std::string line;
    std::getline(ifs, line);
    for(auto c : line)
    {
      switch(c)
      {
        case '<':
          os << "&lt;";
          break;
        case '>':
          os << "&gt;";
          break;
        case '&':
          os << "&amp;";
          break;
        default:
          os << c;
          break;
      }
    }
    os << "\\"
       << "n";
  }
  ifs.close();
  return os.str();
}

void OutputTestResult(
  std::ofstream& ofs,
  const char*    pathToExecutable,
  std::string    testSuiteName,
  TestCase&      testCase,
  std::string    startTime,
  std::string    endTime)
{
  std::string outputFilename = ChildOutputFilename(testCase.childPid);
  std::string testOutput     = ReadAndEscape(outputFilename);

  ofs << "<testcase component=\"CoreAPI/" << testSuiteName << "/default\" execution_type=\"auto\" id=\""
      << testCase.name << "\" purpose=\"\" result=\"" << (testCase.result == 0 ? "PASS" : "FAIL") << "\">" << std::endl
      << "<description><test_script_entry test_script_expected_result=\"0\">"
      << pathToExecutable << testCase.name << "</test_script_entry>" << std::endl
      << "</description>"
      << "<result_info><actual_result>" << (testCase.result == 0 ? "PASS" : "FAIL") << "</actual_result>" << std::endl
      << "<start>" << startTime << "</start>"
      << "<end>" << endTime << "</end>"
      << "<stdout><![CDATA[]]></stdout>"
      << "<stderr><![CDATA[" << testOutput << "]]></stderr></result_info></testcase>" << std::endl;

  unlink(outputFilename.c_str());
}

void OutputTestResults(const char* processName, RunningTestCases& children)
{
  std::ofstream ofs;
  std::string   filename   = TestModuleFilename(processName);
  std::string   moduleName = TestModuleName(processName);
  ofs.open(filename, std::ofstream::out | std::ofstream::app);

  // Sort completed cases by original test case id
  std::vector<TestCase> childTestCases;
  childTestCases.reserve(children.size());
  for(auto& element : children) childTestCases.push_back(element.second);
  std::sort(childTestCases.begin(), childTestCases.end(), [](const TestCase& a, const TestCase& b) {
    return a.testCase < b.testCase;
  });

  const int BUFSIZE = 256;
  char      buffer[BUFSIZE];
  for(auto& testCase : childTestCases)
  {
    auto tt = system_clock::to_time_t(testCase.startSystemTime);
    strftime(buffer, BUFSIZE, "%c", localtime(&tt));
    std::string startTime(buffer);
    OutputTestResult(ofs, processName, moduleName, testCase, startTime, startTime);
  }

  ofs.close();
}

void OutputStatistics(const char* processName, int32_t numPasses, int32_t numFailures)
{
  FILE* fp = fopen("summary.xml", "a");
  if(fp != NULL)
  {
    fprintf(fp,
            "  <suite name=\"%s-tests\">\n"
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
            numPasses + numFailures,
            numPasses,
            (float)numPasses * 100.0f / (numPasses + numFailures),
            numFailures,
            (float)numFailures * 100.0f / (numPasses + numFailures));
    fclose(fp);
  }
}

int32_t RunTestCase(struct ::testcase_s& testCase)
{
  int32_t result = EXIT_STATUS_TESTCASE_FAILED;

  // dont want to catch exception as we want to be able to get
  // gdb stack trace from the first error
  // by default tests should all always pass with no exceptions
  if(testCase.startup)
  {
    testCase.startup();
  }
  try
  {
    result = testCase.function();
  }
  catch(const char*)
  {
    // just catch test fail exception, return is already set to EXIT_STATUS_TESTCASE_FAILED
  }
  if(testCase.cleanup)
  {
    testCase.cleanup();
  }

  return result;
}

int32_t RunTestCaseRedirectOutput(TestCase& testCase, bool suppressOutput)
{
  // Executing in child process
  // Close stdout and stderr to suppress the log output
  close(STDOUT_FILENO); // File descriptor number for stdout is 1

  // The POSIX specification requires that /dev/null must be provided,
  // The open function always chooses the lowest unused file descriptor
  // It is sufficient for stdout to be writable.
  open("/dev/null", O_WRONLY); // Redirect file descriptor number 1 (i.e. stdout) to /dev/null

  fflush(stderr);
  close(STDERR_FILENO);
  if(suppressOutput)
  {
    stderr = fopen("/dev/null", "w+"); // Redirect fd 2 to /dev/null
  }
  else
  {
    // When stderr is opened it must be both readable and writable.
    std::string childOutputFilename = ChildOutputFilename(getpid());
    stderr                          = fopen(childOutputFilename.c_str(), "w+");
  }

  int32_t status = RunTestCase(*testCase.tctPtr);

  fflush(stderr);
  fclose(stderr);

  return status;
}

int32_t RunTestCaseInChildProcess(TestCase& testCase, bool redirect)
{
  int32_t testResult = EXIT_STATUS_TESTCASE_FAILED;

  int32_t pid = fork();
  if(pid == 0) // Child process
  {
    if(redirect)
    {
      int status = RunTestCaseRedirectOutput(testCase, false);
      exit(status);
    }
    else
    {
      int status = RunTestCase(*testCase.tctPtr);
      exit(status);
    }
  }
  else if(pid == -1)
  {
    perror("fork");
    exit(EXIT_STATUS_FORK_FAILED);
  }
  else // Parent process
  {
    int32_t status    = 0;
    int32_t childPid  = waitpid(pid, &status, 0);
    testCase.childPid = childPid;
    if(childPid == -1)
    {
      perror("waitpid");
      exit(EXIT_STATUS_WAITPID_FAILED);
    }
    if(WIFEXITED(status))
    {
      if(childPid > 0)
      {
        testResult = WEXITSTATUS(status);
        if(testResult)
        {
          printf("Test case %s failed: %d\n", testCase.name, testResult);
        }
      }
    }
    else if(WIFSIGNALED(status))
    {
      int32_t signal = WTERMSIG(status);
      testResult     = EXIT_STATUS_TESTCASE_ABORTED;
      if(signal == SIGABRT)
      {
        printf("Test case %s failed: test case asserted\n", testCase.name);
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

int32_t RunAll(const char* processName, ::testcase tc_array[], bool quiet)
{
  int32_t       numFailures = 0;
  int32_t       numPasses   = 0;
  std::ofstream ofs;
  std::string   filename   = TestModuleFilename(processName);
  std::string   moduleName = TestModuleName(processName);
  ofs.open(filename, std::ofstream::out | std::ofstream::app);
  const int BUFSIZE = 256;
  char      buffer[BUFSIZE];

  // Run test cases in child process( to handle signals ), but run serially.
  for(uint32_t i = 0; tc_array[i].name; i++)
  {
    auto tt = system_clock::to_time_t(system_clock::now());
    strftime(buffer, BUFSIZE, "%c", localtime(&tt));
    std::string startTime(buffer);

    TestCase testCase(i, &tc_array[i]);
    testCase.result = RunTestCaseInChildProcess(testCase, quiet);

    tt = system_clock::to_time_t(system_clock::now());
    strftime(buffer, BUFSIZE, "%c", localtime(&tt));
    std::string endTime(buffer);

    if(testCase.result == 0)
    {
      numPasses++;
    }
    else
    {
      numFailures++;
    }
    if(!quiet)
    {
      OutputTestResult(ofs, processName, moduleName, testCase, startTime, endTime);
    }
  }
  ofs.close();

  OutputStatistics(processName, numPasses, numFailures);

  return numFailures;
}

// Constantly runs up to MAX_NUM_CHILDREN processes
int32_t RunAllInParallel(const char* processName, ::testcase tc_array[], bool reRunFailed, bool quiet)
{
  int32_t numFailures = 0;
  int32_t numPasses   = 0;

  RunningTestCases     children;
  std::vector<int32_t> failedTestCases;

  // Fork up to MAX_NUM_CHILDREN processes, then
  // wait. As soon as a proc completes, fork the next.

  int32_t nextTestCase       = 0;
  int32_t numRunningChildren = 0;

  while(tc_array[nextTestCase].name || numRunningChildren > 0)
  {
    // Create more children (up to the max number or til the end of the array)
    while(numRunningChildren < MAX_NUM_CHILDREN && tc_array[nextTestCase].name)
    {
      int32_t pid = fork();
      if(pid == 0) // Child process
      {
        TestCase testCase(nextTestCase, &tc_array[nextTestCase]);
        int      status = RunTestCaseRedirectOutput(testCase, quiet);
        exit(status);
      }
      else if(pid == -1)
      {
        perror("fork");
        exit(EXIT_STATUS_FORK_FAILED);
      }
      else // Parent process
      {
        TestCase tc(nextTestCase, tc_array[nextTestCase].name);
        tc.startTime       = steady_clock::now();
        tc.startSystemTime = system_clock::now();
        tc.childPid        = pid;

        children[pid] = tc;
        nextTestCase++;
        numRunningChildren++;
      }
    }

    // Check to see if any children have finished yet
    int32_t status   = 0;
    int32_t childPid = waitpid(-1, &status, WNOHANG);
    if(childPid == 0)
    {
      // No children have finished.
      // Check if any have exceeded execution time
      auto endTime = std::chrono::steady_clock::now();

      for(auto& tc : children)
      {
        std::chrono::steady_clock::duration timeSpan = endTime - tc.second.startTime;
        std::chrono::duration<double>       seconds  = std::chrono::duration_cast<std::chrono::duration<double>>(timeSpan);
        if(seconds.count() > MAXIMUM_CHILD_LIFETIME)
        {
          // Kill the child process. A subsequent call to waitpid will process signal result below.
          kill(tc.first, SIGKILL);
        }
      }
    }
    else if(childPid == -1) // waitpid errored
    {
      perror("waitpid");
      exit(EXIT_STATUS_WAITPID_FAILED);
    }
    else // a child has finished
    {
      if(WIFEXITED(status))
      {
        auto& testCase  = children[childPid];
        testCase.result = WEXITSTATUS(status);
        if(testCase.result)
        {
          printf("Test case %s failed: %d\n", testCase.name, testCase.result);
          failedTestCases.push_back(testCase.testCase);
          numFailures++;
        }
        else
        {
          numPasses++;
        }
        numRunningChildren--;
      }

      else if(WIFSIGNALED(status) || WIFSTOPPED(status))
      {
        status = WIFSIGNALED(status) ? WTERMSIG(status) : WSTOPSIG(status);

        RunningTestCases::iterator iter = children.find(childPid);
        if(iter != children.end())
        {
          printf("Test case %s exited with signal %s\n", iter->second.name, strsignal(status));
          iter->second.result = 1;
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

  if(!quiet)
  {
    OutputTestResults(processName, children);
  }

  OutputStatistics(processName, numPasses, numFailures);

  if(reRunFailed)
  {
    for(uint32_t i = 0; i < failedTestCases.size(); i++)
    {
      char*   testCaseStrapline;
      int32_t numChars = asprintf(&testCaseStrapline, "Test case %s", tc_array[failedTestCases[i]].name);
      printf("\n%s\n", testCaseStrapline);
      for(int32_t j = 0; j < numChars; j++)
      {
        printf("=");
      }
      printf("\n");
      int      index = failedTestCases[i];
      TestCase testCase(index, &tc_array[index]);
      RunTestCaseInChildProcess(testCase, false);
    }
  }

  return numFailures;
}

int32_t FindAndRunTestCase(::testcase tc_array[], const char* testCaseName)
{
  int32_t result = EXIT_STATUS_TESTCASE_NOT_FOUND;

  for(int32_t i = 0; tc_array[i].name; i++)
  {
    if(!strcmp(testCaseName, tc_array[i].name))
    {
      return RunTestCase(tc_array[i]);
    }
  }

  printf("Unknown testcase name: \"%s\"\n", testCaseName);
  return result;
}

void Usage(const char* program)
{
  printf(
    "Usage: \n"
    "   %s <testcase name>\t\t Execute a test case\n"
    "   %s \t\t Execute all test cases in parallel\n"
    "   %s -r\t\t Execute all test cases in parallel, rerunning failed test cases\n"
    "   %s -s\t\t Execute all test cases serially\n"
    "   %s -q\t\t Run without output\n",
    program,
    program,
    program,
    program,
    program);
}

int RunTests(int argc, char* const argv[], ::testcase tc_array[])
{
  int         result    = TestHarness::EXIT_STATUS_BAD_ARGUMENT;
  const char* optString = "sfq";
  bool        optRerunFailed(true);
  bool        optRunSerially(false);
  bool        optQuiet(false);

  int nextOpt = 0;
  do
  {
    nextOpt = getopt(argc, argv, optString);
    switch(nextOpt)
    {
      case 'f':
        optRerunFailed = false;
        break;
      case 's':
        optRunSerially = true;
        break;
      case 'q':
        optQuiet = true;
        break;
      case '?':
        TestHarness::Usage(argv[0]);
        exit(TestHarness::EXIT_STATUS_BAD_ARGUMENT);
        break;
    }
  } while(nextOpt != -1);

  if(optind == argc) // no testcase name in argument list
  {
    if(optRunSerially)
    {
      result = TestHarness::RunAll(argv[0], tc_array, optQuiet);
    }
    else
    {
      result = TestHarness::RunAllInParallel(argv[0], tc_array, optRerunFailed, optQuiet);
    }
  }
  else
  {
    // optind is index of next argument - interpret as testcase name
    result = TestHarness::FindAndRunTestCase(tc_array, argv[optind]);
  }
  return result;
}

} // namespace TestHarness
