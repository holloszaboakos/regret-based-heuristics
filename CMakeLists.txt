cmake_minimum_required(VERSION 3.29)

project(regret_prediction_postprocessing)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(regret_prediction_postprocessing
        src/main.cpp
        src/permutation.h
        src/graph_edge.h
        src/regret_data.h
        src/edge_based_branch_and_bounds.h
        src/pair.h
        src/utility/first.h
        src/utility/first_or_default.h
        src/utility/first_or_null.h
        src/utility/group_by.h
        src/utility/sort_by.h
)
