cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# === project ===
project(cxnkg LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# === dependencies ===
find_package(ROOT REQUIRED)

# === sources ===
file(GLOB sources *.cc)
file(GLOB headers *.hh)

# === ROOT dictionary ===
message(STATUS "ROOT found, enabling dictionary generation for nkg.")
include(${ROOT_USE_FILE})
add_definitions(${ROOT_CXX_FLAGS})
root_generate_dictionary(G__cxnkg NKG.hh LINKDEF LinkDef.h)
set(NKG_SRCS ${sources} G__cxnkg.cxx)

# === library ===
add_library(cxnkg SHARED ${NKG_SRCS})
target_include_directories(cxnkg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${ROOT_INCLUDE_DIRS})
target_link_libraries(cxnkg PUBLIC ${ROOT_LIBRARIES})

# === install ===
if(NOT DEFINED CXFUNC_TOP_LEVEL_BUILD)
    set(CMAKE_INSTALL_PREFIX $ENV{HOME}/software/install/)
    install(FILES ${headers} DESTINATION include/cxfunc/)

    install(
        FILES
        ${CMAKE_BINARY_DIR}/libcxnkg_rdict.pcm
        ${CMAKE_BINARY_DIR}/libcxnkg.rootmap
        DESTINATION lib/
    )
    install(
        TARGETS cxnkg
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
endif()
