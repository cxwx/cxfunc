cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# === project ===
project(cxfunc_root LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# === dependencies ===
find_package(ROOT REQUIRED)

# === sources ===
file(GLOB sources *.cc)
file(GLOB headers *.hh)

# === ROOT dictionary ===
message(STATUS "ROOT found, enabling dictionary generation for root.")
include(${ROOT_USE_FILE})
add_definitions(${ROOT_CXX_FLAGS})
root_generate_dictionary(G__cxfunc_root funcROOT.hh LINKDEF LinkDef.h)
set(FUNCROOT_SRCS ${sources} G__cxfunc_root.cxx)

# === library ===
add_library(cxfunc_root SHARED ${FUNCROOT_SRCS})
target_include_directories(cxfunc_root PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${ROOT_INCLUDE_DIRS})
target_link_libraries(cxfunc_root PUBLIC ${ROOT_LIBRARIES})

# === install ===
if(NOT DEFINED CXFUNC_TOP_LEVEL_BUILD)
    set(CMAKE_INSTALL_PREFIX $ENV{HOME}/software/install/)
    install(FILES ${headers} DESTINATION include/cxfunc/)

    install(
        FILES
        ${CMAKE_BINARY_DIR}/libcxfunc_root_rdict.pcm
        ${CMAKE_BINARY_DIR}/libcxfunc_root.rootmap
        DESTINATION lib/
    )
    install(
        TARGETS cxfunc_root
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
endif()
