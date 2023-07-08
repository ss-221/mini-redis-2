#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <filesystem>
#include <iostream>
#include <thread>

constexpr int max_buffer_size = 1024;

namespace socket_helper
{

    void GetError(const char *);

    class Socket;
    class ServerSocket;
    class IncomingSocket;




    class Socket
    {
    public:
        Socket() = default;
        virtual ~Socket() = default;

        int GetSocket();

        void set_port(in_port_t port);
        sockaddr_in *get_address();
        void Close();

        int id;
        sockaddr_in address;
    };




    class ServerSocket : public Socket
    {
    public:
        ServerSocket(int domain_, int type_, int protocol_);
        ServerSocket(int dom, int t) : ServerSocket(dom, t, 0){};
        ServerSocket(int dom) : ServerSocket(dom, SOCK_STREAM, 0){};
        ServerSocket() : ServerSocket(AF_INET, SOCK_STREAM, 0){};
        ~ServerSocket();

        void set_address(in_addr_t);
        void set_address(std::string&);
        void Bind();
        void Listen(int);
        void Accept(IncomingSocket*);
        void ClientHandler(int);

    private:
        int domain, type, protocol;
        int max_backlog;
    };




    class IncomingSocket : public Socket
    {
    public:
        IncomingSocket() = default;
        ~IncomingSocket();

        void operator=(const int &sock);
        void set_max_read_bytes(size_t);
        void Send(std::string &);
        void SendFile(std::string);

    private:
        size_t max_read_bytes;
    };

}

#endif