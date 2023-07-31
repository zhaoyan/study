//
// Created by benny on 2022/3/17.
//

#ifndef CPPCOROUTINES_04_TASK_TASKAWAITER_H_
#define CPPCOROUTINES_04_TASK_TASKAWAITER_H_

#include "coroutine_common.h"

#include "io_utils.h"

template<typename ResultType>
struct Task;

template<typename Result>
struct TaskAwaiter {
  explicit TaskAwaiter(Task<Result> &&task) noexcept
      : task(std::move(task)) {}

  TaskAwaiter(TaskAwaiter &&completion) noexcept
      : task(std::exchange(completion.task, {})) {}

  TaskAwaiter(TaskAwaiter &) = delete;

  TaskAwaiter &operator=(TaskAwaiter &) = delete;

  constexpr bool await_ready() const noexcept {
    return false;
  }

  void await_suspend(std::coroutine_handle<> handle) noexcept {
    debug("call suspend")
    std::cout<<handle.address()<<std::endl;
    task.finally([handle]() {
      handle.resume();
    });
    debug("call suspend finished here")
  }

  Result await_resume() noexcept {
    debug("call await_result")
    return task.get_result();
  }

 private:
  Task<Result> task;
};

#endif //CPPCOROUTINES_04_TASK_TASKAWAITER_H_
