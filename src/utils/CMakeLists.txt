file(GLOB SOURCES "*.cc")
file(GLOB HEADERS "*.h")

add_library(utils_shared SHARED ${SOURCES} ${HEADERS})




