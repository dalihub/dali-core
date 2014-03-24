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
#include <dali/dali.h>
#include <dali-test-suite-utils.h>

using namespace Dali;

void utc_dali_render_task_list_startup(void)
{
  test_return_value = TET_UNDEF;
}

void utc_dali_render_task_list_cleanup(void)
{
  test_return_value = TET_PASS;
}

int UtcDaliRenderTaskListDefaultConstructor(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTaskList::RenderTaskList()");

  RenderTaskList taskList;

  DALI_TEST_CHECK( ! taskList );
  END_TEST;
}

int UtcDaliRenderTaskListDownCast(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTaskList::DownCast()");

  BaseHandle base = Stage::GetCurrent().GetRenderTaskList();

  RenderTaskList taskList = RenderTaskList::DownCast( base );

  DALI_TEST_CHECK( taskList );

  // Try calling a method
  DALI_TEST_CHECK( 1u == taskList.GetTaskCount() );
  END_TEST;
}

int UtcDaliRenderTaskListCreateTask(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTaskList::CreateTask()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  DALI_TEST_CHECK( 1u == taskList.GetTaskCount() );

  taskList.CreateTask();
  DALI_TEST_CHECK( 2u == taskList.GetTaskCount() );
  END_TEST;
}

int UtcDaliRenderTaskListRemoveTask(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTaskList::RemoveTask()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  DALI_TEST_CHECK( 1u == taskList.GetTaskCount() );

  RenderTask newTask = taskList.CreateTask();
  DALI_TEST_CHECK( 2u == taskList.GetTaskCount() );

  taskList.RemoveTask( newTask );
  DALI_TEST_CHECK( 1u == taskList.GetTaskCount() );
  END_TEST;
}

int UtcDaliRenderTaskListGetTaskCount(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTaskList::GetTaskCount()");

  RenderTaskList taskList = Stage::GetCurrent().GetRenderTaskList();
  DALI_TEST_CHECK( 1u == taskList.GetTaskCount() );

  taskList.RemoveTask( taskList.GetTask(0u) );
  DALI_TEST_CHECK( 0u == taskList.GetTaskCount() );
  END_TEST;
}

int UtcDaliRenderTaskListGetTask(void)
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
  END_TEST;
}
