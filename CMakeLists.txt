cmake_minimum_required(VERSION 3.12)
project(node_pipeline)

set(nlohmann_json_DIR  ${CMAKE_SOURCE_DIR}/dependencies/nlohmannJson/share/cmake/nlohmann_json/)


find_package(nlohmann_json 3.2.0 REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_executable(PartA  PartA.cpp)
add_executable(PartB  PartB.cpp)
add_executable(tests  tests.cpp)
target_link_libraries(PartA PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(PartB PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(tests PRIVATE nlohmann_json::nlohmann_json)
