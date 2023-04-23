/*
 * @Author: Xiazheng <xiazheng.hhu@qq.com>
 * @Date: 2023-04-22 15:17:57
 * @FilePath: test.cpp
 * @Description: 
 */
#include <iostream>
#include <functional>

void foo(int x){
  std::cout << "foo(" << x << ")" << std::endl;
}

void bar(int x){
  std::cout << "bar(" << x << ")" << std::endl;
}

void process(std::function<void(int)> func, int x){
  func(x);
}

int main(){
  std::function<void(int)> myFunc = foo;
  process(myFunc, 123);

  myFunc = bar;
  process(myFunc, 456);

  return 0;
}
