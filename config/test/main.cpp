#include <iostream>
#include <string>

#include "ConfigYAML.hh"
#include "ConfigJson.hh"
#include "ConfigTomlpp.hh"
#include "ConfigRapidYml.hh"
#include "ConfigManager.hh"

using namespace std;
using namespace cxfunc::config;

// ==================== helper ====================

static void check(bool cond, const string& msg) {
  if (!cond) {
    cerr << "FAIL: " << msg << endl;
    exit(1);
  }
  cout << "  PASS: " << msg << endl;
}

#ifndef CXCONFIG_TEST_DIR
#define CXCONFIG_TEST_DIR "."
#endif

static const string TEST_DIR = CXCONFIG_TEST_DIR;
static const string YAML_PATH = TEST_DIR + "/config.yaml";
static const string JSON_PATH = TEST_DIR + "/config.json";
static const string TOML_PATH = TEST_DIR + "/config.toml";

// ==================== ConfigYAML tests ====================

static void test_yaml() {
  ConfigYAML cfg(YAML_PATH);

  // base class
  check(cfg.Verbose() == 2, "yaml Verbose == 2");
  check(cfg.Filename() == YAML_PATH, "yaml Filename()");

  // Config() direct access
  check(cfg.Config()["app"]["name"].as<string>() == "test_app", "yaml Config() app.name");
  check(cfg.Config()["app"]["version"].as<int>() == 3, "yaml Config() app.version");
  check(cfg.Config()["database"]["host"].as<string>() == "localhost", "yaml Config() db host");
  check(cfg.Config()["database"]["pool"]["size"].as<int>() == 10, "yaml Config() db pool size");

  // operator[]
  check(cfg["app"]["name"].as<string>() == "test_app", "yaml [] app.name");
  check(cfg["database"]["pool"]["timeout"].as<int>() == 30, "yaml [] db.pool.timeout");

  // operator()
  check(cfg("app", "name").as<string>() == "test_app", "yaml () app.name");
  check(cfg("database", "pool", "timeout").as<int>() == 30, "yaml () db.pool.timeout");

  cout << "  ConfigYAML all tests passed" << endl;
}

// ==================== ConfigJson tests ====================

static void test_json() {
  ConfigJson cfg(JSON_PATH);

  // base class
  check(cfg.Verbose() == 3, "json Verbose == 3");
  check(cfg.Filename() == JSON_PATH, "json Filename()");

  // Config() direct access
  check(cfg.Config()["app"]["name"].get<string>() == "json_test", "json Config() app.name");
  check(cfg.Config()["app"]["version"].get<int>() == 4, "json Config() app.version");
  check(cfg.Config()["database"]["host"].get<string>() == "remotehost", "json Config() db host");
  check(cfg.Config()["database"]["pool"]["size"].get<int>() == 20, "json Config() db pool size");

  // operator[]
  check(cfg["app"]["name"].get<string>() == "json_test", "json [] app.name");
  check(cfg["app"]["debug"].get<bool>() == false, "json [] debug");

  // operator()
  check(cfg("app", "name").get<string>() == "json_test", "json () app.name");
  check(cfg("database", "pool", "timeout").get<int>() == 60, "json () db.pool.timeout");

  // write back
  cfg["app"]["version"] = 5;
  check(cfg["app"]["version"].get<int>() == 5, "json write back");

  cout << "  ConfigJson all tests passed" << endl;
}

// ==================== ConfigRapidYml tests ====================

static void test_ryml() {
  ConfigRapidYml cfg(YAML_PATH);

  // base class
  check(cfg.Verbose() == 2, "ryml Verbose == 2");
  check(cfg.Filename() == YAML_PATH, "ryml Filename()");

  // Config() direct access
  check(cfg.Config()["app"]["name"].val() == "test_app", "ryml Config() app.name");
  check(cfg.Config()["database"]["host"].val() == "localhost", "ryml Config() db host");

  // operator[]
  check(cfg["app"]["name"].val() == "test_app", "ryml [] app.name");
  check(cfg["database"]["pool"]["size"].val() == "10", "ryml [] db.pool.size");

  // operator()
  check(cfg("app", "name").val() == "test_app", "ryml () app.name");
  check(cfg("database", "pool", "timeout").val() == "30", "ryml () db.pool.timeout");

  cout << "  ConfigRapidYml all tests passed" << endl;
}

