# `cxconfig`

[![CI](https://github.com/cxwx/cxConfig/actions/workflows/ci.yml/badge.svg)](https://github.com/cxwx/cxConfig/actions/workflows/ci.yml)

C++17 多格式配置文件解析库。统一抽象层，支持 YAML、JSON、TOML、RYML 四种后端。

## 设计原则

Speed is everything.

## 后端对比

| 维度 | yaml-cpp | nlohmann_json | toml++ | rapidyaml |
|------|----------|---------------|--------|-----------|
| 速度 | 最慢 | 快 | 快 | 最快 |
| 易用性 | 最简单 | 简单 | 简单 | 较难 |
| 写入 | 支持(引用语义) | 支持(引用语义) | 需手动 | 需手动 |
| 头文件开销 | 轻 | 重(模板密集) | 重 | 中 |

## 统一接口

四个后端共享相同的访问语法 `operator[]` 和 `operator()`，差异仅在底层值的读写 API：

```cpp
ConfigYAML cfg("config.yaml");
cfg["app"]["name"];        // operator[] 链式访问
cfg("app", "name");        // operator() 链式访问
cfg.Config()["app"]["name"].as<string>();  // 底层库读取 API
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

```bash
brew install yaml-cpp nlohmann-json tomlplusplus rapidyaml
```

> ROOT 为可选依赖，用于生成 C++ 字典（ROOT 宏交互）。不安装 ROOT 仍可正常构建和使用库的核心功能。

## 使用

```cpp
#include "ConfigYAML.hh"
#include "ConfigJson.hh"
#include "ConfigManager.hh"

using namespace cxfunc::config;

// YAML
ConfigYAML yaml("config.yaml");
string name = yaml["app"]["name"].as<string>();
string host = yaml("database", "host").as<string>();  // 链式访问

// JSON
ConfigJson json("config.json");
int timeout = json("database", "pool", "timeout").get<int>();
json["app"]["version"] = 5;  // 写入

// Manager
ConfigManager mgr;
mgr.Register("yaml", &yaml);
mgr.Register("json", &json);
auto* typed = mgr.GetAs<ConfigYAML>("yaml");
```

## 致谢
* qwen3.6:35b -> ollama(local)
* claude-cli
* mimo-v2.5-pro
