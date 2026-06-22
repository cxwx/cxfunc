# Benchmark Comparison

## benchmark.hh vs benchmark2.hh

### benchmark.hh (ROOT-based)
```cpp
#include "benchmark.hh"
using cxfunc::timer::benchmarkDecorator;

auto timedFunc = benchmarkDecorator(myFunction, "myFunction");
```

**特点：**
- 依赖 ROOT 框架
- 使用 `ROOT::TBenchmark` 类
- 适合 ROOT 项目
- 需要更多样板代码

### benchmark2.hh (Pure C++) - 推荐用于纯C++项目
```cpp
#include "benchmark2.hh"

auto timedFunc = cxfunc::timer::benchmark2(myFunction, "myFunction");
```

**特点：**
- 纯 C++ 实现，无外部依赖
- 直接在装饰器中计时，无额外类
- API 更简洁：`benchmark2(function, name)`
- 高精度计时：`std::chrono::high_resolution_clock`
- 完全可移植，适合任何 C++17+ 项目

## 使用建议

- **ROOT 项目**: 使用 `benchmark.hh`
- **纯 C++ 项目**: 使用 `benchmark2.hh` (推荐)
- **跨平台项目**: 使用 `benchmark2.hh` (无 ROOT 依赖)

## API 对比

### 原始版本（过于复杂）
```cpp
class SimpleBenchmark {
    void Start(const std::string& name);
    double Stop(const std::string& name);
    void Show(const std::string& name);
    void Reset();
};

auto decorated = benchmarkDecorator2(func, "name", &gBenchmark2);
```

### 简化版本（推荐）
```cpp
auto decorated = benchmark2(func, "name");
```

简化版本移除了不必要的类设计，直接在函数装饰器中完成所有计时逻辑，更加简洁直观。