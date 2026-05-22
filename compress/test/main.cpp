#include "cxFileIO.hh"
#include <iostream>
#include <string>

int main() {
    // Test 1: gzip write + read
    std::string data1 = "hello cxFileOW\ngoodbye world\n";
    {
        cxfunc::cxFileOW w("/tmp/test_cxfileow.txt.gz");
        w.ostream().write(data1.data(), static_cast<std::streamsize>(data1.size()));
        w.ostream().flush();
    }

    cxfunc::cxFileIO r1("/tmp/test_cxfileow.txt.gz");
    std::string content1(std::istreambuf_iterator(r1.stream()), {});
    std::cout << "gzip: write " << data1.size() << " read " << content1.size() << " "
              << (content1 == data1 ? "PASS" : "FAIL") << "\n";

    // Test 2: bz2 write + read
    std::string data2 = "hello bzip2\ngoodbye world\n";
    {
        cxfunc::cxFileOW w("/tmp/test_cxfileow.txt.bz2", cxfunc::cxFileIOFormat::kBzip2);
        w.ostream().write(data2.data(), static_cast<std::streamsize>(data2.size()));
        w.ostream().flush();
    }

    cxfunc::cxFileIO r2("/tmp/test_cxfileow.txt.bz2");
    std::string content2(std::istreambuf_iterator(r2.stream()), {});
    std::cout << "bz2: write " << data2.size() << " read " << content2.size() << " "
              << (content2 == data2 ? "PASS" : "FAIL") << "\n";

    // Test 3: plain
    std::string data3 = "hello plain\n";
    {
        cxfunc::cxFileOW w("/tmp/test_cxfileow.txt");
        w.ostream().write(data3.data(), static_cast<std::streamsize>(data3.size()));
        w.ostream().flush();
    }

    cxfunc::cxFileIO r3("/tmp/test_cxfileow.txt");
    std::string content3(std::istreambuf_iterator(r3.stream()), {});
    std::cout << "plain: write " << data3.size() << " read " << content3.size() << " "
              << (content3 == data3 ? "PASS" : "FAIL") << "\n";

    std::cout << "ALL DONE\n";
    return 0;
}
