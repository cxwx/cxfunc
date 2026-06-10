#include "funcROOT.hh"


void p1_resetLog() {
  TH1D * h1 = new TH1D("h1", "h1", 10, 1, 10);
  cxfunc::ROOT::resetLogBin(h1);
}
