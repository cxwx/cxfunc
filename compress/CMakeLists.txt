cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
set(CX_VER_MAJ 0) # major
set(CX_VER_MIN 6) # minor
set(CX_VER_PAT 0) # patch
set(CX_VER_SS ${CX_VER_MAJ}.${CX_VER_MIN}.${CX_VER_PAT})
message("Version: ${CX_VER_SS}")

project(cxfileio VERSION ${CX_VER_SS} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
find_package(Boost REQUIRED COMPONENTS iostreams)
find_package(ROOT QUIET)

# === library ===
if(ROOT_FOUND)
    message(STATUS "ROOT found, enabling dictionary generation.")
    include(${ROOT_USE_FILE})
    add_definitions(${ROOT_CXX_FLAGS})
    root_generate_dictionary(G__cxfileio cxFileIO.hh LINKDEF LinkDef.h)
    set(CX_SRCS cxFileIO.cc G__cxfileio.cxx)
else()
    message(STATUS "ROOT not found, building without dictionary support.")
    set(CX_SRCS cxFileIO.cc)
endif()

add_library(cxfileio SHARED ${CX_SRCS})
set_target_properties(
    cxfileio
    PROPERTIES VERSION ${CX_VER_SS} SOVERSION ${CX_VER_MAJ}
)

target_include_directories(cxfileio PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(cxfileio PUBLIC Boost::iostreams ${ROOT_LIBRARIES})

# === test ===
option(CX_BUILD_TEST "Build test binary" ON)
if(CX_BUILD_TEST)
    add_executable(cxfileio_test test/main.cpp)
    target_include_directories(
        cxfileio_test
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    )
    target_link_libraries(cxfileio_test PRIVATE cxfileio)
endif()

# === install ===
if(NOT DEFINED CXFUNC_TOP_LEVEL_BUILD)
    set(CMAKE_INSTALL_PREFIX $ENV{HOME}/software/install/)
    install(FILES cxFileIO.hh DESTINATION include/cxfunc/)

    if(ROOT_FOUND)
        install(
            FILES
                ${CMAKE_BINARY_DIR}/libcxfileio_rdict.pcm
                ${CMAKE_BINARY_DIR}/libcxfileio.rootmap
            DESTINATION lib/
        )
    endif()
    install(
        TARGETS cxfileio
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )
endif()
