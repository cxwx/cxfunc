#include "ConfigJson.hh"
#include <fstream>
#include <iostream>

using namespace std;

namespace cxfunc::config {

ConfigJson::ConfigJson(const string& filename) {
  theFilename = filename;
  ifstream file(filename);
  if (!file) {
    throw runtime_error("Failed to open file: " + filename);
  }
  // file >> theConfig;  // no comments
  theConfig = nlohmann::json::parse(file, nullptr, true, true);


  if (theConfig["global"]["verbose"].is_number_integer()) {
    theVerbose = theConfig["global"]["verbose"].get<int>();
  } else if (theConfig["verbose"].is_number_integer()) {
    theVerbose = theConfig["verbose"].get<int>();
  }

  if (theVerbose > 0) {
    cout << theConfig.dump(2) << '\n';
  }
}

}  // namespace cxfunc::config
