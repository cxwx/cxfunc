#ifdef __CLING__

#pragma link C++ function cxfunc::ROOT::writeLicense(TFile&, const std::string&);
#pragma link C++ function cxfunc::ROOT::writeLicense(TFile&);

#pragma link C++ function cxfunc::ROOT::openfile;

#pragma link C++ function cxfunc::ROOT::setColorSize(TEllipse*, Color_t, Float_t, Width_t);
#pragma link C++ function cxfunc::ROOT::setColorSize;
#pragma link C++ function cxfunc::ROOT::centerTitle;
#pragma link C++ function cxfunc::ROOT::draw::upperLimit;
#pragma link C++ function cxfunc::ROOT::draw::preliminary;
#pragma link C++ function cxfunc::ROOT::draw::preliminary2;
#pragma link C++ function cxfunc::ROOT::draw::skyMap;
#pragma link C++ function cxfunc::ROOT::draw::skymapCircle;

// reverse namespace functions
#pragma link C++ function cxfunc::ROOT::reverse::reverseXAxis;
#pragma link C++ function cxfunc::ROOT::reverse::reDrawYAxis;
#pragma link C++ function cxfunc::ROOT::reverse::reverseYAxis;
#pragma link C++ function cxfunc::ROOT::reverse::reverseXTH2D;
#pragma link C++ function cxfunc::ROOT::reverse::reverseXTH2D_self;
#pragma link C++ function cxfunc::ROOT::reverse::reverseX(TGraph*, TAxis*);
#pragma link C++ function cxfunc::ROOT::reverse::reverseX(TEllipse*, TAxis*);
#pragma link C++ function cxfunc::ROOT::reverse::reverseX(TArrow*, TAxis*);
#pragma link C++ function cxfunc::ROOT::reverse::reverseX(double, TAxis*);

#pragma link C++ function cxfunc::ROOT::imageToTH2D;

// conv namespace functions
#pragma link C++ function cxfunc::ROOT::conv::TH1Scale;
#pragma link C++ function cxfunc::ROOT::conv::TH2XScale;
#pragma link C++ function cxfunc::ROOT::conv::TH2YScale;
#pragma link C++ function cxfunc::ROOT::conv::TH2XYScale;
#pragma link C++ function cxfunc::ROOT::conv::THSetAll;
#pragma link C++ function cxfunc::ROOT::conv::THAddAll;
#pragma link C++ function cxfunc::ROOT::conv::powerIndex(TH1*, double);
#pragma link C++ function cxfunc::ROOT::conv::powerIndexlg(TH1*, double);
#pragma link C++ function cxfunc::ROOT::conv::powerIndex(::ROOT::RDF::RResultPtr<::TH1D>, double);
#pragma link C++ function cxfunc::ROOT::conv::powerIndex(TGraph*, double);
#pragma link C++ function cxfunc::ROOT::conv::powerIndex(TGraphErrors*, double);
#pragma link C++ function cxfunc::ROOT::conv::powerIndex(TGraphAsymmErrors*, double);
#pragma link C++ function cxfunc::ROOT::conv::powerIndex(TF1*, double);
#pragma link C++ function cxfunc::ROOT::conv::cxDnDE;
#pragma link C++ function cxfunc::ROOT::conv::aitoff;
#pragma link C++ function cxfunc::ROOT::conv::aitoff360;

#endif
