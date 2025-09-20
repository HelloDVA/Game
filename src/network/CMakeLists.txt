file(GLOB SOURCES "*.cc")
file(GLOB HEADERS "*.h")

add_library(network SHARED ${SOURCES} ${HEADERS})

target_include_directories(network PRIVATE ../utils)

target_link_libraries(network utils)



