# Error Handler Module

纯 C++17 错误处理装饰器，无任何外部依赖。

## 功能

类似 `timer` 模块，这个模块提供了错误处理装饰器，用于在函数执行出错时打印函数信息。

## 使用方法

```cpp
#include "error_exit.hh"
using namespace cxfunc::error;

// 基本使用
auto wrappedFunc = errorExitDecorator(originalFunction, "functionName", true);
wrappedFunc(args...);

// 简化版本
auto simpleWrapped = simpleErrorExit(originalFunction, "functionName");
simpleWrapped(args...);
```

## 特性

- 在函数进入时打印 `[ENTER]` 消息
- 在函数成功完成时打印 `[SUCCESS]` 消息  
- 在函数抛出异常时打印 `[ERROR]` 消息，包括：
  - 错误类型
  - 错误消息
  - 可选的详细跟踪信息

## 依赖

- C++17 或更高版本
- 无其他外部依赖

## 示例

编译并运行示例程序：

```bash
cd error
mkdir build && cd build
cmake ..
make
./cxErrorTest
```

参见 `cxErrorTest.cpp` 文件查看完整的使用示例。
