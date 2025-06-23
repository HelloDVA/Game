file(GLOB SOURCES "*.cc")
file(GLOB HEADERS "*.h")

add_library(database SHARED ${SOURCES} ${HEADERS})

target_link_libraries(database PRIVATE mysqlclient)
