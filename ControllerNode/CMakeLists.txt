cmake_minimum_required(VERSION 3.17)
project(ControllerNode)

set(CMAKE_CXX_STANDARD 20)

add_executable(ControllerNode main.cpp ControllerNode.cpp  ../lib/Huffman.cpp ../lib/Huffman.h ../lib/UtilityText.cpp ../lib/UtilityText.h ControllerNode.h ../lib/List.h ../lib/RequestConstants.h)

target_link_libraries(ControllerNode pthread)
