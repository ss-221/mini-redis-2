#include <server.h>

void socket_helper::GetError(const char *exception_message)
{

    char buffer[max_buffer_size];
    if ((_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && !_GNU_SOURCE)
    {
        strerror_r(errno, buffer, max_buffer_size);
        std::cout << buffer << std::endl;
    }
    else
    {
        std::cout << strerror_r(errno, buffer, max_buffer_size) << std::endl;
    }
    throw(exception_message);
}

socket_helper::ServerSocket::ServerSocket(int domain_, int type_, int protocol_) : domain(domain_), type(type_), protocol(protocol_)
{
    id = socket(domain, type, protocol);
    if (id < 0)
    {
        GetError("Socket creation failed.");
    }
    address.sin_family = AF_INET;
};

socket_helper::ServerSocket::~ServerSocket()
{
    std::cout << "Destroying the ServerSocket." << std::endl;
    close(id);
}

int socket_helper::Socket::GetSocket()
{
    return id;
}

void socket_helper::Socket::Close()
{
    close(id);
}

void socket_helper::ServerSocket::set_address(in_addr_t addr)
{
    address.sin_addr.s_addr = addr;
}

void socket_helper::ServerSocket::set_address(std::string& addr)
{
    int ret_val = inet_pton(domain, addr.c_str(), &(address.sin_addr));
    if (ret_val == 0)
    {
        std::cout << "Improper IP address. Switching to default" << std::endl;
        set_address(INADDR_ANY);
    }
    else if (ret_val < 0)
    {
        GetError("Could not convert the IP address.");
    }
}

sockaddr_in *socket_helper::Socket::get_address()
{
    return &address;
}

void socket_helper::Socket::set_port(in_port_t port)
{
    address.sin_port = port;
}

void socket_helper::ServerSocket::Bind()
{
    if (bind(id, (sockaddr *)(&address), sizeof(address)) < 0)
    {
        GetError("Failed to bind the address to the socket.");
    }
}

void socket_helper::ServerSocket::Listen(int backlog)
{
    if (listen(id, backlog) < 0)
    {
        GetError("Failed to listen.");
    }
    std::cout << "Listening..." << std::endl;

    while (true)
    {
        IncomingSocket* client_socket = new IncomingSocket;
        Accept(client_socket);
        std::cout << "Connected to the client." << std::endl;

        std::thread worker(&ServerSocket::ClientHandler, this, client_socket->GetSocket());
        worker.detach();
    }
}

void socket_helper::ServerSocket::Accept(IncomingSocket *client_socket)
{
    sockaddr_in *client_sock_addr = client_socket->get_address();
    socklen_t addr_len = sizeof(*client_sock_addr);
    client_socket->id = accept(id, (sockaddr *)(client_sock_addr), &addr_len);

    if (client_socket->GetSocket() < 0)
    {
        GetError("Failed to accept the incoming socket.");
    }
}

socket_helper::IncomingSocket::~IncomingSocket()
{
    std::cout << "Destroying the IncomingSocket." << std::endl;
    close(id);
}

void socket_helper::IncomingSocket::operator=(const int &sock)
{
    id = sock;
}

void socket_helper::IncomingSocket::set_max_read_bytes(size_t bytes)
{
    max_read_bytes = bytes;
}

void socket_helper::IncomingSocket::Send(std::string &message)
{
    if (send(GetSocket(), message.c_str(), message.size(), 0) < 0)
    {
        GetError("Failed to send the message.");
    }
}

void socket_helper::ServerSocket::ClientHandler(int socket)
{
    std::cout << "Entered ClientHandler" << std::endl;
    char buffer[max_buffer_size] = {0};
    int len = 0;
    while((len = read(socket, buffer, max_buffer_size)) > 0)
    {
        if(buffer[max_buffer_size - 1] != 0)
        {
            std::cout << "Message too long: " << len << "\nWill exit" << std::endl;
            strcpy(buffer, "EXIT");
        }
        else
        {
            buffer[len] = 0;
            std::cout << "Message received: " << buffer << std::endl;
            //ProcessCommand(msgBuffer);
        }

        if(strcmp(buffer, "EXIT") == 0)
        {
            printf("EXIT received, closing the connection.\n");
            return;
        }
    }

    return;
}