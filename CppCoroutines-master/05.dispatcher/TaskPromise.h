//
// Created by benny on 2022/3/17.
//

#ifndef CPPCOROUTINES_TASKS_04_TASK_TASKPROMISE_H_
#define CPPCOROUTINES_TASKS_04_TASK_TASKPROMISE_H_

#include <functional>
#include <mutex>
#include <list>
#include <optional>

#include "coroutine_common.h"
#include "Result.h"
#include "DispatchAwaiter.h"
#include "TaskAwaiter.h"

template<typename ResultType, typename Executor>
class Task;

template<typename ResultType, typename Executor>
struct TaskPromise {
  DispatchAwaiter initial_suspend() { return DispatchAwaiter{&executor}; }

  std::suspend_always final_suspend() noexcept { return {}; }

  Task<ResultType, Executor> get_return_object() {
    return Task{std::coroutine_handle<TaskPromise>::from_promise(*this)};
  }

  template<typename _ResultType, typename _Executor>
  TaskAwaiter<_ResultType, _Executor> await_transform(Task<_ResultType, _Executor> &&task) {
    return TaskAwaiter<_ResultType, _Executor>(&executor, std::move(task));
  }

  void unhandled_exception() {
    std::lock_guard lock(completion_lock);
    result = Result<ResultType>(std::current_exception());
    completion.notify_all();
    notify_callbacks();
  }

  void return_value(ResultType value) {
    debug("return_value");
    std::lock_guard lock(completion_lock);
    result = Result<ResultType>(std::move(value));
    completion.notify_all();
    notify_callbacks();
  }

  ResultType get_result() {
    // blocking for result or throw on exception
    std::unique_lock lock(completion_lock);
    if (!result.has_value()) {
      debug("get_result waiting...");
      completion.wait(lock);
      //debug("get_result finish waiting");
    }
    return result->get_or_throw();
  }

  void on_completed(std::function<void(Result<ResultType>)> &&func) {
    std::unique_lock lock(completion_lock);
    if (result.has_value()) {
      debug("on_complated is ready");
      auto value = result.value();
      lock.unlock();
      func(value);
    } else {
      debug("on_complated is not ready");
      completion_callbacks.push_back(func);
    }
  }

 private:
  std::optional<Result<ResultType>> result;

  std::mutex completion_lock;
  std::condition_variable completion;

  std::list<std::function<void(Result<ResultType>)>> completion_callbacks;

  Executor executor;

  void notify_callbacks() {
    auto value = result.value();
    for (auto &callback : completion_callbacks) {
      callback(value);
    }
    completion_callbacks.clear();
  }

};

#endif //CPPCOROUTINES_TASKS_04_TASK_TASKPROMISE_H_
