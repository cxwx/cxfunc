#include "ConfigYAML.hh"
#include <fstream>
#include <iostream>

using namespace std;

namespace cxfunc::config {

ConfigYAML::ConfigYAML(const string& filename) {
  theFilename = filename;
  ifstream file(filename);
  if (!file) {
    throw runtime_error("Failed to open file: " + filename);
  }
  theConfig = YAML::LoadFile(filename);

  if (theConfig["global"] && theConfig["global"]["verbose"].IsScalar()) {
    theVerbose = theConfig["global"]["verbose"].as<int>();
  } else if (theConfig["verbose"] && theConfig["verbose"].IsScalar()) {
    theVerbose = theConfig["verbose"].as<int>();
  }

  if (theVerbose > 0) {
    cout << theConfig;
  }
}

}  // namespace cxfunc::config
