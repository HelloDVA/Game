add_subdirectory(network)
add_subdirectory(server)
add_subdirectory(utils)
add_subdirectory(database)

#include_directories(${PROJECT_SOURCE_DIR}/network)
#include_directories(${PROJECT_SOURCE_DIR}/utils)
#include_directories(${PROJECT_SOURCE_DIR}/server)

add_executable(main main.cc)

target_include_directories(main PRIVATE ./server)

target_link_libraries(main network utils server database)
