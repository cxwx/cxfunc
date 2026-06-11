cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# === project ===
project(cxfunc_pause LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# === dependencies ===
find_package(ROOT)

# === sources ===
file(GLOB sources *.cc)
file(GLOB headers *.hh)

# === ROOT dictionary (conditional) ===
if(ROOT_FOUND)
    message(STATUS "ROOT found, enabling dictionary generation for pause.")
    include(${ROOT_USE_FILE})
    add_definitions(${ROOT_CXX_FLAGS})
    root_generate_dictionary(G__cxfunc_pause pause.hh LINKDEF LinkDef.h)
    set(PAUSE_SRCS ${sources} G__cxfunc_pause.cxx)
else()
    message(STATUS "ROOT not found, building pause without dictionary support.")
    set(PAUSE_SRCS ${sources})
endif()

# === library ===
add_library(cxfunc_pause SHARED ${PAUSE_SRCS})
target_include_directories(cxfunc_pause PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(ROOT_FOUND)
    target_include_directories(cxfunc_pause PUBLIC ${ROOT_INCLUDE_DIRS})
    target_link_libraries(cxfunc_pause PUBLIC ${ROOT_LIBRARIES})
endif()

# === install ===
if(NOT DEFINED CXFUNC_TOP_LEVEL_BUILD)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    # Install library
    install(
        TARGETS cxfunc_pause
        EXPORT cxfunc_pauseTargets
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    # Install headers
    install(FILES ${headers} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cxfunc)

    # Install ROOT dictionary files
    if(ROOT_FOUND)
        install(
            FILES
                ${CMAKE_BINARY_DIR}/libcxfunc_pause_rdict.pcm
                ${CMAKE_BINARY_DIR}/libcxfunc_pause.rootmap
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )
    endif()

    # Export targets
    install(
        EXPORT cxfunc_pauseTargets
        NAMESPACE cxfunc_pause::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxfunc_pause
    )

    # Create package version file
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_pauseConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
    )

    # Create package config file
    configure_package_config_file(
        "${CMAKE_CURRENT_LIST_DIR}/cmake/cxfunc_pauseConfig.in.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_pauseConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxfunc_pause
    )

    # Install package config files
    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_pauseConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/cxfunc_pauseConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxfunc_pause
    )
endif()