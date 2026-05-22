#ifndef CONFIGYAML_T_HH__
#define CONFIGYAML_T_HH__

#include <yaml-cpp/yaml.h>
#include <string>
#include "ConfigBase.hh"

namespace cxfunc::config {

class ConfigYAML : public ConfigBase {
 private:
  YAML::Node theConfig;

 public:
  [[nodiscard]] const YAML::Node& Config() const { return theConfig; }

  template <typename T>
  YAML::Node operator[](const T& key) const {
    return theConfig[key];
  }

  template <typename T>
  YAML::Node operator()(const YAML::Node& node, const T& key) const {
    return node[key];
  }

  template <typename T, typename... Args>
  YAML::Node operator()(const YAML::Node& node, const T& key, Args... args) const {
    return operator()(node[key], args...);
  }

  template <typename... Args>
  YAML::Node operator()(Args... args) const {
    return operator()(theConfig, args...);
  }

  explicit ConfigYAML(const std::string& filename = std::string(std::getenv("HOME")) + "/rc/cxDefault.yml");
};

}  // namespace cxfunc::config

#endif  // CONFIGYAML_T_HH__
