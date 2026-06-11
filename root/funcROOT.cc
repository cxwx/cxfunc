#include "funcROOT.hh"
#include <TError.h>
#include <TMath.h>
#include <exception>

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

void removeCurrentAxis(TAxis* axis) {
  axis->SetLabelOffset(999);
  axis->SetTickLength(0);
  gPad->Update();  // need check
}
void copyCurrentAxisStyle(TAxis* ax1, TGaxis* ax2) {
  ax2->SetLabelSize(ax1->GetLabelSize());
  ax2->SetLabelFont(ax1->GetLabelFont());
}
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

void setColorSize(TEllipse *x, Color_t tc, Float_t tcalpla, Width_t lw) {
  x->SetLineColorAlpha(tc, tcalpla);
  x->SetFillColorAlpha(tc, tcalpla);
  x->SetLineWidth(lw);
}

void centerTitle(TH1* h1) {
  h1->GetXaxis()->CenterTitle();
  h1->GetYaxis()->CenterTitle();
  h1->GetZaxis()->CenterTitle();
}

namespace draw {
void upperLimit(TGraphAsymmErrors* g1) {
  for (auto i : ::ROOT::TSeqI(g1->GetN())) {
    double x = NAN;
    double y = NAN;
    double yl = NAN;
    double yh = NAN;
    g1->GetPoint(i, x, y);
    yl = g1->GetErrorYlow(i);
    yh = g1->GetErrorYhigh(i);
    if (yl == 0.0 && yh == 0.0) {
      auto* t1 = new TArrow(x, y, x, y / 2.0, 0.01, ">");
      t1->SetBit(kCanDelete);  // TODO(CX): check memory
      t1->SetLineColor(g1->GetLineColor());
      t1->SetFillColor(g1->GetFillColor());
      t1->SetLineWidth(g1->GetLineWidth());
      t1->SetLineStyle(g1->GetLineStyle());
    }
  }
}
auto preliminary(
  TPad* c1,
  double x,
  double y,
  double angle,
  int font,
  double size,
  const std::string & name,
  int acolor
) -> TLatex* {
  c1->cd();
  auto* l1 = new TLatex(x, y, name.c_str());
  l1->SetBit(kCanDelete);
  l1->SetTextFont(font);
  l1->SetTextSize(size);
  l1->SetTextAngle(angle);
  l1->SetTextColor(acolor);
  l1->Draw("same");
  return l1;
}
auto preliminary2(
  TPad* c1,
  double x1,
  double y1,
  double x2,
  double y2,
  Float_t angle,
  Font_t font,
  Float_t size,
  const string & name,
  Color_t acolor
) -> TPaveText* {
  c1->cd();
  auto* l1 = new TPaveText(x1, y1, x2, y2, "NB");
  l1->SetFillColor(kGray);
  l1->AddText(name.c_str());
  l1->SetTextFont(font);
  l1->SetTextSize(size);
  l1->SetTextAngle(angle);
  l1->SetTextColor(acolor);
  l1->SetLineColor(kRed);
  l1->SetLineWidth(3);
  l1->Draw();
  return l1;
}

void skyMap(
  TPad* c1,
  const string & name,
  double x,
  double y,
  double x1,
  double x2,
  bool iftext,
  Color_t acolor,
  Style_t astyle,
  Size_t markerSize,
  Float_t textsize,
  Float_t textangle
) {
  c1->cd();
  while (x > x2) {
    x -= 360.0;
  }
  while (x < x1) {
    x += 360.0;
  }
  auto* g1 = new TGraph(1);
  g1->SetBit(kCanDelete);
  g1->SetPoint(0, x, y);
  g1->SetMarkerSize(markerSize);
  g1->SetMarkerStyle(astyle);
  g1->SetName(name.c_str());
  g1->SetMarkerColor(acolor);
  g1->Draw("Psame");

  if (iftext) {
    auto* t1 = new TText(x, y, name.c_str());
    t1->SetBit(kCanDelete);
    t1->SetTextSize(textsize);
    t1->SetTextColor(acolor);
    t1->SetTextAngle(textangle);
    t1->Draw("same");
  }
}

auto skymapCircle(
  double x,
  double y,
  double r,
  int ndelta,
  const string & theText,
  Color_t color,
  double forError,
  Width_t lw,
  Style_t ls
) ->TGraph * {
  double ra0 = x;
  double dec0 = y;
  double decmin = dec0 - r;
  double delta = 2 * r / ndelta;
  auto * theGraph = new TGraph((2*ndelta) +1);

  for (int i = 0; i <= ndelta; i++) {
    double dec = decmin + (i * delta);
    if (i == ndelta) {
      dec -= forError;  // some bug
    }
    if (i == 0) {
      dec += forError;
    }
    if (dec > 90.0) {
      dec -= forError;
    }
    if (dec < -90.0) {
      dec += forError;
    }
    using TMath::DegToRad;
    using TMath::RadToDeg;
    double w = RadToDeg() * (acos((cos(r * DegToRad()) - sin(dec0 * DegToRad()) * sin(dec * DegToRad())) / (cos(dec0 * DegToRad()) * cos(dec * DegToRad()))));
    double ra1 = ra0 + w;
    double ra2 = ra0 - w;
    theGraph->SetPoint(i, ra1, dec);
    theGraph->SetPoint((2 * ndelta) + 1 - i, ra2, dec);
    if (i == 0) {
      theGraph->SetPoint((2 * ndelta) + 2, ra1, dec);
    }
  }
  theGraph->SetLineColor(color);
  theGraph->SetMarkerColor(color);
  theGraph->SetLineWidth(lw);
  theGraph->SetLineStyle(ls);
  theGraph->SetName(theText.c_str());

  return theGraph;
}
}

