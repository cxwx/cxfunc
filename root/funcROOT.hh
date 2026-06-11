#pragma once

#include <ROOT/RDataFrame.hxx>
#include <ROOT/TSeq.hxx>
#include <TASImage.h>
#include <TArrow.h>
#include <TAxis.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TEllipse.h>
#include <TF1.h>
#include <TFile.h>
#include <TGaxis.h>
#include <TGraph.h>
#include <TGraphAsymmErrors.h>
#include <TGraphErrors.h>
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TLatex.h>
#include <TMatrix.h>
#include <TPaveText.h>
#include <TText.h>
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

template <class T>
void setColorSize(  // TODO(CX): check LinkDef?
  T *x,
  Color_t tc = kRed,
  Float_t tcalpla = 0.5,
  Width_t lw = 1,
  Size_t ps = 0.04
) {
  x->SetLineColorAlpha(tc, tcalpla);
  x->SetFillColorAlpha(tc, tcalpla);
  x->SetMarkerColorAlpha(tc, tcalpla);
  x->SetMarkerSize(ps);
  x->SetLineWidth(lw);
}

void setColorSize(
  TEllipse *x,
  Color_t tc = kRed,
  Float_t tcalpla = 0.5,
  Width_t lw = 1
);

void centerTitle(TH1* h1);

namespace draw {

void upperLimit(TGraphAsymmErrors* g1);

auto preliminary(
  TPad* c1,
  double x = 0.7,
  double y = 0.7,
  double angle = 30,
  int font = 12,
  double size = 0.1,
  const std::string & name = "PRELIMINARY",
  int acolor = kRed
) -> TLatex*;
auto preliminary2(
  TPad* c1,
  double x = 0.7,
  double y = 0.7,
  double x2 = 0.9,
  double y2 = 0.9,
  Float_t angle = 30,
  Font_t font = 12,
  Float_t size = 0.1,
  const std::string & name = "PRELIMINARY",
  Color_t acolor = kRed
) -> TPaveText*;

void skyMap(
  TPad* c1,
  const std::string & name,
  double x0,
  double y,
  double x1 = 0.0,
  double x2 = 360.0,
  bool iftext = true,
  Color_t acolor = kRed,
  Style_t astyle = kCircle,
  Size_t markerSize = 0.6,
  Float_t textsize = 0.03,
  Float_t textangle = 0.0
);

auto skymapCircle(
  double x,
  double y,
  double r,
  int ndelta = 100,
  const std::string & theText = "",
  Color_t color = kRed,
  double forError = 0.0001,
  Width_t lw = 1,
  Style_t ls = kSolid
) -> TGraph *;
}

namespace reverse {
auto reverseXAxis(TH1* h) -> TGaxis*;
auto reDrawYAxis(TH1* h) -> TGaxis*;
void reverseYAxis(TH1* h);
void reverseXTH2D(TH2* h, TAxis* haxis);
void reverseXTH2D_self(TH2* h);
void reverseX(TGraph* g1, TAxis* haxis);
void reverseX(TEllipse* e1, TAxis* haxis);
void reverseX(TArrow* e1, TAxis* haxis);
auto reverseX(double e1, TAxis* haxis) -> double;
}

auto imageToTH2D(TASImage* img, double (*fcn)(double), double xmin, double xmax, double ymin, double ymax) -> TH2D*;
template<typename T1, typename T2>

auto cxTH2toMatrix(T2 *h2, TString op = "C") -> TMatrixT<T1> {
  static_assert(
      (std::is_same_v<T1, double> && std::is_same_v<T2, TH2D>
       || std::is_same_v<T1, float> && std::is_same_v<T2, TH2F>
       || std::is_same_v<T1, int> && std::is_same_v<T2, TH2I>),
      "cxTH2toMatrix type incorrect");
  auto nx = h2->GetNbinsX();
  auto ny = h2->GetNbinsY();
  return TMatrixT<T1>(nx + 2, ny + 2, h2->GetArray(), op);
}

namespace conv {
void TH1Scale(TH1*, double val = 1);
void TH2XScale(TH2*, double val = 1);
void TH2YScale(TH2*, double val = 1);
void TH2XYScale(TH2*, double val = 1);
void THSetAll(TH1*, double val = 0);
void THAddAll(TH1*, double val = 1.0);

auto convertGraph(TGraph* in, void (*)(double& x, double& y)) -> TGraph*;
auto convertGraph(TGraphErrors* in, void (*)(double& x, double& y)) -> TGraphErrors*;
auto convertGraph(TGraphAsymmErrors* in, void (*)(double& x, double& y)) -> TGraphAsymmErrors*;

// harper:ignore
// void Mul(double& x, double& y);
// template <typename T>
// void Mul(T* g) requires(std::is_base_of_v<TGraph, T>) {
//   T* g2 = cxConvertGraph(g, Mul);
//   (*g) = (*g2);
//   delete g2;
// }

void powerIndex(TH1* h1, double index);
void powerIndexlg(TH1* h1, double index);
void powerIndex(::ROOT::RDF::RResultPtr<::TH1D> h1, double index);
void powerIndex(TGraph* g1, double index);
void powerIndex(TGraphErrors* g1, double index);
void powerIndex(TGraphAsymmErrors* g1, double index);
auto powerIndex(TF1* fin, double index) -> TF1*;

void cxDnDE(TH1D* h1);

void aitoff(double& x, double& y);
void aitoff360(double& x, double& y);

template <typename T>
auto aitoff(T* in) -> T* requires(std::is_base_of_v<TGraph, T>) {
  return convertGraph(in, aitoff);
}
template <typename T>
auto aitoff360(T* in) -> T* requires(std::is_base_of_v<TGraph, T>) {
  return convertGraph(in, aitoff360);
}

}

}  // namespace cxfunc::ROOT
