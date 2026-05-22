// cspell:disable
#include "cxFileIO.hh"

#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/lzma.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filter/zstd.hpp>

#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace cxfunc {

namespace {

struct non_closing_source {
  using char_type = char;
  using category = boost::iostreams::source_tag;

  std::streambuf* buf;
  explicit non_closing_source(std::streambuf* b) : buf(b) {}

  auto read(char* s, std::streamsize n) const -> std::streamsize {
    std::streamsize got = buf->sgetn(s, n);
    return got == 0 ? -1 : got;
  }
  void close() {}
};

auto guess_format(const std::string& filename) -> cxFileIOFormat {
  auto ext = std::filesystem::path(filename).extension().string();
  if (ext == ".gz") { return cxFileIOFormat::kGzip; }
  if (ext == ".bz2") { return cxFileIOFormat::kBzip2; }
  if (ext == ".xz") { return cxFileIOFormat::kLzma; }
  if (ext == ".zst") { return cxFileIOFormat::kZstd; }
  if (ext == ".zlib") { return cxFileIOFormat::kZlib; }
  return cxFileIOFormat::kPlain;
}

void push_decompressor(boost::iostreams::filtering_streambuf<boost::iostreams::input>& f,
                       cxFileIOFormat fmt) {
  switch (fmt) {
    case cxFileIOFormat::kGzip: f.push(boost::iostreams::gzip_decompressor()); break;
    case cxFileIOFormat::kBzip2: f.push(boost::iostreams::bzip2_decompressor()); break;
    case cxFileIOFormat::kLzma: f.push(boost::iostreams::lzma_decompressor()); break;
    case cxFileIOFormat::kZstd: f.push(boost::iostreams::zstd_decompressor()); break;
    case cxFileIOFormat::kZlib: f.push(boost::iostreams::zlib_decompressor()); break;
    case cxFileIOFormat::kAuto:
    case cxFileIOFormat::kPlain: break;
  }
}

void push_compressor(boost::iostreams::filtering_streambuf<boost::iostreams::output>& f,
                     cxFileIOFormat fmt) {
  switch (fmt) {
    case cxFileIOFormat::kGzip: f.push(boost::iostreams::gzip_compressor()); break;
    case cxFileIOFormat::kBzip2: f.push(boost::iostreams::bzip2_compressor()); break;
    case cxFileIOFormat::kLzma: f.push(boost::iostreams::lzma_compressor()); break;
    case cxFileIOFormat::kZstd: f.push(boost::iostreams::zstd_compressor()); break;
    case cxFileIOFormat::kZlib: f.push(boost::iostreams::zlib_compressor()); break;
    case cxFileIOFormat::kAuto:
    case cxFileIOFormat::kPlain: break;
  }
}

}  // namespace

cxFileIO::cxFileIO() {
  theFilter.push(non_closing_source{std::cin.rdbuf()});
}

cxFileIO::cxFileIO(const std::string& filename, cxFileIOFormat fmt) {
  theFileBuf.open(filename, std::ios_base::in | std::ios_base::binary);
  if (!theFileBuf.is_open()) {
    throw std::runtime_error("cannot open file: " + filename);
  }

  if (fmt == cxFileIOFormat::kAuto) { fmt = guess_format(filename); }
  push_decompressor(theFilter, fmt);
  theFilter.push(theFileBuf);
}

cxFileIO::cxFileIO(std::istream& stream, cxFileIOFormat fmt) {
  push_decompressor(theFilter, fmt);
  theFilter.push(non_closing_source{stream.rdbuf()});
}

cxFileIO::~cxFileIO() {
  theFilter.reset();
}

cxFileOW::cxFileOW(const std::string& filename, cxFileIOFormat fmt)
    : filename_(filename), fmt_(fmt) {
  outFileBuf_.open(filename_, std::ios_base::out | std::ios_base::binary);
  if (!outFileBuf_.is_open()) {
    throw std::runtime_error("cannot write file: " + filename_);
  }

  auto realFmt = fmt_;
  if (fmt_ == cxFileIOFormat::kAuto) { realFmt = guess_format(filename_); }
  push_compressor(outFilter_, realFmt);
  outFilter_.push(outFileBuf_);
  outStream_.rdbuf(&outFilter_);
}

cxFileOW::~cxFileOW() = default;

}  // namespace cxfunc
