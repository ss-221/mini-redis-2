project(mini-redis-server)

set(SOURCE_FILES src/main.cpp
                 src/server.cpp)

add_executable(mini-redis-server ${SOURCE_FILES})

