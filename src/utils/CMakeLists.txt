file(GLOB SOURCES "*.cc")
file(GLOB HEADERS "*.h")

add_library(utils SHARED ${SOURCES} ${HEADERS})