namespace reverse {
auto reverseXAxis(TH1 *h) -> TGaxis * {
  TAxis *xaxis = h->GetXaxis();
  removeCurrentAxis(xaxis);
  auto *newaxis = new TGaxis(gPad->GetUxmax(), gPad->GetUymin(), gPad->GetUxmin(), gPad->GetUymin(), xaxis->GetXmin(), xaxis->GetXmax(), 510, "-");
  newaxis->SetLabelOffset(-0.03);
  copyCurrentAxisStyle(xaxis, newaxis);
  newaxis->Draw();
  return newaxis;
}

auto reDrawYAxis(TH1 *h) -> TGaxis * {
  TAxis *yaxis = h->GetYaxis();
  removeCurrentAxis(yaxis);
  auto *newaxis = new TGaxis(gPad->GetUxmin(), gPad->GetUymin(), gPad->GetUxmin(), gPad->GetUymax(), yaxis->GetXmin(), yaxis->GetXmax(), 510, "-");
  // newaxis->SetLabelOffset();
  copyCurrentAxisStyle(yaxis, newaxis);
  newaxis->Draw();
  return newaxis;
}

void reverseYAxis(TH1 *h) {
  TAxis *yaxis = h->GetYaxis();
  removeCurrentAxis(yaxis);
  auto *newaxis = new TGaxis(gPad->GetUxmin(), gPad->GetUymax(), gPad->GetUxmin() - 0.001, gPad->GetUymin(), yaxis->GetXmin(), yaxis->GetXmax(), 510, "+");
  newaxis->SetLabelOffset(-0.03);
  copyCurrentAxisStyle(yaxis, newaxis);
  newaxis->Draw();
}

void reverseXTH2D(TH2 *h, TAxis *haxis) {
  double x1 = h->GetXaxis()->GetXmin();
  double x2 = h->GetXaxis()->GetXmax();
  double x11 = haxis->GetXmin();
  double x21 = haxis->GetXmax();
  // if(x1+x2-x11-x21>1e-11*x1 || x1 == 0.0) //need check
  if (fabs(x1 + x2 - x11 - x21) > 1e-11) {  // need check
    Error("reverseXTH2D", "TH2D in frame errors not middle");
    std::terminate();
  }
  reverseXTH2D_self(h);
}

void reverseXTH2D_self(TH2 *h) {
  for (auto j : ::ROOT::TSeqI(h->GetYaxis()->GetNbins() + 2)) {
    int n = h->GetXaxis()->GetNbins() + 1;
    for (auto i : ::ROOT::TSeqI(n + 1)) {
      int i2 = n - i;
      if (i2 > i) {
        auto VarL = h->GetBinContent(i, j);
        auto VarR = h->GetBinContent(i2, j);
        h->SetBinContent(i, j, VarR);
        h->SetBinContent(i2, j, VarL);
      } else {
        break;
      }
    }
  }
}

void reverseX(TGraph *g1, TAxis *haxis) {
  double xmax = haxis->GetXmax();
  double xmin = haxis->GetXmin();
  double x2 = xmax + xmin;
  for (auto i : ::ROOT::TSeqI(g1->GetN())) {
    double x = NAN;
    double y = NAN;
    g1->GetPoint(i, x, y);
    x = x2 - x;
    g1->SetPoint(i, x, y);
  }
}

void reverseX(TEllipse *e1, TAxis *haxis) {
  Warning("reverse", "Reverse only for circle not ellipse, need modify");
  double xmax = haxis->GetXmax();
  double xmin = haxis->GetXmin();
  double x2 = xmax + xmin;
  e1->SetX1(x2 - e1->GetX1());
}

void reverseX(TArrow *e1, TAxis *haxis) {
  double xmax = haxis->GetXmax();
  double xmin = haxis->GetXmin();
  double xm = xmax + xmin;
  e1->SetX1(xm - e1->GetX1());
  e1->SetX2(xm - e1->GetX2());
}

double reverseX(double e1, TAxis *haxis) {
  double xmax = haxis->GetXmax();
  double xmin = haxis->GetXmin();
  double x2 = xmax + xmin;
  return x2 - e1;
}
} // namespace reverse
}
