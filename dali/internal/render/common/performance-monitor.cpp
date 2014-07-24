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

// CLASS HEADER
#include <dali/internal/render/common/performance-monitor.h>

// EXTERNAL INCLUDES
#include <string>
#include <time.h>
#include <climits>

// INTERNAL INCLUDES
#include <dali/public-api/common/dali-common.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/platform-abstraction.h>


#if defined(PRINT_TIMERS) || defined(PRINT_COUNTERS) || defined(PRINT_DRAW_CALLS) || defined(PRINT_MATH_COUNTERS)

namespace Dali
{

namespace Internal
{

namespace
{
PerformanceMonitor *mInstance = NULL;
const float epsilon = 0.9f; // smooth average = (average * epsilon) + (current * epsilon)

const unsigned int MICROSECONDS_PER_SECOND = 1000000;
const unsigned int NANOSECS_TO_MICROSECS = 1000;      ///< 1000 nanoseconds = 1 microsecond

/*
 * Structure similar to timespec which holds the seconds and millisecond values.
 */
struct TimeInfo
{
  unsigned int seconds;
  unsigned int microSeconds;
};

/*
 * Returns the time difference in seconds (as a float)
 */
float Diff(const TimeInfo& start, const TimeInfo& end)
{
  int diff = end.microSeconds - start.microSeconds;
  if (diff < 0)
  {
    diff += MICROSECONDS_PER_SECOND;
  }
  return (float) diff / MICROSECONDS_PER_SECOND;
}

} // unnamed namespace

/*
 * Base performance metric class, needs overloading
 */
class PerformanceMetric
{

public:

  enum MetricType
  {
    TIMER,           ///< timer
    FPS_TIMER,       ///< timer that outputs in frames per second
    COUNTER,         ///< counter that is set once per frame using SET macro
    INC_COUNTER,     ///< incremental counter which is set multiple times per frame (e.g. to count number of draw calls).
    DATA_COUNTER,    ///< used to measure changes in data, e.g. amount of texture data uploaded
  };

  PerformanceMetric(MetricType type):mType(type) {}

  virtual void SetFloat(float value) {}
  virtual void SetInt(unsigned int value) {}
  virtual void IncreaseBy(unsigned int value) {}
  virtual void Tick() {}
  virtual void Reset() {};
  virtual void Log() = 0;

  bool mEnabled;                ///< whether the metric is enabled
  MetricType mType;             ///< type of metric
  std::string mName;            ///< name
  unsigned int mMetricId;       ///< unique id
};

/*
 * Timer metric, used to measure elapsed time each frame (e.g. for frame rate)
 */
class TimerMetric : public PerformanceMetric
{
public:

  TimerMetric()
  : PerformanceMetric( TIMER ),
    mAvg( 0.0 ),
    mTotal( 0.0 ),
    mCount( 0.0f )
  {
    Reset();
  }

  void Reset()
  {
    mMin = 10.0;  // default min to 10 seconds
    mMax = 0.0;
  }

  void Log()
  {
    LogMessage( Integration::Log::DebugInfo, "%s min: %.06f, max: %.06f, total %.06f, average: %.06f, count: %.0f, mean: %f\n", mName.c_str(), mMin, mMax, mTotal, mAvg, mCount, mTotal/mCount );
  }

  float mMin;
  float mMax;
  float mAvg;
  float mTotal;
  float mCount;
  TimeInfo mTime;
};

/*
 * Timer metric, used to measure elapsed time each frame (e.g. for frame rate)
 */
class FPSTimerMetric : public TimerMetric
{
public:

  void Log()
  {
    LogMessage( Integration::Log::DebugInfo, "%s average: %04f\n", mName.c_str(), 1.0f/mAvg );
  }

};

/*
 * Counter class, used to measure things like the number of nodes drawn
 */
class CounterMetric : public PerformanceMetric
{
public:

  CounterMetric():PerformanceMetric(COUNTER) { Reset();}

  void Reset()
  {
    mMin = UINT_MAX;
    mMax = 0;
    mAvg = mLast;
  }
  /*
   * Used for values set one per-frame, e.g. number of messages
   */
  void SetInt(unsigned int value)
  {
    mLast = value;

    if (mMin > value)
    {
      mMin = value;
    }
    if (mMax < value)
    {
      mMax = value;
    }

    if (mAvg == 0)
    {
      mAvg = value;
    }
    else
    {
      mAvg =  ((float)mAvg * epsilon) + ((float)value * (1.0f - epsilon));
    }
  }
  void Log()
  {
    LogMessage(Integration::Log::DebugInfo, "%s  min: %04d, max: %04d, average: %04d\n",mName.c_str(),mMin,mMax,mAvg);
  }

