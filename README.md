# mini-redis-2

A basic version of [Redis](https://redis.io/) written in C++ that supports a limited set of functions.

## Steps to build it-
Make a build folder and navigate to it
```
mini-redis-2$ mkdir build && cd build
```
Run CMake
```
mini-redis-2/build$ cmake .. && make
```

Run the server (provide a port no. as the arg.)

```
mini-redis-2/build/$ mini-redis-server/mini-redis-server 8080
```

Run a client in another terminal (provide a port no. as the arg.)

```
mini-redis-2/build/$ mini-redis-client/mini-redis-client 8080
```

### Commands supported

* set 
* get
* del
* ttl
* expire
* keys

The server and client can be closed using ctrl + c. The client can additionally be closed by passing "EXIT".

(NOTE: set is supported based on Redis 6.0 i.e. GET, EXAT and PXAT flags are unavailable. The other commands support the same flags as in Redis 7.0)
