cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# === project ===
project(cxatomsphere VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# === dependencies ===
find_package(ROOT REQUIRED)

# === sources ===
file(GLOB sources *.cc)
file(GLOB headers *.hh)

# === ROOT dictionary ===
message(STATUS "ROOT found, enabling dictionary generation for atomsphere.")
include(${ROOT_USE_FILE})
add_definitions(${ROOT_CXX_FLAGS})
root_generate_dictionary(G__cxatomsphere atmosphere.hh LINKDEF LinkDef.h)
set(ATOMSPHERE_SRCS ${sources} G__cxatomsphere.cxx)

# === library ===
add_library(cxatomsphere SHARED ${ATOMSPHERE_SRCS})
target_include_directories(cxatomsphere PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
    ${ROOT_INCLUDE_DIRS}
)
target_link_libraries(cxatomsphere PUBLIC ${ROOT_LIBRARIES})

# === install ===
if(NOT DEFINED CXFUNC_TOP_LEVEL_BUILD)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    # Install library
    install(
        TARGETS cxatomsphere
        EXPORT cxatomsphereTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    # Install headers
    install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cxfunc)

    # Install ROOT dictionary files
    install(
        FILES
            ${CMAKE_BINARY_DIR}/libcxatomsphere_rdict.pcm
            ${CMAKE_BINARY_DIR}/libcxatomsphere.rootmap
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    # Export targets
    install(
        EXPORT cxatomsphereTargets
        NAMESPACE cxatomsphere::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxatomsphere
    )

    # Create package version file
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/cxatomsphereConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    # Create package config file
    configure_package_config_file(
        "${CMAKE_CURRENT_LIST_DIR}/cmake/cxatomsphereConfig.in.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cxatomsphereConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxatomsphere
    )

    # Install package config files
    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/cxatomsphereConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/cxatomsphereConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxatomsphere
    )
endif()
