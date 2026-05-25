cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

file(GLOB headers *.hh)

install(FILES ${headers} DESTINATION include/cxfunc/)
