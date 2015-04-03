#ifndef __DALI_INTERNAL_PERFORMANCE_MONITOR_H__
#define __DALI_INTERNAL_PERFORMANCE_MONITOR_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

namespace Dali
{

namespace Internal
{



/**
 * @brief PerformanceMonitor.
 * Empty stubs should we decided to hook into monitoring counters
 */
class PerformanceMonitor
{
public:

  /*
   * Metric enums
   */
  enum Metric
  {
    FRAME_RATE,
    MATRIX_MULTIPLYS,
    QUATERNION_TO_MATRIX,
    FLOAT_POINT_MULTIPLY,
    RESET_PROPERTIES,
    PROCESS_MESSAGES,
    ANIMATE_NODES,
    ANIMATORS_APPLIED,
    APPLY_CONSTRAINTS,
    CONSTRAINTS_APPLIED,
    CONSTRAINTS_SKIPPED,
    UPDATE_NODES,
    PREPARE_RENDERABLES,
    PROCESS_RENDER_TASKS,
    DRAW_NODES,
  };
};

#define PERFORMANCE_MONITOR_INIT(x)
#define PERF_MONITOR_START(x)     // start of timed event
#define PERF_MONITOR_END(x)       // end of a timed event
#define INCREASE_COUNTER(x)       // increase a counter by 1
#define INCREASE_BY(x,y)          // increase a count by x
#define MATH_INCREASE_COUNTER(x)  // increase a math counter ( MATRIX_MULTIPLYS, QUATERNION_TO_MATRIX, FLOAT_POINT_MULTIPLY)
#define MATH_INCREASE_BY(x,y)     // increase a math counter by x
#define PERF_MONITOR_NEXT_FRAME() // update started rendering a new frame

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_PERFORMANCE_MONITOR_H_
