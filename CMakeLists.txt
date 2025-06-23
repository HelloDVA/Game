# set the version
cmake_minimum_required(VERSION 2.0)

# set the proejct name
project(Documents/Game)

# set the c++ version
set(CMAKE_CXX_STANDARD 14)

# set the file.exe position
set(EXECUTABLE_OUTPUT_PATH ../../bin)

# add child directory
add_subdirectory(src)