  unsigned int mMin;
  unsigned int mMax;
  unsigned int mAvg;
  unsigned int mLast;

};

/*
 * Incremental Counter class, used to measure things like the number of nodes drawn
 */
class IncCounterMetric : public PerformanceMetric
{
public:
  IncCounterMetric():PerformanceMetric(INC_COUNTER){ Reset();}

  virtual void Reset()
  {
    mMin = UINT_MAX;
    mMax = 0;
    mAvg = mCurrent;
    mCurrent = 0;
  }
  /*
   * Used for values that are set multiple times a frame, e.g. number of draw calls
   */
  void IncreaseBy(unsigned int value)
  {
    mCurrent += value;
  }
  virtual void Tick()
  {
    if (mMin > mCurrent)
    {
      mMin = mCurrent;
    }
    if (mMax < mCurrent)
    {
      mMax = mCurrent;
    }

    if (mAvg == 0)
    {
      mAvg = mCurrent;
    }
    else
    {
      mAvg =  ((float)mAvg * epsilon) + ((float)mCurrent * (1.0f - epsilon));
    }

    mCurrent = 0;
  }

  virtual void Log()
  {
    LogMessage(Integration::Log::DebugInfo, "%s  min: %04d, max: %04d, average: %04d\n",mName.c_str(),mMin,mMax,mAvg);
  }

  unsigned int mMin;
  unsigned int mMax;
  unsigned int mAvg;
  unsigned int mCurrent;

};

/*
 * Data Counter class
 * used to measure things like the number of texture bytes uploaded
 */
class DataCountMetric : public IncCounterMetric
{
public:

  DataCountMetric():mTotal(0) {}

  virtual void Tick()
  {
    if (mMax < mCurrent)
    {
      mMax = mCurrent;
    }

    mTotal += mCurrent;

    mCurrent = 0;
  }

