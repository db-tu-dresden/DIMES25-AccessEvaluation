cmake_minimum_required(VERSION 3.10)
project(gather_investigation)
include(ExternalProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -fno-tree-vectorize -Wno-ignored-attributes -march='native' -fopenmp")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -DNDEBUG -fno-tree-vectorize -Wno-ignored-attributes -march='native' -fopenmp")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|ARM64|arm64)$")
    message(STATUS "ARM architecture detected")
    add_definitions(-DARM)
else()
    message(STATUS "Assume x86 architecture")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
message("Building in '${CMAKE_BUILD_TYPE}' mode.")

ExternalProject_Add(
    perf-cpp-external
    GIT_REPOSITORY "https://github.com/jmuehlig/perf-cpp"
    GIT_TAG "v0.11-dev"
    PREFIX "lib/perf-cpp"
    INSTALL_COMMAND ${CMAKE_COMMAND} -E echo ""
)

add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/perf-profiles/available_counters.csv"
    COMMAND ${CMAKE_COMMAND} -E echo "Building perf-list if needed..."
    COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}/lib/perf-cpp/src/perf-cpp-external-build ${CMAKE_COMMAND} --build . --target perf-list
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/lib/perf-cpp/src/perf-cpp-external-build/perf_list.csv
            ${CMAKE_BINARY_DIR}/perf-profiles/available_counters.csv
    DEPENDS perf-cpp-external
    COMMENT "Generating available_counters.csv"
)

include_directories(
        include
        lib/perf-cpp/src/perf-cpp-external/include
        lib/page-info/
)

link_directories(
        lib/perf-cpp/src/perf-cpp-external-build
)

#####################################
#   Executables                     #
#####################################

set(EXPERIMENTS
    experiment_fixed_data_size src/experiment_0_fixed_data/main.cpp
    experiment_fixed_stride_size src/experiment_1_fixed_stride/main.cpp
    experiment_fixed_partition_count src/experiment_2_fixed_partition/main.cpp
    experiment_multithreading src/experiment_3_multithreading/main.cpp
)

foreach(i RANGE 0 3)
    math(EXPR idx "${i} * 2")
    list(GET EXPERIMENTS ${idx} exe_name)
    math(EXPR idx_src "${idx} + 1")
    list(GET EXPERIMENTS ${idx_src} src_file)

    add_executable(${exe_name} ${src_file})
    target_sources(${exe_name} PRIVATE lib/page-info/page-info.c)
    target_compile_options(${exe_name} PRIVATE -DBENCHMARKNAME="${exe_name}")
    
    target_link_libraries(${exe_name} perf-cpp)
    
    add_custom_command(
        TARGET ${exe_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/results/${exe_name}
        COMMENT "Creating results/${exe_name} directory"
    )
endforeach()

#####################################
#   Custom Commands                #
#####################################
add_custom_target(build-docker sh script/build_docker.sh)
add_custom_target(plots sh script/plot_all.sh)
if(UNIX)
    add_custom_target(perf-list ALL
        DEPENDS ${CMAKE_BINARY_DIR}/perf-profiles/available_counters.csv
    )
endif()
