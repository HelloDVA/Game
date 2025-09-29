
file(GLOB SOURCES "*.cc")
file(GLOB HEADERS "*.h")

add_library(server_shared SHARED ${SOURCES} ${HEADERS})

target_include_directories(server_shared PRIVATE ../network)

target_link_libraries(server_shared PUBLIC network_shared)

