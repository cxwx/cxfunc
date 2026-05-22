#ifndef CONFIGMANAGER_HH__
#define CONFIGMANAGER_HH__

#include "ConfigBase.hh"
#include <string>
#include <unordered_map>

namespace cxfunc::config {

class ConfigManager {
 private:
  std::unordered_map<std::string, ConfigBase*> theConfigs;

 public:
  void Register(const std::string& name, ConfigBase* cfg) {
    theConfigs[name] = cfg;
  }

  [[nodiscard]] ConfigBase* Get(const std::string& name) const {
    auto it = theConfigs.find(name);
    if (it == theConfigs.end()) {
      return nullptr;
    }
    return it->second;
  }

  template <typename T>
  [[nodiscard]] auto GetAs(const std::string& name) const -> T* {
    return dynamic_cast<T*>(Get(name));
  }

  [[nodiscard]] auto Size() const -> size_t { return theConfigs.size(); }
  [[nodiscard]] auto Has(const std::string& name) const -> bool {
    return theConfigs.contains(name);
  }
};

}  // namespace cxfunc::config

#endif  // CONFIGMANAGER_HH__
