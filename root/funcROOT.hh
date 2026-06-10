#pragma once

#include <ROOT/TSeq.hxx>
#include <TAxis.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <cmath>
#include <ranges>
// #include <ROOT/TSeq.hxx>

namespace cxfunc::ROOT {

void writeLicense(TFile &file, const std::string &license);
void writeLicense(TFile &file);
inline constexpr auto writeLic = [](TFile &f) -> void { writeLicense(f); };

auto openfile(const std::string &fname, const std::string &op = "READ") -> std::unique_ptr<TFile>;

// TODO(CX): test
template <typename T>
void resetLogBin(T *h, const TString &logaxis = "xyz")
  requires(std::is_base_of_v<TH1, T>)
{
  auto setaxis = [](std::vector<double> &x, double min, double max, int n, bool ifLog) -> void {
    if (ifLog) {
      max = log10(max);
      min = log10(min);
      double w = (max - min) / static_cast<double>(n);
      for (auto i : std::views::iota(0, n + 1)) { x[i] = pow(10.0, min + (i * w)); }
    } else {
      double w = (max - min) / static_cast<double>(n);
      for (auto i : std::views::iota(0, n + 1)) { x[i] = min + (i * w); }
    }
  };

  TAxis *xaxis = h->GetXaxis();
  TAxis *yaxis = h->GetYaxis();
  TAxis *zaxis = h->GetZaxis();

  int nx = xaxis->GetNbins();
  double minx = xaxis->GetBinLowEdge(1);
  double maxx = xaxis->GetBinUpEdge(nx);
  int ny = yaxis->GetNbins();
  double miny = yaxis->GetBinLowEdge(1);
  double maxy = yaxis->GetBinUpEdge(ny);
  int nz = zaxis->GetNbins();
  double minz = zaxis->GetBinLowEdge(1);
  double maxz = zaxis->GetBinUpEdge(nz);

  std::vector<double> xl(nx + 1);
  std::vector<double> yl(ny + 1);
  std::vector<double> zl(nz + 1);

  if (logaxis.Contains("x")) {
    setaxis(xl, minx, maxx, nx, true);
  } else {
    setaxis(xl, minx, maxx, nx, false);
  }
  if (logaxis.Contains("y")) {
    setaxis(yl, miny, maxy, ny, true);
  } else {
    setaxis(yl, miny, maxy, ny, false);
  }
  if (logaxis.Contains("z")) {
    setaxis(zl, minz, maxz, nz, true);
  } else {
    setaxis(zl, minz, maxz, nz, false);
  }
  if (std::is_base_of_v<TH3, T>) {
    h->SetBins(nx, xl.data(), ny, yl.data(), nz, zl.data());
  } else if (std::is_base_of_v<TH2, T>) {
    h->SetBins(nx, xl.data(), ny, yl.data());
  } else if (std::is_base_of_v<TH1, T>) {
    h->SetBins(nx, xl.data());
  }
}
}  // namespace cxfunc::ROOT
