#ifndef CONFIGJSON_T_HH__
#define CONFIGJSON_T_HH__

#include "ConfigBase.hh"
#include <nlohmann/json.hpp>
#include <string>

namespace cxfunc::config {

class ConfigJson : public ConfigBase {
 private:
  nlohmann::json theConfig;

 public:
  explicit ConfigJson(const std::string& filename = std::string(std::getenv("HOME")) + "/rc/cxDefault.json");

  [[nodiscard]] auto Config() const -> const nlohmann::json& { return theConfig; }

  auto operator[](const std::string& key) -> nlohmann::json& { return theConfig[key]; }
  auto operator[](const std::string& key) const -> const nlohmann::json& { return theConfig[key]; }

  template <typename T>
  auto operator()(const nlohmann::json& node, const T& key) const -> const nlohmann::json& {
    return node[key];
  }

  template <typename T, typename... Args>
  auto operator()(const nlohmann::json& node, const T& key, Args... args) const -> const nlohmann::json& {
    return operator()(node[key], args...);
  }

  template <typename... Args>
  auto operator()(Args... args) const -> const nlohmann::json& {
    return operator()(theConfig, args...);
  }
};

}  // namespace cxfunc::config

#endif  // CONFIGJSON_T_HH__
