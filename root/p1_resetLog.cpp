#include "funcROOT.hh"


void p1_resetLog() {
  TH1D * h1 = new TH1D("h1", "h1", 100, 0, 100);
  cxfunc::ROOT::resetLogBin(h1, "x");
}
