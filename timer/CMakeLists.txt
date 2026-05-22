cmake_minimum_required(VERSION 4.3)

message(STATUS "Adding timer module")

# 查找 ROOT
find_package(ROOT QUIET)

if(ROOT_FOUND)
    set(CMAKE_CXX_STANDARD 20)
    message(STATUS "Building timer examples with ROOT")

    # 查找 Geant4
    find_package(Geant4 QUIET)
    if(NOT Geant4_FOUND)
        message(FATAL_ERROR "ROOT found but Geant4 not found. Geant4 is required for timer module.")
    endif()

    # 查找 spdlog
    find_package(spdlog QUIET)
    if(NOT spdlog_FOUND)
        message(FATAL_ERROR "ROOT found but spdlog not found. spdlog is required for timer module.")
    endif()

    # 随机数性能比较示例
    add_executable(cxRandTest
        cxRandCompare.cpp
    )

    target_link_libraries(cxRandTest PRIVATE
        ${ROOT_LIBRARIES}
        ${Geant4_LIBRARIES}
        spdlog::spdlog
    )
    target_include_directories(cxRandTest PRIVATE
        ${ROOT_INCLUDE_DIRS}
        ${Geant4_INCLUDE_DIRS}
    )

    # ROOT 头文件测试示例
    add_executable(cxHeaderTest
        cxHeaderTest.cpp
    )

    target_link_libraries(cxHeaderTest PRIVATE
        ${ROOT_LIBRARIES}
        spdlog::spdlog
    )
    target_include_directories(cxHeaderTest PRIVATE
        ${ROOT_INCLUDE_DIRS}
    )

    # 安装可执行文件到 bin 目录
    install(TARGETS cxRandTest cxHeaderTest
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

else()
    message(WARNING "ROOT not found, timer module examples will not be built")
endif()

# 安装头文件
install(FILES
    benchmark.hh
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cxfunc/timer
)
