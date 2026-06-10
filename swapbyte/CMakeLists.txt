cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# === project ===
project(cxswapbyte LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# === dependencies ===
find_package(ROOT QUIET)

# === sources ===
file(GLOB sources *.cc)
file(GLOB headers *.hh)

# === ROOT dictionary ===
if(ROOT_FOUND)
    message(STATUS "ROOT found, enabling dictionary generation for swapbyte.")
    include(${ROOT_USE_FILE})
    add_definitions(${ROOT_CXX_FLAGS})
    include_directories(${CMAKE_CURRENT_SOURCE_DIR})
    root_generate_dictionary(G__cxswapbyte swapbyte.hh LINKDEF LinkDef.h)
    set(SWAPBYTE_SRCS ${sources} G__cxswapbyte.cxx)
else()
    set(SWAPBYTE_SRCS ${sources})
    message(STATUS "ROOT not found, building swapbyte without dictionary.")
endif()

# === library ===
if(ROOT_FOUND)
    add_library(cxswapbyte SHARED ${SWAPBYTE_SRCS})
    target_include_directories(cxswapbyte PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${ROOT_INCLUDE_DIRS})
    target_link_libraries(cxswapbyte PUBLIC ${ROOT_LIBRARIES})
else()
    add_library(cxswapbyte INTERFACE ${SWAPBYTE_SRCS})
    target_include_directories(cxswapbyte INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# === install ===
if(NOT DEFINED CXFUNC_TOP_LEVEL_BUILD)
    set(CMAKE_INSTALL_PREFIX $ENV{HOME}/software/install/)
    install(FILES ${headers} DESTINATION include/cxfunc/)

    if(ROOT_FOUND)
        install(
            FILES
                ${CMAKE_BINARY_DIR}/libcxswapbyte_rdict.pcm
                ${CMAKE_BINARY_DIR}/libcxswapbyte.rootmap
            DESTINATION lib/
        )
        install(
            TARGETS cxswapbyte
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
        )
    endif()
endif()