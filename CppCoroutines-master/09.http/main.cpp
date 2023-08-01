//
// Created by benny on 2022/3/17.
//
#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <httplib.h>
#include <nlohmann/json.hpp>

#include "Executor.h"
#include "Task.h"
#include "io_utils.h"

Task<std::string, AsyncExecutor> http_get(std::string host, std::string path)
{
  httplib::Client cli(host);

  auto res = cli.Get(path.c_str());
  debug("http_get");

  int k = 3;
  long long sum = 0;
  while(k>0){
    for(int i = 0;i< 100000;i++){
        for(int j = 0;j< 10000;j++){
          sum += float(i+j);
        }
      }
    debug(sum);
    --k;
  }

  if (res)
  {
    co_return res->body;
  }
  else
  {
    co_return httplib::to_string(res.error());
  }
}

Task<void, LooperExecutor> test_http()
{
  try
  {
    debug("send request...");
    auto result = co_await http_get("https://api.github.com", "/users/bennyhuo");
    debug("done.");
    auto json = nlohmann::json::parse(result);
    debug(json.dump(2));
    debug(json["login"]);
    debug(json["url"]);
    debug(json["bio"]);
  }
  catch (std::exception &e)
  {
    debug(e.what());
  }
}

int main()
{
  debug("main..");
  //test_http().get_result();
  auto rr = test_http();
  debug("finish test_http..");
  float sum;
  while (true)
  {
    for(int i = 0;i< 100000;i++){
      for(int j = 0;j< 10000;j++){
        sum += float(i+j);
      }
    }
    debug(sum);
  }

  return 0;
}
