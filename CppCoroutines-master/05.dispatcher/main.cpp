//
// Created by benny on 2022/3/17.
//
#include "Executor.h"
#include "Task.h"
#include "io_utils.h"
#include "Scheduler.h"

void test_scheduler() {
  auto scheduler = Scheduler();

  debug("start")
  scheduler.execute([]() { debug("1"); }, 50);
  scheduler.execute([]() { debug("2"); }, 100);
  scheduler.execute([]() { debug("3"); }, 200);
  scheduler.execute([]() { debug("4"); }, 300);
  scheduler.execute([]() { debug("5"); }, 500);
  scheduler.execute([]() { debug("6"); }, 1000);

  scheduler.shutdown();
}

Task<int, AsyncExecutor> simple_task2() {
  debug("task 2 start ...");
  using namespace std::chrono_literals;
  std::this_thread::sleep_for(5s);
  debug("task 2 returns after 1s.");
  co_return 2;
}

Task<int, NewThreadExecutor> simple_task3() {
  debug("in task 3 start ...");
  using namespace std::chrono_literals;
  std::this_thread::sleep_for(5s);
  debug("task 3 returns after 5s.");
  co_return 3;
}

Task<int, LooperExecutor> simple_task() {
  debug("task start ...");
  auto result2 = co_await simple_task2();
  debug("returns from task2: ", result2);
  auto result3 = co_await simple_task3();
  debug("returns from task3: ", result3);
  co_return 1 + result2 + result3;
}

void test_tasks() {
  auto simpleTask = simple_task();
  debug("finish the simple_task");
  simpleTask.then([](int i) {
    debug("simple task end: ", i);
  }).catching([](std::exception &e) {
    debug("error occurred", e.what());
  });
  /*
  try {
    auto i = simpleTask.get_result();
    debug("simple task end from get: ", i);
  } catch (std::exception &e) {
    debug("error: ", e.what());
  }*/
  using namespace std::chrono_literals;
  for(int i = 0;i<10;i++){
    std::this_thread::sleep_for(1s);
    debug("I am still alive");
  }
}

int main() {
  debug("Main...");
  test_tasks();

  auto looper = LooperExecutor();

  using namespace std::chrono_literals;
  std::this_thread::sleep_for(1s);
  looper.shutdown(false);
  std::this_thread::sleep_for(1s);

  return 0;
}
