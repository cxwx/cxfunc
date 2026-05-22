#ifndef CONFIGBASE_HH__
#define CONFIGBASE_HH__

#include <string>

namespace cxfunc::config {

class ConfigBase {
 protected:
  int theVerbose = 0;
  std::string theFilename;

 public:
  [[nodiscard]] auto Verbose() const -> int { return theVerbose; }
  [[nodiscard]] auto Filename() const -> const std::string& { return theFilename; }
  virtual ~ConfigBase() = default;

  ConfigBase(const ConfigBase&) = delete;
  auto operator=(const ConfigBase&) -> ConfigBase& = delete;
  ConfigBase(ConfigBase&&) = delete;
  auto operator=(ConfigBase&&) -> ConfigBase& = delete;

 protected:
  ConfigBase() = default;
};

}  // namespace cxfunc::config

#endif  // CONFIGBASE_HH__
