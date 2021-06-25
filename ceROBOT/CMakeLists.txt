cmake_minimum_required(VERSION 3.17)
project(ceROBOT)

set(CMAKE_CXX_STANDARD 20)

add_executable(ceROBOT main.cpp ../lib/Huffman.cpp ../lib/Huffman.h ../lib/UtilityText.cpp ../lib/UtilityText.h ceROBOT.cpp ceROBOT.h ../lib/json.hpp ../lib/RequestConstants.h)

target_link_libraries(ceROBOT stdc++fs)