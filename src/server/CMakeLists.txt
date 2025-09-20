
file(GLOB SOURCES "*.cc")
file(GLOB HEADERS "*.h")

add_library(server SHARED ${SOURCES} ${HEADERS})

target_include_directories(server PRIVATE ../network)

target_link_libraries(server network)

