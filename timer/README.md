# Timer 模块 - ROOT Benchmark 工具

## 🎯 简单的 ROOT 性能测试工具

基于 ROOT 的 TBenchmark，提供简单的装饰器模式进行性能测试。

## 🚀 快速开始

### 基础用法

```cpp
#include "benchmark.hh"

using namespace cxfunc::timer;

// 初始化 ROOT benchmark
gBenchmark = new TBenchmark;
gErrorIgnoreLevel = kInfo;

// 使用装饰器测试函数
benchmarkDecorator(myFunction, "Function Name")(args...);

// 输出示例:
// Function Name: Real time 0.12s, CP time 0.11s
```

### 完整示例

```cpp
#include "benchmark.hh"

void testFunction(int n) {
    // 复杂计算
    for (int i = 0; i < n; i++) {
        // ...
    }
}

int main() {
    gBenchmark = new TBenchmark;
    gErrorIgnoreLevel = kInfo;

    benchmarkDecorator(testFunction, "Test 1")(1000);
    benchmarkDecorator(testFunction, "Test 2")(2000);

    return 0;
}
```

## 📖 特性

- ✅ **简单**: 装饰器模式，一行代码完成测试
- ✅ **自动**: 自动计时、格式化输出、重置
- ✅ **ROOT集成**: 完美集成 ROOT 环境
- ✅ **类型安全**: 支持有返回值和无返回值函数

## 🔧 高级用法

### 自定义 Benchmark 对象

```cpp
TBenchmark* myBenchmark = new TBenchmark;
benchmarkDecorator(myFunc, "Test", myBenchmark)(args...);
```

### 条件性测试

```cpp
// 只有当 gErrorIgnoreLevel <= kInfo 时才计时
// gErrorIgnoreLevel = kPrint;  // 不计时
// gErrorIgnoreLevel = kInfo;   // 计时
```

## 📊 输出格式

ROOT TBenchmark 自动输出格式：
```
Function Name: Real time 0.12s, CP time 0.11s, Real time/Cp time = 1.09
```

## 💡 使用建议

1. **ROOT环境**: 如果你已经在使用ROOT，这是最简单的选择
2. **快速测试**: 装饰器模式，插入现有代码很方便
3. **熟悉工具**: 如果你熟悉ROOT，学习成本为零

## 🛠️ 构建

需要 ROOT 环境：

```bash
# 确保 ROOT 已安装
source /path/to/root/bin/thisroot.sh

# 编译
g++ -std=c++17 $(root-config --cflags --libs) test.cpp -o test

# 运行
./test
```