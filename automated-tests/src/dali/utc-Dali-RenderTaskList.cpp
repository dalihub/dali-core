/*
 * Copyright (c) 2024 Samsung Electronics Co., Ltd.
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

#include <dali-test-suite-utils.h>
#include <dali/devel-api/threading/thread.h>
#include <dali/public-api/dali-core.h>
#include <stdlib.h>

#include <iostream>

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

  DALI_TEST_CHECK(!taskList);
  END_TEST;
}

int UtcDaliRenderTaskListCopyConstructor(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTaskList::RenderTaskList(const RenderTaskList& handle)");

  RenderTaskList taskList1;

  RenderTaskList taskList2(taskList1);

  DALI_TEST_CHECK(!taskList2);
  END_TEST;
}

int UtcDaliRenderTaskListAssignment(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTaskList::RenderTaskList(const RenderTaskList& handle)");

  RenderTaskList taskList1;

  RenderTaskList taskList2;

  taskList1 = taskList2;

  DALI_TEST_CHECK(!taskList1);
  END_TEST;
}

int UtcDaliRenderTaskListMoveConstructor(void)
{
  TestApplication application;

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  DALI_TEST_CHECK(taskList);
  DALI_TEST_EQUALS(2, taskList.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(1u == taskList.GetTaskCount());

  RenderTaskList move = std::move(taskList);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(2, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(1u == move.GetTaskCount());
  DALI_TEST_CHECK(!taskList);

  END_TEST;
}

int UtcDaliRenderTaskListMoveAssignment(void)
{
  TestApplication application;

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  DALI_TEST_CHECK(taskList);
  DALI_TEST_EQUALS(2, taskList.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(1u == taskList.GetTaskCount());

  RenderTaskList move;
  move = std::move(taskList);
  DALI_TEST_CHECK(move);
  DALI_TEST_EQUALS(2, move.GetBaseObject().ReferenceCount(), TEST_LOCATION);
  DALI_TEST_CHECK(1u == move.GetTaskCount());
  DALI_TEST_CHECK(!taskList);

  END_TEST;
}

int UtcDaliRenderTaskListDownCast(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTaskList::DownCast()");

  BaseHandle base = application.GetScene().GetRenderTaskList();

  RenderTaskList taskList = RenderTaskList::DownCast(base);

  DALI_TEST_CHECK(taskList);

  // Try calling a method
  DALI_TEST_CHECK(1u == taskList.GetTaskCount());
  END_TEST;
}

int UtcDaliRenderTaskListCreateTask(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTaskList::CreateTask()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  DALI_TEST_CHECK(1u == taskList.GetTaskCount());

  taskList.CreateTask();
  DALI_TEST_CHECK(2u == taskList.GetTaskCount());
  END_TEST;
}

int UtcDaliRenderTaskListRemoveTask(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTaskList::RemoveTask()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  DALI_TEST_CHECK(1u == taskList.GetTaskCount());

  RenderTask newTask = taskList.CreateTask();
  DALI_TEST_CHECK(2u == taskList.GetTaskCount());

  taskList.RemoveTask(newTask);
  DALI_TEST_CHECK(1u == taskList.GetTaskCount());
  END_TEST;
}

int UtcDaliRenderTaskListRemoveTaskWithExclusiveActor(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTaskList::RemoveTask() which has an exclusive actor set");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  DALI_TEST_CHECK(1u == taskList.GetTaskCount());

  RenderTask newTask = taskList.CreateTask();
  DALI_TEST_CHECK(2u == taskList.GetTaskCount());

  auto actor = CreateRenderableActor();
  newTask.SetSourceActor(actor);
  newTask.SetExclusive(true);
  DALI_TEST_EQUALS(actor, newTask.GetSourceActor(), TEST_LOCATION);
  DALI_TEST_EQUALS(true, newTask.IsExclusive(), TEST_LOCATION);
  taskList.RemoveTask(newTask);

  DALI_TEST_CHECK(1u == taskList.GetTaskCount());
  END_TEST;
}

int UtcDaliRenderTaskListGetTaskCount(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTaskList::GetTaskCount()");

  RenderTaskList taskList = application.GetScene().GetRenderTaskList();
  DALI_TEST_CHECK(1u == taskList.GetTaskCount());

  taskList.RemoveTask(taskList.GetTask(0u));
  DALI_TEST_CHECK(0u == taskList.GetTaskCount());
  END_TEST;
}

int UtcDaliRenderTaskListGetTask(void)
{
  TestApplication application;

  tet_infoline("Testing RenderTaskList::GetTask()");

  RenderTaskList taskList    = application.GetScene().GetRenderTaskList();
  RenderTask     defaultTask = taskList.GetTask(0u);
  DALI_TEST_CHECK(1u == taskList.GetTaskCount());
  DALI_TEST_CHECK(defaultTask);
  DALI_TEST_CHECK(defaultTask == taskList.GetTask(0u));

  RenderTask newTask = taskList.CreateTask();
  DALI_TEST_CHECK(2u == taskList.GetTaskCount());

  RenderTask temp  = taskList.GetTask(0u);
  RenderTask temp2 = taskList.GetTask(1u);

  DALI_TEST_CHECK(newTask);
  DALI_TEST_CHECK(defaultTask != newTask);
  DALI_TEST_CHECK(taskList.GetTask(0u) == defaultTask);
  DALI_TEST_CHECK(taskList.GetTask(1u) == newTask);
  DALI_TEST_CHECK(taskList.GetTask(1u) != defaultTask);

  taskList.RemoveTask(taskList.GetTask(0u));
  DALI_TEST_CHECK(1u == taskList.GetTaskCount());
  DALI_TEST_CHECK(taskList.GetTask(0u) != defaultTask);
  DALI_TEST_CHECK(taskList.GetTask(0u) == newTask);
  END_TEST;
}

int UtcDaliRenderTaskListCreateTaskNegative(void)
{
  TestApplication      application;
  Dali::RenderTaskList instance;
  try
  {
    instance.CreateTask();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskListRemoveTaskNegative(void)
{
  TestApplication      application;
  Dali::RenderTaskList instance;
  try
  {
    Dali::RenderTask arg1(application.GetScene().GetRenderTaskList().GetTask(0u));
    instance.RemoveTask(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskListGetTaskCountNegative(void)
{
  TestApplication      application;
  Dali::RenderTaskList instance;
  try
  {
    instance.GetTaskCount();
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskListGetTaskNegative(void)
{
  TestApplication      application;
  Dali::RenderTaskList instance;
  try
  {
    unsigned int arg1(0u);
    instance.GetTask(arg1);
    DALI_TEST_CHECK(false); // Should not get here
  }
  catch(...)
  {
    DALI_TEST_CHECK(true); // We expect an assert
  }
  END_TEST;
}

int UtcDaliRenderTaskListDestructWorkerThreadN(void)
{
  TestApplication application;
  tet_infoline("UtcDaliRenderTaskListDestructWorkerThreadN Test, for line coverage");

  try
  {
    class TestThread : public Thread
    {
    public:
      virtual void Run()
      {
        tet_printf("Run TestThread\n");
        // Destruct at worker thread.
        mRenderTaskList.Reset();
      }

      Dali::RenderTaskList mRenderTaskList;
    };
    TestThread thread;

    Dali::Integration::Scene scene = Dali::Integration::Scene::New(Size(480.0f, 800.0f));

    RenderTaskList renderTaskList = scene.GetRenderTaskList();

    thread.mRenderTaskList = std::move(renderTaskList);
    renderTaskList.Reset();

    scene.RemoveSceneObject();
    scene.Discard();
    scene.Reset();

    thread.Start();

    thread.Join();
  }
  catch(...)
  {
  }

  // Always success
  DALI_TEST_CHECK(true);

  END_TEST;
}
