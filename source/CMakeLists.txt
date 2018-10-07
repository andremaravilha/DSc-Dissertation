cmake_minimum_required(VERSION 3.9)


# ==============================================================================
# Project info

project(phd)


# ==============================================================================
# C++ standard and settings

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)


# ==============================================================================
# External dependencies

# Gurobi (path)
if (NOT DEFINED GUROBI_PATH)
    set(GUROBI_PATH "/opt/gurobi")
endif()

# Gurobi (libraries)
if (NOT DEFINED GUROBI_LIBRARY)
    set(GUROBI_LIBRARY gurobi80)
endif()


# ==============================================================================
# Paths to search for headers

include_directories(
        ${PROJECT_SOURCE_DIR}/lib/
        ${GUROBI_PATH}/include/)


# ==============================================================================
# Paths to search for libraries

link_directories(
        ${GUROBI_PATH}/lib/)


# ==============================================================================
# Libraries to link

set(GUROBI_LIBS gurobi_c++ ${GUROBI_LIBRARY})
set(OTHER_LIBS  pthread m)


# ==============================================================================
# Source files

set(SOURCE_FILES
        src/main.cpp
        src/problem/problem.h src/problem/problem.cpp
        src/algorithm/algorithm.h
        src/neighborhood/neighborhood.h
        src/util/common.h src/util/common.cpp
        src/util/local_search.h src/util/local_search.cpp
        src/neighborhood/shift.h src/neighborhood/shift.cpp
        src/neighborhood/exchange.h src/neighborhood/exchange.cpp
        src/neighborhood/reassignment.h src/neighborhood/reassignment.cpp
        src/neighborhood/swap.h src/neighborhood/swap.cpp
        src/neighborhood/direct_swap.h src/neighborhood/direct_swap.cpp
        src/algorithm/mip/mip_precedence.h src/algorithm/mip/mip_precedence.cpp
        src/algorithm/mip/mip_linear_ordering.h src/algorithm/mip/mip_linear_ordering.cpp
        src/algorithm/mip/mip_arc_time_indexed.h src/algorithm/mip/mip_arc_time_indexed.cpp
        src/algorithm/heuristic/greedy.h src/algorithm/heuristic/greedy.cpp
        src/algorithm/heuristic/neh.h src/algorithm/heuristic/neh.cpp
        src/algorithm/heuristic/ils.h src/algorithm/heuristic/ils.cpp
        )


# ==============================================================================
# Targets

add_executable(schd ${SOURCE_FILES})
target_link_libraries(schd ${GUROBI_LIBS} ${OTHER_LIBS})
