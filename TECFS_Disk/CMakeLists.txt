cmake_minimum_required(VERSION 3.17)
project(TECFS_Disk)

set(CMAKE_CXX_STANDARD 20)

add_executable(TECFS_Disk main.cpp DiskNode.cpp DiskNode.h ../lib/json.hpp ../lib/tinyxml2.h ../lib/tinyxml2.cpp ../lib/List.h ../lib/Huffman.h ../lib/List.h ../lib/Huffman.cpp ../lib/List.h ../lib/UtilityText.h ../lib/List.h ../lib/UtilityText.cpp DataBlock.cpp DataBlock.h Metadata.cpp Metadata.h)