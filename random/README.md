# `cxrandom`

[![CI](https://github.com/cxwx/cxRandom/actions/workflows/ci.yml/badge.svg)](https://github.com/cxwx/cxRandom/actions/workflows/ci.yml)

C++20 随机种子生成库。提供字符串配置方式选择种子来源，支持时间、系统随机设备、操作系统熵源和固定数值。

## 种子来源

| 字符串值          | 来源                     | 确定性 |
|:-----------------:|------------------------|:------:|
| `"TIME"`          | `time(nullptr)`        | 可复现 |
| `"random_device"` | `std::random_device`   | 不可复现 |
| `"urandom"`       | `/dev/urandom`         | 不可复现 |
| `"42"` 等数值     | `std::stoi`            | 可复现 |

## 使用

```cpp
#include "cxRandomSeed3.hh"

using namespace cxfunc::rand;

// 从 /dev/urandom 读取
unsigned seed = strToSeed("urandom");
std::mt19937 rng(seed);

// 固定种子
seed = strToSeed("12345");

// 时间种子（会输出 warn 日志）
seed = strToSeed("TIME");
```

## 构建

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.ncpu)
```

## 安装

```bash
cmake --install .
# 默认安装到 $HOME/software/install
```

## 依赖

> ROOT 为可选依赖，用于生成 C++ 字典（ROOT 宏交互）。不安装 ROOT 仍可正常构建和使用库的核心功能。

## 版本历史

- `cxRandomSeed2.hh` — cxfunc::rand 命名空间，新增 urandom
- `cxRandomSeed3.hh` — 当前版本，合并两个版本，移除 spdlog 依赖

## 设计原则

- **统一接口**：所有种子来源通过同一个 `strToSeed()` 函数接入
- **零依赖**：无第三方依赖即可编译，仅用 C++ 标准库
- **安全第一**：`urandom` 使用 `std::bit_cast` 进行位转换，避免未定义行为
