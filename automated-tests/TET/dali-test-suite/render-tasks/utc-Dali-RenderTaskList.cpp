//
// Copyright (c) 2014 Samsung Electronics Co., Ltd.
//
// Licensed under the Flora License, Version 1.0 (the License);
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://floralicense.org/license/
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <iostream>

#include <stdlib.h>
#include <tet_api.h>

#include <dali/public-api/dali-core.h>

#include <dali-test-suite-utils.h>

using namespace Dali;

static void Startup();
static void Cleanup();

extern "C" {
  void (*tet_startup)() = Startup;
  void (*tet_cleanup)() = Cleanup;
}

static void UtcDaliRenderTaskListDefaultConstructor();
static void UtcDaliRenderTaskListDownCast();
static void UtcDaliRenderTaskListCreateTask();
static void UtcDaliRenderTaskListRemoveTask();
static void UtcDaliRenderTaskListGetTaskCount();
static void UtcDaliRenderTaskListGetTask();

enum {
  POSITIVE_TC_IDX = 0x01,
  NEGATIVE_TC_IDX,
};

// Add test functionality for all APIs in the class (Positive and Negative)
extern "C" {
  struct tet_testlist tet_testlist[] =
  {
    { UtcDaliRenderTaskListDefaultConstructor,  POSITIVE_TC_IDX },
    { UtcDaliRenderTaskListDownCast,            POSITIVE_TC_IDX },
    { UtcDaliRenderTaskListCreateTask,          POSITIVE_TC_IDX },
    { UtcDaliRenderTaskListRemoveTask,          POSITIVE_TC_IDX },
    { UtcDaliRenderTaskListGetTaskCount,        POSITIVE_TC_IDX },
    { UtcDaliRenderTaskListGetTask,             POSITIVE_TC_IDX },

    { NULL, 0 }
  };
}

// Called only once before first test is run.
static void Startup()
{
}

// Called only once after last test is run
static void Cleanup()
{
}

static void UtcDaliRenderTaskListDefaultConstructor()
{
  TestApplication application;

  tet_infoline("Testing RenderTaskList::RenderTaskList()");

  RenderTaskList taskList;

  DALI_TEST_CHECK( ! taskList );
}

static void UtcDaliRenderTaskListDownCast()
{
  TestApplication application;

  tet_infoline("Testing RenderTaskList::DownCast()");

  BaseHandle base = Stage::GetCurrent().GetRenderTaskList();

  RenderTaskList taskList = RenderTaskList::DownCast( base );

  DALI_TEST_CHECK( taskList );

  // Try calling a method
  DALI_TEST_CHECK( 1u == taskList.GetTaskCount() );
}

static void UtcDaliRenderTaskListCreateTask()
{
  TestApplication application;

  tet_infoline("Testing RenderTaskList::CreateTask()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  DALI_TEST_CHECK( 1u == taskList.GetTaskCount() );

  taskList.CreateTask();
  DALI_TEST_CHECK( 2u == taskList.GetTaskCount() );
}

static void UtcDaliRenderTaskListRemoveTask()
{
  TestApplication application;

  tet_infoline("Testing RenderTaskList::RemoveTask()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  DALI_TEST_CHECK( 1u == taskList.GetTaskCount() );

  RenderTask newTask = taskList.CreateTask();
  DALI_TEST_CHECK( 2u == taskList.GetTaskCount() );

  taskList.RemoveTask( newTask );
  DALI_TEST_CHECK( 1u == taskList.GetTaskCount() );
}

static void UtcDaliRenderTaskListGetTaskCount()
{
  TestApplication application;

  tet_infoline("Testing RenderTaskList::GetTaskCount()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  DALI_TEST_CHECK( 1u == taskList.GetTaskCount() );

  taskList.RemoveTask( taskList.GetTask(0u) );
  DALI_TEST_CHECK( 0u == taskList.GetTaskCount() );
}

static void UtcDaliRenderTaskListGetTask()
{
  TestApplication application;

  tet_infoline("Testing RenderTaskList::GetTask()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  RenderTask defaultTask = taskList.GetTask( 0u );
  DALI_TEST_CHECK( 1u == taskList.GetTaskCount() );
  DALI_TEST_CHECK( defaultTask );
  DALI_TEST_CHECK( defaultTask == taskList.GetTask( 0u ) );

  RenderTask newTask = taskList.CreateTask();
  DALI_TEST_CHECK( 2u == taskList.GetTaskCount() );

  RenderTask temp = taskList.GetTask( 0u );
  RenderTask temp2 = taskList.GetTask( 1u );

  DALI_TEST_CHECK( newTask );
  DALI_TEST_CHECK( defaultTask != newTask );
  DALI_TEST_CHECK( taskList.GetTask( 0u ) == defaultTask );
  DALI_TEST_CHECK( taskList.GetTask( 1u ) == newTask );
  DALI_TEST_CHECK( taskList.GetTask( 1u ) != defaultTask );

  taskList.RemoveTask( taskList.GetTask(0u) );
  DALI_TEST_CHECK( 1u == taskList.GetTaskCount() );
  DALI_TEST_CHECK( taskList.GetTask( 0u ) != defaultTask  );
  DALI_TEST_CHECK( taskList.GetTask( 0u ) == newTask );
}
