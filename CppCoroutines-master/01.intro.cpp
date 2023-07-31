#define __cpp_lib_coroutine

#include <iostream>
#include <coroutine>
#include <future>
#include <chrono>
#include <thread>

using namespace std::chrono_literals;

void Fun() {
  std::cout << 1 << std::endl;
  std::cout << 2 << std::endl;
  std::cout << 3 << std::endl;
  std::cout << 4 << std::endl;
}

struct Result {
  struct promise_type {
    std::suspend_never initial_suspend() {
      return {};
    }

    std::suspend_never final_suspend() noexcept {
      return {};
    }

    Result get_return_object() {
      return {};
    }

    void return_void() {

    }

//    void return_value(int value) {
//
//    }

    void unhandled_exception() {

    }
  };
};

struct Awaiter {
  int value;

  bool await_ready() {
    return false;
  }

  void await_suspend(std::coroutine_handle<> coroutine_handle) {
    std::async([=](){
      std::this_thread::sleep_for(5s);  
      std::cout<<"sus "<<std::this_thread::get_id()<<std::endl;
      //coroutine_handle.resume();
      std::cout<<std::this_thread::get_id()<<" END "<<std::endl;
    });
    coroutine_handle.resume();
    std::cout<<std::this_thread::get_id()<<" raise hand!"<<std::endl;
  }

  int await_resume() {
    std::cout<<"resume "<<std::endl;
    return value;
  }
};

Result Coroutine() {
  std::cout << 1 << std::endl;
  std::cout << co_await Awaiter{.value = 1000} << std::endl;
  std::cout <<std::this_thread::get_id()<<"--->"<< 2 << std::endl;
  std::cout << 3 << std::endl;
  co_await std::suspend_always{};
  std::cout << 4 << std::endl;

  co_return;
};

//Result Coroutine(int start_value) {
//  std::cout << start_value << std::endl;
//  co_await std::suspend_always{};
//  std::cout << start_value + 1 << std::endl;
//
//  co_return 100;
//};
void test_fun(){
  std::async([=](){
      std::this_thread::sleep_for(5s);
      
      std::cout<<"sus "<<std::this_thread::get_id()<<std::endl;
    
    });
  std::cout<<"out of async"<<std::endl;
}

int main() {
  std::cout<<"main "<<std::this_thread::get_id()<<std::endl;
  //test_fun();
  Coroutine();  
  std::cout<<"see me?"<<std::endl; 
  return 0;
}
