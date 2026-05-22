#include "ConfigRapidYml.hh"
#include <fstream>
#include <iostream>

using namespace std;

namespace cxfunc::config {

ConfigRapidYml::ConfigRapidYml(const string& filename) {
  theFilename = filename;
  ifstream file(filename, ios::ate | ios::binary);
  if (!file) {
    throw runtime_error("Failed to open file: " + filename);
  }
  auto size = static_cast<streamsize>(file.tellg());
  buffer.resize(size + 1, '\0');
  file.seekg(0);
  file.read(buffer.data(), size);
  theTree = ryml::parse_in_place(ryml::to_substr(buffer.data()));
  theConfig = theTree.rootref();

  if (theConfig.has_child("global")) {
    ryml::ConstNodeRef global = theConfig["global"];
    if (global.has_child("verbose")) {
      global["verbose"] >> theVerbose;
    }
  } else if (theConfig.has_child("verbose")) {
    theConfig["verbose"] >> theVerbose;
  }

  if (theVerbose > 0) {
    ryml::emit_yaml(theTree);
  }
}

}  // namespace cxfunc::config
