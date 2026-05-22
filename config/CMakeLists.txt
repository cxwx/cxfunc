cmake_minimum_required(VERSION 3.16)
project(cxconfig VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CXCONFIG_BUILD_TEST "Build test binary" ON)

set(CMAKE_INSTALL_PREFIX
    "$ENV{HOME}/software/install"
    CACHE PATH
    "Install prefix"
)

# --- dependencies ---
find_package(yaml-cpp REQUIRED)
find_package(nlohmann_json REQUIRED)
find_path(tomlplusplus_INCLUDE_DIRS toml++/toml.h)
if(NOT tomlplusplus_INCLUDE_DIRS)
    message(
        FATAL_ERROR
        "toml++ not found. Install it: brew install tomlplusplus"
    )
endif()
find_package(ryml REQUIRED)

# === sources ===
set(CXCONFIG_SRCS ConfigYAML.cc ConfigJson.cc ConfigTomlpp.cc ConfigRapidYml.cc)

set(DICT_HEADERS
    ConfigBase.hh
    ConfigManager.hh
    ConfigTomlpp.hh
    ConfigJson.hh
    ConfigRapidYml.hh
    ConfigYAML.hh
)

find_package(ROOT QUIET)

if(ROOT_FOUND)
    message(STATUS "ROOT found, enabling dictionary generation.")
    include(${ROOT_USE_FILE})
    add_definitions(${ROOT_CXX_FLAGS})
    root_generate_dictionary(G__cxconfig ${DICT_HEADERS} LINKDEF LinkDef.h)
    set(CXCONFIG_SRCS ConfigYAML.cc ConfigJson.cc ConfigTomlpp.cc ConfigRapidYml.cc G__cxconfig.cxx)
    set(ROOT_DEPS ${ROOT_LIBRARIES})
else()
    message(STATUS "ROOT not found, building without dictionary support.")
    set(CXCONFIG_SRCS ConfigYAML.cc ConfigJson.cc ConfigTomlpp.cc ConfigRapidYml.cc)
    set(ROOT_DEPS "")
endif()

add_library(cxconfig SHARED ${CXCONFIG_SRCS} ${DICT_HEADERS})
set_target_properties(
    cxconfig
    PROPERTIES CXX_VISIBILITY_PRESET default POSITION_INDEPENDENT_CODE ON
)
add_library(cxconfig::cxconfig ALIAS cxconfig)

target_include_directories(
    cxconfig
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include>
        ${yaml-cpp_INCLUDE_DIRS}
        ${nlohmann_json_INCLUDE_DIRS}
        ${tomlplusplus_INCLUDE_DIRS}
        ${ryml_INCLUDE_DIRS}
)
target_link_libraries(
    cxconfig
    PUBLIC
        yaml-cpp::yaml-cpp
        nlohmann_json::nlohmann_json
        ryml::ryml
        ${ROOT_DEPS}
)

# === test ===
if(CXCONFIG_BUILD_TEST)
    add_executable(cxconfig_test test/main.cpp)
    target_include_directories(
        cxconfig_test
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    )
    target_link_libraries(cxconfig_test PRIVATE cxconfig::cxconfig)
    target_compile_definitions(
        cxconfig_test
        PRIVATE CXCONFIG_TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test"
    )
endif()

# === install ===
if(NOT DEFINED CXFUNC_TOP_LEVEL_BUILD)
    include(GNUInstallDirs)
    install(
        TARGETS cxconfig
        EXPORT cxconfigTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(
        FILES
            ConfigBase.hh
            ConfigManager.hh
            ConfigYAML.hh
            ConfigJson.hh
            ConfigTomlpp.hh
            ConfigRapidYml.hh
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cxfunc
    )
    install(
        EXPORT cxconfigTargets
        NAMESPACE cxconfig::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxconfig
    )
    if(ROOT_FOUND)
        install(
            FILES
                ${CMAKE_BINARY_DIR}/libcxconfig_rdict.pcm
                ${CMAKE_BINARY_DIR}/libcxconfig.rootmap
            DESTINATION lib/
        )
    endif()
endif()
