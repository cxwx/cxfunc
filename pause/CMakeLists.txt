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
    set(CMAKE_INSTALL_PREFIX $ENV{HOME}/software/install/)
    install(FILES ${headers} DESTINATION include/cxfunc/)
    install(
        TARGETS cxfunc_pause
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )

    # Only install ROOT files if ROOT was found
    if(ROOT_FOUND)
        install(
            FILES
            ${CMAKE_BINARY_DIR}/libcxfunc_pause_rdict.pcm
            ${CMAKE_BINARY_DIR}/libcxfunc_pause.rootmap
            DESTINATION lib/
        )
    endif()
endif()