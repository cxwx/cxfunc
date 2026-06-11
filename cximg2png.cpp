// cspell:disable
#include <TImage.h>
#include <string>
using namespace std;

namespace {
void eps2png(const string& filename) {
  TImage* feps = TImage::Open(filename.c_str());
  feps->WriteImage(("cxpng/" + filename + ".png").c_str());
}
}  // namespace

int main(int argc, char** argv) {
  system("mkdir -p cxpng");
  for (int i = 1; i < argc; i++) {
    string filename = argv[1];
    eps2png(filename);
  }
}
