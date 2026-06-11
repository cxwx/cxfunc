#ifndef READWRITE_CX___
#define READWRITE_CX___

#include <cstdarg>
#include <cstdlib>
#include <fstream>
#include <string>

namespace cxfunc::read {

void errorexit(int i, ...);

auto striget(const std::string &s, int n) -> int;
auto strfget(const std::string &s, int n) -> double;
auto strsget(const std::string &s, int n) -> std::string;
auto striget(const std::string &in, int n, std::string &out) -> bool;
auto strfget(const std::string &in, int n, std::string &out) -> bool;
auto strsget(const std::string &in, int n, std::string &out) -> bool;

auto getline(std::ifstream &fin, std::string &s) -> bool;
}

#endif
