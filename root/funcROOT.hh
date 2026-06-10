#pragma once

#include <TFile.h>
#include <string>

namespace cxfunc::ROOT {

void writeLicense(TFile& file, const std::string& license);
void writeLicense(TFile& file);
inline constexpr auto writeLic = [](TFile& f) { writeLicense(f); };

auto openfile(const std::string& fname, const std::string& op = "READ") -> std::unique_ptr<TFile>;

}