// ==================== ConfigTomlpp tests ====================

static void test_toml() {
  ConfigTomlpp cfg(TOML_PATH);

  // base class
  check(cfg.Verbose() == 2, "toml Verbose == 2");
  check(cfg.Filename() == TOML_PATH, "toml Filename()");

  // Tree() direct access
  check(cfg.Tree()["app"]["name"].value_or("") == std::string("toml_test"), "toml Tree() app.name");
  check(cfg.Tree()["app"]["version"].value_or(0) == 2, "toml Tree() app.version");
  check(cfg.Tree()["database"]["host"].value_or("") == std::string("toml-host"), "toml Tree() db host");
  check(cfg.Tree()["database"]["pool"]["size"].value_or(0) == 5, "toml Tree() db pool size");

  // operator[]
  check(cfg["app"]["name"].value_or("") == std::string("toml_test"), "toml [] app.name");
  check(cfg["database"]["pool"]["timeout"].value_or(0) == 15, "toml [] db.pool.timeout");
  check(cfg["database"]["pool"]["tags"]["main"].value_or(false) == true, "toml [] db.pool.tags.main");

  // operator()
  check(cfg("app", "name").value_or("") == std::string("toml_test"), "toml () app.name");
  check(cfg("database", "pool", "size").value_or(0) == 5, "toml () db.pool.size");
  check(cfg("database", "pool", "timeout").value_or(0) == 15, "toml () db.pool.timeout");

  // type conversions
  check(cfg["count"].value_or(0) == 42, "toml int");
  check(cfg["pi"].value_or(0.0) > 3.14, "toml float");
  check(cfg["enabled"].value_or(false) == true, "toml bool");
  check(cfg["mode"].value_or("") == std::string("test"), "toml string");
  check(cfg["empty_str"].value_or("default") == std::string(""), "toml empty string");
  check(cfg["nonexistent"].value_or(-1) == -1, "toml missing key");

  // array
  auto seed = cfg["seed"];
  check(seed.is_array(), "toml seed is array");
  check(seed.as_array()->size() == 3, "toml seed array size");

  cout << "  ConfigTomlpp all tests passed" << endl;
}

// ==================== ConfigManager tests ====================

static void test_manager() {
  ConfigManager mgr;
  ConfigYAML yamlCfg(YAML_PATH);
  ConfigJson jsonCfg(JSON_PATH);

  mgr.Register("yaml", &yamlCfg);
  mgr.Register("json", &jsonCfg);

  check(mgr.Size() == 2, "manager has 2 entries");
  check(mgr.Has("yaml"), "manager has yaml");
  check(mgr.Has("json"), "manager has json");
  check(!mgr.Has("toml"), "manager has no toml");

  auto* base = mgr.Get("yaml");
  check(base != nullptr, "manager Get yaml");
  check(base->Verbose() == 2, "manager yaml Verbose");

  auto* typed = mgr.GetAs<ConfigYAML>("yaml");
  check(typed != nullptr, "manager GetAs yaml");
  check(typed->Config()["app"]["name"].as<string>() == "test_app", "manager yaml access");

  check(mgr.Get("nonexistent") == nullptr, "manager returns nullptr for missing");

  cout << "  ConfigManager all tests passed" << endl;
}

// ==================== main ====================

int main() {
  cout << "=== cxconfig unit tests ===" << endl;

  cout << "--- YAML ---" << endl;
  test_yaml();

  cout << "--- JSON ---" << endl;
  test_json();

  cout << "--- TOML ---" << endl;
  test_toml();

  cout << "--- RYML ---" << endl;
  test_ryml();

  cout << "--- Manager ---" << endl;
  test_manager();

  cout << endl << "ALL TESTS PASSED" << endl;
  return 0;
}
