file(GLOB SOURCES "*.cc")
file(GLOB HEADERS "*.h")

add_library(database_shared SHARED ${SOURCES} ${HEADERS})

target_link_libraries(database_shared PUBLIC mysqlclient)
