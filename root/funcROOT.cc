#include "funcROOT.hh"
#include <TError.h>

using namespace std;

namespace {
constinit auto DEFAULT_LICENSE = R"(
Copyright (c) 2026 cx, c????x@mail.ustc.edu.cn

MIT License:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.
)";
}

namespace cxfunc::ROOT {

auto openfile(const string &fname, const string &op) -> unique_ptr<TFile> {
  unique_ptr<TFile> f(TFile::Open(fname.c_str(), op.c_str()));
  if (f == nullptr) {
    Error("cxfunc::ROOT::openfile", "Could not open file %s :%s re NULL", fname.c_str(), op.c_str());
    return f;
  }
  if (f->IsZombie()) {
    Error("cxfunc::ROOT::openfile", "Could not open file %s :%s is Zombie", fname.c_str(), op.c_str());
    f->Close();
    return f;
  }
  Info("cxROOT", "open file %s", fname.c_str());
  return f;
}

void writeLicense(TFile& file, const string& license) {
  file.cd();
  TNamed license_obj("License", license.c_str());
  license_obj.Write();
}

void writeLicense(TFile& file) {
  writeLicense(file, DEFAULT_LICENSE);
}

}
