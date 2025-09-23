/*
 * Copyright (c) 2025 Samsung Electronics Co., Ltd.
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
#include <dali/devel-api/threading/thread-pool.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <type_traits>

namespace
{
Dali::ThreadPool gThreadPool;

// Helper function dividing workload into N batches
// the loop lambda contains
Dali::UniqueFutureGroup ForEachMT(Dali::ThreadPool*                                 pThreadPool,
                                  uint32_t                                          first,
                                  uint32_t                                          size,
                                  std::function<void(uint32_t, uint32_t, uint32_t)> task)
{
  uint32_t   i           = 0;
  uint32_t   j           = 0;
  const auto workerCount = uint32_t(pThreadPool->GetWorkerCount());
  const auto step        = size / workerCount;
  j                      = workerCount + step;

  std::vector<Dali::Task> tasks;
  tasks.reserve(workerCount);

  for(auto threadIndex = 0u; threadIndex < workerCount; ++threadIndex)
  {
    Dali::Task lambda = [task, i, j](int workerIndex)
    {
      task(uint32_t(workerIndex), i, j);
    };
    tasks.emplace_back(lambda);
    i = j;
    j = i + step;
    if(j > size)
      j = size;
  }
  return pThreadPool->SubmitTasks(tasks, workerCount);
}

} // namespace

int UtcDaliThreadPoolMultipleTasks(void)
{
  // initialise global thread pool
  if(!gThreadPool.GetWorkerCount())
  {
    gThreadPool.Initialize(0u);
  }

  // populate inputs
  std::array<int, 8192> inputs;
  int                   checksum = 0;
  for(auto i = 0; i < decltype(i)(inputs.size()); ++i)
  {
    inputs[i] = i;
    checksum += i;
  }

  // allocate outputs ( number of outputs equals number of worker threads
  auto workerCount = gThreadPool.GetWorkerCount();

  std::vector<int> outputs;
  outputs.resize(workerCount);
  std::fill(outputs.begin(), outputs.end(), 0);

  // submit
  auto future = ForEachMT(&gThreadPool, 0, inputs.size(), [&inputs, &outputs](uint32_t workerIndex, uint32_t begin, uint32_t end)
  {
    for(auto i = begin; i < end; ++i)
    {
      outputs[workerIndex] += inputs[i];
    }
  });

  future->Wait();

  // check outputs
  int checksum2 = 0;
  for(auto output : outputs)
  {
    checksum2 += output;
  }

  printf("sum: %d, sum2: %d\n", checksum, checksum2);

  DALI_TEST_EQUALS(checksum, checksum2, TEST_LOCATION);

  END_TEST;
}

int UtcDaliThreadPoolSingleTask(void)
{
  // initialise global thread pool
  if(!gThreadPool.GetWorkerCount())
  {
    gThreadPool.Initialize(0u);
  }

  // some long lasting task
  int  counter = 0;
  auto task    = [&counter](int workerIndex)
  {
    for(int i = 0; i < 10; ++i)
    {
      counter++;
      usleep(16 * 1000);
    }
  };

  auto future = gThreadPool.SubmitTask(0, task);
  future->Wait();
  DALI_TEST_EQUALS(counter, 10, TEST_LOCATION);

  END_TEST;
}

int UtcDaliThreadPoolSubmitTasksCopyArray(void)
{
  // initialise global thread pool
  if(!gThreadPool.GetWorkerCount())
  {
    gThreadPool.Initialize(0u);
  }

  std::array<uint8_t, 1024 * 1024> dataSrc;
  for(auto i = 0; i < decltype(i)(dataSrc.size()); ++i)
  {
    dataSrc[i] = (std::rand() % 0xff);
  }

  std::array<uint8_t, 1024 * 1024> dataDst;

  // each task copies 1kb od data
  std::vector<Dali::Task> tasks;
  for(int i = 0; i < 1024; ++i)
  {
    auto task = [&dataSrc, &dataDst, i](int workerIndex)
    {
      for(int k = 0; k < 1024; ++k)
      {
        dataDst[i * 1024 + k] = dataSrc[i * 1024 + k];
      }
    };
    tasks.push_back(task);
  }

  DALI_TEST_EQUALS(1024, tasks.size(), TEST_LOCATION);

  gThreadPool.SubmitTasks(tasks);

  // wait for pool to finish
  gThreadPool.Wait();

  // compare arrays
  for(auto i = 0; i < decltype(i)(dataSrc.size()); ++i)
  {
    DALI_TEST_EQUALS(dataSrc[i], dataDst[i], TEST_LOCATION);
    if(dataSrc[i] != dataDst[i])
    {
      break;
    }
  }

  END_TEST;
}