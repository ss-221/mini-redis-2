project(mini-redis-client)

set(SOURCE_FILES src/main.cpp
                 src/client.cpp)

add_executable(mini-redis-client ${SOURCE_FILES})

