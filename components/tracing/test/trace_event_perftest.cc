// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "base/bind.h"
#include "base/memory/ptr_util.h"
#include "base/memory/ref_counted_memory.h"
#include "base/pending_task.h"
#include "base/run_loop.h"
#include "base/test/scoped_task_environment.h"
#include "base/threading/thread.h"
#include "base/trace_event/trace_event.h"
#include "base/trace_event/traced_value.h"
#include "perf_test_helpers.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace tracing {
namespace {

using base::BindOnce;
using base::Closure;
using base::RunLoop;
using base::Thread;
using base::Unretained;
using base::WaitableEvent;
using base::trace_event::TraceConfig;
using base::trace_event::TracedValue;
using base::trace_event::TraceLog;
using base::trace_event::TraceRecordMode;

const int kNumRuns = 10;

class TraceEventPerfTest : public ::testing::Test {
 public:
  void BeginTrace() {
    TraceConfig config("*", "");
    config.SetTraceRecordMode(TraceRecordMode::RECORD_CONTINUOUSLY);
    TraceLog::GetInstance()->SetEnabled(config, TraceLog::RECORDING_MODE);
  }

  void EndTraceAndFlush() {
    ScopedStopwatch stopwatch("flush");
    base::RunLoop run_loop;
    TraceLog::GetInstance()->SetDisabled();
    TraceLog::GetInstance()->Flush(
        Bind(&OnTraceDataCollected, run_loop.QuitClosure()));
    run_loop.Run();
  }

  static void OnTraceDataCollected(
      Closure quit_closure,
      const scoped_refptr<base::RefCountedString>& events_str,
      bool has_more_events) {

    if (!has_more_events)
      quit_closure.Run();
  }

  std::unique_ptr<TracedValue> MakeTracedValue(int counter) {
    auto value = std::make_unique<TracedValue>();
    value->SetInteger("counter", counter);
    value->BeginDictionary("test_dict");
    value->BeginArray("nodes");
    for (int i = 0; i < 10; i++) {
      value->BeginDictionary();
      value->SetInteger("id", i);
      value->SetBoolean("valid", true);
      value->SetString("value", "foo");
      value->EndDictionary();
    }
    value->EndArray();
    value->SetInteger("count", 10);
    value->EndDictionary();
    return value;
  }

  static void SubmitTraceEventsAndSignal(WaitableEvent* complete_event) {
    for (int i = 0; i < 10000; i++) {
      TRACE_EVENT0("test_category", "some call");
    }
    complete_event->Signal();
  }

  static void SubmitTraceEvents(int count) {
    for (int i = 0; i < count; i++) {
      TRACE_EVENT0("test_category", "some call");
    }
  }

 private:
  base::test::ScopedTaskEnvironment scoped_task_environment;
};

TEST_F(TraceEventPerfTest, Submit_10000_TRACE_EVENT0) {
  BeginTrace();
  IterableStopwatch stopwatch("events");
  for (int lap = 0; lap < kNumRuns; lap++) {
    SubmitTraceEvents(10000);
    stopwatch.NextLap();
  }
  EndTraceAndFlush();
}

TEST_F(TraceEventPerfTest, Long_TRACE_EVENT0) {
  BeginTrace();
  IterableStopwatch stopwatch("long_event");
  for (int lap = 0; lap < kNumRuns; lap++) {
    TRACE_EVENT0("test_category", "Outer event");
    SubmitTraceEvents(10000);
    stopwatch.NextLap();
  }
  EndTraceAndFlush();
}

TEST_F(TraceEventPerfTest, Create_10000_TracedValue) {
  std::unique_ptr<TracedValue> value;
  {
    ScopedStopwatch value_sw("create_traced_values");
    for (int i = 0; i < 10000; i++) {
      value = MakeTracedValue(i);
    }
  }
}

TEST_F(TraceEventPerfTest, Submit_10000_TRACE_EVENT_with_TracedValue) {
  BeginTrace();
  // Time reported by this timer includes TracedValue creation as well.
  IterableStopwatch trace_sw("events_with_value");
  for (int lap = 0; lap < kNumRuns; lap++) {
    for (int i = 0; i < 10000; i++) {
      TRACE_EVENT_INSTANT1("test_category", "event_with_value",
          TRACE_EVENT_SCOPE_THREAD, "value", MakeTracedValue(i));
    }
    trace_sw.NextLap();
  }
  EndTraceAndFlush();
}

TEST_F(TraceEventPerfTest, Submit_10000_TRACE_EVENT0_multithreaded) {
  BeginTrace();
  const int kNumThreads = 4;

  std::vector<std::unique_ptr<Thread>> threads;
  std::vector<std::unique_ptr<WaitableEvent>> complete_events;

  for (int i = 0; i < kNumThreads; i++) {
    Thread* thread = new Thread(std::string("thread_%d") + std::to_string(i));
    WaitableEvent* complete_event =
        new WaitableEvent(WaitableEvent::ResetPolicy::AUTOMATIC,
                          WaitableEvent::InitialState::NOT_SIGNALED);
    threads.push_back(base::WrapUnique(thread));
    complete_events.push_back(base::WrapUnique(complete_event));
    thread->Start();
  }

  {
    ScopedStopwatch stopwatch("events_over_multiple_threads");
    for (int i = 0; i < kNumThreads; i++) {
      threads[i]->task_runner()->PostTask(
          FROM_HERE, base::BindOnce(&SubmitTraceEventsAndSignal,
                                    complete_events[i].get()));
    }
    for (int i = 0; i < kNumThreads; i++) {
      complete_events[i]->Wait();
    }
  }

  EndTraceAndFlush();
  for (int i = 0; i < kNumThreads; i++) {
    threads[i]->Stop();
  }
}

TEST_F(TraceEventPerfTest, Submit_10000_TRACE_EVENT0_in_traceable_tasks) {
  BeginTrace();
  IterableStopwatch task_sw("events_in_task");
  for (int i = 0; i < 100; i++) {
    base::PendingTask pending_task(FROM_HERE,
                                   BindOnce(&SubmitTraceEvents, 10000));
    TRACE_TASK_EXECUTION("TraceEventPerfTest::PendingTask", pending_task);
    std::move(pending_task.task).Run();
    task_sw.NextLap();
  }
  EndTraceAndFlush();
}

TEST_F(TraceEventPerfTest, Submit_10000_TRACE_EVENT0_with_tracing_disabled) {
  ScopedStopwatch stopwatch("events");
  SubmitTraceEvents(10000);
}

}  // namespace
}  // namespace tracing
