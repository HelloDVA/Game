file(GLOB SOURCES "*.cc")
file(GLOB HEADERS "*.h")

add_library(network_shared SHARED ${SOURCES} ${HEADERS})

target_include_directories(network_shared PRIVATE ../utils)

target_link_libraries(network_shared 
    PUBLIC 
        utils_shared
)

