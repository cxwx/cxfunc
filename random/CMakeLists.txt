cmake_minimum_required(VERSION 3.16)
project(cxrandom VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CXRANDOM_BUILD_TEST "Build test binary" ON)

set(CMAKE_INSTALL_PREFIX
    "$ENV{HOME}/software/install"
    CACHE PATH
    "Install prefix"
)

# === sources ===
set(CXRANDOM_SRCS randomSeed3.cc)

set(DICT_HEADERS
    randomSeed3.hh
)

find_package(ROOT QUIET)

if(ROOT_FOUND)
    message(STATUS "ROOT found, enabling dictionary generation.")
    include(${ROOT_USE_FILE})
    add_definitions(${ROOT_CXX_FLAGS})
    root_generate_dictionary(G__cxrandom ${DICT_HEADERS} LINKDEF LinkDef.h)
    set(CXRANDOM_SRCS randomSeed3.cc G__cxrandom.cxx)
    set(ROOT_DEPS ${ROOT_LIBRARIES})
else()
    message(STATUS "ROOT not found, building without dictionary support.")
    set(CXRANDOM_SRCS randomSeed3.cc)
    set(ROOT_DEPS "")
endif()

add_library(cxrandom SHARED ${CXRANDOM_SRCS} ${DICT_HEADERS})

set_target_properties(
    cxrandom
    PROPERTIES CXX_VISIBILITY_PRESET default POSITION_INDEPENDENT_CODE ON
)
add_library(cxrandom::cxrandom ALIAS cxrandom)

target_include_directories(
    cxrandom
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(
    cxrandom
    PUBLIC
        ${ROOT_DEPS}
)

# === test ===
if(CXRANDOM_BUILD_TEST)
    add_executable(cxrandom_test test/main.cpp)
    target_include_directories(
        cxrandom_test
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    )
    target_link_libraries(cxrandom_test PRIVATE cxrandom::cxrandom)
endif()

# === install ===
if(NOT DEFINED CXFUNC_TOP_LEVEL_BUILD)
    include(GNUInstallDirs)
    install(
        TARGETS cxrandom
        EXPORT cxrandomTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(
        FILES
            randomSeed3.hh
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cxfunc
    )
    install(
        EXPORT cxrandomTargets
        NAMESPACE cxrandom::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxrandom
    )
    if(ROOT_FOUND)
        install(
            FILES
                ${CMAKE_BINARY_DIR}/libcxrandom_rdict.pcm
                ${CMAKE_BINARY_DIR}/libcxrandom.rootmap
            DESTINATION lib/
        )
    endif()
endif()