  virtual void Log()
  {
    unsigned int shift = 0;
    std::string label("bytes");

    if (mMax>>20)
    {
      shift = 20;
      label = "MegaBytes";
    }
    else if (mMax>>10)
    {
      shift = 10;
      label = "KiloBytes";
    }

    LogMessage(Integration::Log::DebugInfo, "%s  max: %01d %s, total: %01d MegaBytes since start-up \n",
               mName.c_str(),
               mMax>>shift,
               label.c_str(),
               mTotal>>20);
  }
  unsigned int mTotal;
};

/*
 * small structure used in MetricTable
 */
struct MetricInfo
{
  std::string mName;
  int mId;
  PerformanceMetric::MetricType type;
};

/*
 * table to map a metric id, to a string / metric type
 */
const MetricInfo MetricTable[] = {
  {  "NODE_COUNT            ",   PerformanceMonitor::NODE_COUNT,            PerformanceMetric::COUNTER },
  {  "NODES_DRAWN           ",   PerformanceMonitor::NODES_DRAWN,           PerformanceMetric::COUNTER },
  {  "MESSAGE_COUNT         ",   PerformanceMonitor::MESSAGE_COUNT,         PerformanceMetric::COUNTER },
  {  "NODES_ADDED           ",   PerformanceMonitor::NODES_ADDED,           PerformanceMetric::INC_COUNTER },
  {  "NODES_REMOVED         ",   PerformanceMonitor::NODES_REMOVED,         PerformanceMetric::INC_COUNTER },
  {  "ANIMATORS_APPLIED     ",   PerformanceMonitor::ANIMATORS_APPLIED,     PerformanceMetric::INC_COUNTER },
  {  "CONSTRAINTS_APPLIED   ",   PerformanceMonitor::CONSTRAINTS_APPLIED,   PerformanceMetric::INC_COUNTER },
  {  "CONSTRAINTS_SKIPPED   ",   PerformanceMonitor::CONSTRAINTS_SKIPPED,   PerformanceMetric::INC_COUNTER },
  {  "TEXTURE_STATE_CHANGES ",   PerformanceMonitor::TEXTURE_STATE_CHANGES, PerformanceMetric::INC_COUNTER },
  {  "SHADER_STATE_CHANGES  ",   PerformanceMonitor::SHADER_STATE_CHANGES,  PerformanceMetric::INC_COUNTER },
  {  "BLEND_MODE_CHANGES    ",   PerformanceMonitor::BLEND_MODE_CHANGES,    PerformanceMetric::INC_COUNTER },
  {  "INDICIES              ",   PerformanceMonitor::INDICIE_COUNT,         PerformanceMetric::INC_COUNTER },
  {  "GL_DRAW_CALLS         ",   PerformanceMonitor::GL_DRAW_CALLS,         PerformanceMetric::INC_COUNTER },
  {  "GL_DRAW_ELEMENTS      ",   PerformanceMonitor::GL_DRAW_ELEMENTS,      PerformanceMetric::INC_COUNTER },
  {  "GL_DRAW_ARRAYS        ",   PerformanceMonitor::GL_DRAW_ARRAYS,        PerformanceMetric::INC_COUNTER },
  {  "TEXTURE_LOADS         ",   PerformanceMonitor::TEXTURE_LOADS,         PerformanceMetric::INC_COUNTER },
  {  "TEXTURE_DATA_UPLOADED ",   PerformanceMonitor::TEXTURE_DATA_UPLOADED, PerformanceMetric::DATA_COUNTER },
  {  "VERTEX_BUFFERS_BUILT  ",   PerformanceMonitor::VERTEX_BUFFERS_BUILT,  PerformanceMetric::INC_COUNTER },
  {  "QUATERNION_TO_MATRIX  ",   PerformanceMonitor::QUATERNION_TO_MATRIX,  PerformanceMetric::INC_COUNTER },
  {  "MATRIX_MULTIPLYS      ",   PerformanceMonitor::MATRIX_MULTIPLYS,      PerformanceMetric::INC_COUNTER },
  {  "FLOAT_POINT_MULTIPLY  ",   PerformanceMonitor::FLOAT_POINT_MULTIPLY,  PerformanceMetric::INC_COUNTER },
  {  "UPDATE                ",   PerformanceMonitor::UPDATE,                PerformanceMetric::TIMER },
  {  "RESET_PROPERTIES      ",   PerformanceMonitor::RESET_PROPERTIES,      PerformanceMetric::TIMER },
  {  "PROCESS_MESSAGES      ",   PerformanceMonitor::PROCESS_MESSAGES,      PerformanceMetric::TIMER },
  {  "ANIMATE_NODES         ",   PerformanceMonitor::ANIMATE_NODES,         PerformanceMetric::TIMER },
  {  "APPLY_CONSTRAINTS     ",   PerformanceMonitor::APPLY_CONSTRAINTS,     PerformanceMetric::TIMER },
  {  "UPDATE_AND_SORT_NODES ",   PerformanceMonitor::UPDATE_NODES,          PerformanceMetric::TIMER },
  {  "PREPARE_RENDERABLES   ",   PerformanceMonitor::PREPARE_RENDERABLES,   PerformanceMetric::TIMER },
  {  "PROCESS_RENDER_TASKS  ",   PerformanceMonitor::PROCESS_RENDER_TASKS,  PerformanceMetric::TIMER },
  {  "DRAW_NODES            ",   PerformanceMonitor::DRAW_NODES,            PerformanceMetric::TIMER },
  {  "FRAME_RATE            ",   PerformanceMonitor::FRAME_RATE,            PerformanceMetric::FPS_TIMER }
};

namespace
{
const unsigned int NUM_METRICS = sizeof(MetricTable)/sizeof(MetricInfo);
} // unnamed namespace

void PerformanceMonitor::Init( Integration::PlatformAbstraction& platform )
{
   if( NULL == mInstance )
   {
     mInstance = new PerformanceMonitor( platform );
   }
}

PerformanceMonitor::PerformanceMonitor( Integration::PlatformAbstraction& platform )
: mPlatform( platform ),
  mStartSeconds( 0 ),
  mSeconds( 0 )
{
}

PerformanceMonitor* PerformanceMonitor::Get()
{
  DALI_ASSERT_ALWAYS( NULL != mInstance );
  return mInstance;
}

PerformanceMetric *PerformanceMonitor::Add(Metric metricId)
{
  PerformanceMetric *metric = NULL;
  const MetricInfo &info = GetMetricInfo(metricId);

  switch (info.type)
  {
    case PerformanceMetric::FPS_TIMER   : metric = new FPSTimerMetric();   break;
    case PerformanceMetric::TIMER       : metric = new TimerMetric();      break;
    case PerformanceMetric::COUNTER     : metric = new CounterMetric();    break;
    case PerformanceMetric::INC_COUNTER : metric = new IncCounterMetric(); break;
    case PerformanceMetric::DATA_COUNTER: metric = new DataCountMetric();  break;
    default : DALI_ASSERT_ALWAYS( 0 && "PerformanceMetric enumeration out of bounds");
  }

  metric->mMetricId = metricId;
  metric->mType = info.type;
  metric->mName = info.mName;
  metric->mEnabled = true;
  mMetrics[metricId] = metric;

  return metric;
}

PerformanceMetric *PerformanceMonitor::GetMetric(Metric metricId)
{
  mLookupTypeType::const_iterator item=mMetrics.find(metricId);
  if (item!=mMetrics.end())
  {
    return ((*item).second);
  }
  // metric not found, so add it
  return Add(metricId);
}

void PerformanceMonitor::Set(Metric metricId, float value)
{
  PerformanceMetric *metric = GetMetric(metricId);

  metric->SetFloat(value);
}

void PerformanceMonitor::Set(Metric metricId, unsigned int value)
{
  PerformanceMetric *metric = GetMetric(metricId);

  metric->SetInt(value);
}

void PerformanceMonitor::Increase(Metric metricId,unsigned int value)
{
  PerformanceMetric *metric = GetMetric(metricId);

  metric->IncreaseBy(value);
}

void PerformanceMonitor::StartTimer(Metric metricId)
{
  TimerMetric *timer = static_cast<TimerMetric *>(GetMetric(metricId));

  TimeInfo& timeInfo = timer->mTime;
  mPlatform.GetTimeMicroseconds(timeInfo.seconds, timeInfo.microSeconds);
}

void PerformanceMonitor::EndTimer(Metric metricId)
{
  TimerMetric *timer = static_cast<TimerMetric *>(GetMetric(metricId));
  TimeInfo endTime;

  mPlatform.GetTimeMicroseconds(endTime.seconds, endTime.microSeconds);

  // frame time in seconds
  float elapsedTime = Diff(timer->mTime, endTime);

  if (elapsedTime < timer->mMin)
  {
    timer->mMin = elapsedTime;
  }
  if (elapsedTime > timer->mMax)
  {
    timer->mMax = elapsedTime;
  }

  timer->mTotal += elapsedTime;

  timer->mCount += 1.0f;

  timer->mAvg = (elapsedTime * (1.0f - epsilon)) + (timer->mAvg * epsilon);
}

const MetricInfo &PerformanceMonitor::GetMetricInfo(Metric metricId)
{
  for (unsigned int i = 0; i < NUM_METRICS; i++)
  {
    if (MetricTable[i].mId == metricId)
    {
      return MetricTable[i];
    }
  }
  DALI_ASSERT_ALWAYS(0 && "metricId not found");
  return MetricTable[0];
}

void PerformanceMonitor::FrameTick()
{
  unsigned int currentTimeSeconds;
  unsigned int currentTimeMicroSeconds;
  mLookupTypeType::const_iterator item;

  mPlatform.GetTimeMicroseconds(currentTimeSeconds, currentTimeMicroSeconds);

  // incremental counters need to know when a frame has been done
  // to store min/max/average values

  for (item = mMetrics.begin(); item!=mMetrics.end(); item++)
  {
    PerformanceMetric *metric=(*item).second;
    metric->Tick();
  }

  // only display info every other second
  if ( 0 == mSeconds )
  {
    // This is the first tick
    mStartSeconds = currentTimeSeconds;
  }
  else if ( currentTimeSeconds < (mSeconds + DEBUG_FREQUENCY))
  {
    return;
  }

  mSeconds = currentTimeSeconds;

  LogMessage(Integration::Log::DebugInfo, "--------------------------- %d\n", mSeconds - mStartSeconds);
  for (item = mMetrics.begin(); item!=mMetrics.end(); item++)
  {
    PerformanceMetric *metric=(*item).second;
    if (metric->mEnabled)
    {
      metric->Log();
      metric->Reset();
    }
  }
}

} // namespace Internal

} // namespace Dali


#endif
