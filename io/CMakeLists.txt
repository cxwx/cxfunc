cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

file(GLOB sources *.cc)
file(GLOB headers *.hh)

add_library(cxrw SHARED
  ${headers}
  ${sources}
)

install(FILES ${headers} DESTINATION include)
install(TARGETS
  cxrw
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
