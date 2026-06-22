#include "error_exit.hh"
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

namespace {

void testNormalFunction(const string &message) { cout << "  Processing message: " << message << '\n'; }

void testThrowingFunction(int value) {
  if (value < 0) { throw std::runtime_error("Value cannot be negative!"); }
  cout << "  Processing value: " << value << '\n';
}

auto testCalculateFunction(int a, int b) -> int {
  if (b == 0) { throw std::runtime_error("Division by zero!"); }
  return a / b;
}


void cxErrorTest() {
  using namespace cxfunc::error;

  cout << "=== 测试1: 正常函数执行 ===" << '\n';
  try {
    auto wrappedNormal = errorExitDecorator(testNormalFunction, "testNormalFunction", true);
    wrappedNormal("Hello from error handler!");
  } catch (const exception &e) { cerr << "Caught exception: " << e.what() << '\n'; }

  cout << "\n=== 测试2: 异常函数执行 ===" << '\n';
  try {
    auto wrappedThrowing = errorExitDecorator(testThrowingFunction, "testThrowingFunction", true);
    wrappedThrowing(-5);  // 这会抛出异常
  } catch (const exception &e) { cerr << "Caught exception: " << e.what() << '\n'; }

  cout << "\n=== 测试3: 带返回值的正常函数 ===" << '\n';
  try {
    auto wrappedCalculate = errorExitDecorator(testCalculateFunction, "testCalculateFunction", false);
    int result = wrappedCalculate(10, 2);
    cout << "Result: " << result << '\n';
  } catch (const exception &e) { cerr << "Caught exception: " << e.what() << '\n'; }

  cout << "\n=== 测试4: 带返回值的异常函数 ===" << '\n';
  try {
    auto wrappedCalculate = errorExitDecorator(testCalculateFunction, "testCalculateFunction", true);
    int result = wrappedCalculate(10, 0);  // 这会抛出异常
    cout << "Result: " << result << '\n';
  } catch (const exception &e) { cerr << "Caught exception: " << e.what() << '\n'; }

  cout << "\n=== 测试5: 简化版装饰器 ===" << '\n';
  try {
    auto wrappedSimple = simpleErrorExit(testThrowingFunction, "simpleTest");
    wrappedSimple(10);  // 正常执行
  } catch (const exception &e) { cerr << "Caught exception: " << e.what() << '\n'; }

  cout << "\n=== 测试6: 进入/退出装饰器（无 try/catch）正常情况 ===" << '\n';
  {
    auto wrappedEntryExit = entryExitDecorator(testNormalFunction, "testNormalFunction");
    wrappedEntryExit("entry/exit normal");
  }

  cout << "\n=== 测试7: 进入/退出装饰器 - 异常情况（装饰器本身无 try/catch） ===" << '\n';
  // 装饰器不含 try/catch：异常会传播出来，但 ExitLog 析构时仍会打印 [EXIT]
  // 这里的 try/catch 只为让测试继续跑完，不是装饰器的一部分
  try {
    auto wrappedEntryExit = entryExitDecorator(testThrowingFunction, "testThrowingFunction");
    wrappedEntryExit(-1);  // 抛异常
  } catch (const exception &e) { cerr << "Caught exception: " << e.what() << '\n'; }

  cout << "\n=== 所有测试完成 ===" << '\n';
}
}  // namespace

auto main() -> int {
  cxErrorTest();
  return 0;
}
