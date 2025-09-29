
add_subdirectory(network)
add_subdirectory(utils)
add_subdirectory(server)
add_subdirectory(database)

find_package(PkgConfig REQUIRED)
pkg_check_modules(HIREDIS REQUIRED hiredis)

# 查找nlohmann/json (头文件库)
find_package(nlohmann_json 3.2.0 REQUIRED)

add_executable(main main.cc)

target_include_directories(main 
    PRIVATE 
        ./network 
        ./utils 
        ./server
        ./database
)

target_link_libraries(main 
    network_shared
    utils_shared 
    server_shared
    database_shared
    nlohmann_json::nlohmann_json
    ${HIREDIS_LIBRARIES}
)
