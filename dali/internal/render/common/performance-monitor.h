#ifndef __DALI_INTERNAL_PERFORMANCE_MONITOR_H__
#define __DALI_INTERNAL_PERFORMANCE_MONITOR_H__

/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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

// INTERNAL INCLUDES
#include <dali/public-api/common/map-wrapper.h>

namespace Dali
{

namespace Integration
{
class PlatformAbstraction;
}

namespace Internal
{

#define DEBUG_FREQUENCY 2  // how often to print out the performance statistics in seconds

#ifdef PERFORMANCE_MONITOR_ENABLED

#define PRINT_TIMERS      // comment to hide Frame rate and timing information
//#define PRINT_COUNTERS    // comment to hide counters
//#define PRINT_DRAW_CALLS  // comment to hide draw call counters
//#define PRINT_MATH_COUNTERS // comment to hide maths counters

#endif

class PerformanceMetric;
struct MetricInfo;

/**
 * PerformanceMonitor.
 * Used to measure the time that Dali takes to performs operations
 * and the amount of objects it processes/ state changes.
 * Uses macros so that there is not performance impact if nothing is being monitored
 *
 * To enable performance monitor you have configure dali with --enable-performance-monitor
 *
 * @todo create a graphical overlay to see performance on screen
 * instead of the terminal.
 */
class PerformanceMonitor
{
public:

  /*
   * The order in which they appear here, defines the order in which
   * they are displayed on the terminal
   */
  enum Metric
  {
    FRAME_RATE,
    NODE_COUNT,
    NODES_DRAWN,
    NODES_ADDED,
    NODES_REMOVED,
    MESSAGE_COUNT,
    MATRIX_MULTIPLYS,
    QUATERNION_TO_MATRIX,
    FLOAT_POINT_MULTIPLY,
    TEXTURE_STATE_CHANGES,
    SHADER_STATE_CHANGES,
    BLEND_MODE_CHANGES,
    GL_DRAW_CALLS,
    GL_DRAW_ELEMENTS,
    GL_DRAW_ARRAYS,
    TEXTURE_LOADS,
    TEXTURE_DATA_UPLOADED,
    VERTEX_BUFFERS_BUILT,
    INDICIE_COUNT,
    UPDATE,
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
    UPDATE_DYNAMICS
  };

  /**
   * Called once if Core is built with --enable-performance-monitor
   * @param[in] platfrom Used for querying the current time
   */
  static void Init( Integration::PlatformAbstraction& platform );

  void FrameTick();
  void Increase(Metric metricId,unsigned int value);
  void StartTimer(Metric metricId);
  void EndTimer(Metric metricId);
  void Set(Metric metricId, unsigned int Value);
  void Set(Metric metricId, float Value);
  static PerformanceMonitor *Get();

private:

  /**
   * Private constructor
   * @param[in] platfrom Used for querying the current time
   */
  PerformanceMonitor( Integration::PlatformAbstraction& platform );

private:

  Integration::PlatformAbstraction& mPlatform;

  const MetricInfo &GetMetricInfo(Metric metricId);
  PerformanceMetric *Add(Metric metricId);
  PerformanceMetric *GetMetric(Metric metricId);

  typedef std::map<int, PerformanceMetric *> mLookupTypeType;
  mLookupTypeType mMetrics;           ///< list of metrics
  unsigned int mStartSeconds;         ///< frame-time of the first tick
  unsigned int mSeconds;              ///< last second the data was printed
};

#ifdef PERFORMANCE_MONITOR_ENABLED
#define PERFORMANCE_MONITOR_INIT(x) PerformanceMonitor::Init(x);
#else
#define PERFORMANCE_MONITOR_INIT(x)
#endif

#ifdef PRINT_TIMERS
#define PERF_MONITOR_START(x)  PerformanceMonitor::Get()->StartTimer(x)
#define PERF_MONITOR_END(x)    PerformanceMonitor::Get()->EndTimer(x)
#else
#define PERF_MONITOR_START(x)
#define PERF_MONITOR_END(x)
#endif

#ifdef PRINT_COUNTERS
#define SET(x,y)  PerformanceMonitor::Get()->Set(x,y)
#define INCREASE_COUNTER(x) PerformanceMonitor::Get()->Increase(x,1);
#define INCREASE_BY(x,y) PerformanceMonitor::Get()->Increase(x,y);
#else
#define SET(x,y)
#define INCREASE_COUNTER(x)
#define INCREASE_BY(x,y)
#endif

#ifdef PRINT_MATH_COUNTERS
// for vectors/matrices
#define MATH_INCREASE_COUNTER(x)  PerformanceMonitor::Get()->Increase(x,1);
#define MATH_INCREASE_BY(x,y) PerformanceMonitor::Get()->Increase(x,y);
#else
#define MATH_INCREASE_COUNTER(x)
#define MATH_INCREASE_BY(x,y)
#endif

#ifdef PRINT_DRAW_CALLS
#define DRAW_ARRAY_RECORD(x)                                               \
PerformanceMonitor::Get()->Increase(PerformanceMonitor::GL_DRAW_ARRAYS,1); \
PerformanceMonitor::Get()->Increase(PerformanceMonitor::INDICIE_COUNT,x)

#define DRAW_ELEMENT_RECORD(x)                                               \
PerformanceMonitor::Get()->Increase(PerformanceMonitor::GL_DRAW_ELEMENTS,1); \
PerformanceMonitor::Get()->Increase(PerformanceMonitor::INDICIE_COUNT,x)
#else
#define DRAW_ARRAY_RECORD(x)
#define DRAW_ELEMENT_RECORD(x)
#endif

#if defined(PRINT_TIMERS ) || defined(PRINT_COUNTERS) || defined(PRINT_DRAW_CALLS)
#define PERF_MONITOR_NEXT_FRAME()  PerformanceMonitor::Get()->FrameTick()
#else
#define PERF_MONITOR_NEXT_FRAME()
#endif

} // namespace Internal

} // namespace Dali

#endif // __DALI_INTERNAL_PERFORMANCE_MONITOR_H_
