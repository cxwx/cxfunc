#ifndef CONFIGTOMLPP_T_HH__
#define CONFIGTOMLPP_T_HH__

#include <string>
#include <toml++/toml.h>
#include "ConfigBase.hh"

namespace cxfunc::config {

class ConfigTomlpp : public ConfigBase {
 private:
  toml::table theTree;

 public:
  [[nodiscard]] auto Tree() const -> const toml::table& { return theTree; }

  auto operator[](const std::string& key) const -> toml::node_view<const toml::node> { return theTree[key]; }

  template <typename T>
  auto operator()(const toml::node_view<const toml::node>& node, const T& key) const -> toml::node_view<const toml::node> {
    return node[key];
  }

  template <typename T, typename... Args>
  auto operator()(const toml::node_view<const toml::node>& node, const T& key, Args... args) const -> toml::node_view<const toml::node> {
    return operator()(node[key], args...);
  }

  template <typename... Args>
  auto operator()(Args... args) const -> toml::node_view<const toml::node> {
    return operator()(toml::node_view<const toml::node>(theTree), args...);
  }

  explicit ConfigTomlpp(const std::string& filename = std::string(std::getenv("HOME")) + "/rc/cxDefault.toml");
};

}  // namespace cxfunc::config

#endif  // CONFIGTOMLPP_T_HH__
