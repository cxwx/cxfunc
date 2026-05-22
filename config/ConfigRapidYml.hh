#ifndef CONFIGRAPIDYML_T_HH__
#define CONFIGRAPIDYML_T_HH__

#include <ryml.hpp>
#include <string>
#include "ConfigBase.hh"

namespace cxfunc::config {

class ConfigRapidYml : public ConfigBase {
 private:
  ryml::Tree theTree;
  ryml::ConstNodeRef theConfig;
  std::string buffer;

 public:
  [[nodiscard]] auto Config() const -> ryml::ConstNodeRef { return theConfig; }
  [[nodiscard]] auto Tree() const -> const ryml::Tree& { return theTree; }

  template <typename T>
  auto operator[](const T& key) const -> ryml::ConstNodeRef {
    return theConfig[key];
  }

  template <typename T>
  auto operator()(const ryml::ConstNodeRef& node, const T& key) const -> ryml::ConstNodeRef {
    return node[key];
  }

  template <typename T, typename... Args>
  auto operator()(const ryml::ConstNodeRef& node, const T& key, Args... args) const -> ryml::ConstNodeRef {
    return operator()(node[key], args...);
  }

  template <typename... Args>
  auto operator()(Args... args) const -> ryml::ConstNodeRef {
    return operator()(theConfig, args...);
  }

  explicit ConfigRapidYml(const std::string& filename = std::string(std::getenv("HOME")) + "/rc/cxDefault.yml");
};

}  // namespace cxfunc::config

#endif  // CONFIGRAPIDYML_T_HH__
