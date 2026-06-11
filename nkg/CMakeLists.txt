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
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    # Install library
    install(
        TARGETS cxnkg
        EXPORT cxnkgTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    # Install headers
    install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cxfunc)

    # Install ROOT dictionary files
    install(
        FILES
            ${CMAKE_BINARY_DIR}/libcxnkg_rdict.pcm
            ${CMAKE_BINARY_DIR}/libcxnkg.rootmap
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    # Export targets
    install(
        EXPORT cxnkgTargets
        NAMESPACE cxnkg::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxnkg
    )

    # Create package version file
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/cxnkgConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    # Create package config file
    configure_package_config_file(
        "${CMAKE_CURRENT_LIST_DIR}/cmake/cxnkgConfig.in.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cxnkgConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxnkg
    )

    # Install package config files
    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/cxnkgConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/cxnkgConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxnkg
    )
endif()
