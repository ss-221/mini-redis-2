#include <server.h>
#include <bit>

constexpr int max_backlog_queue = 10;

int main(int argc, char *argv[])
{
    try
    {
        using namespace socket_helper;

        ServerSocket server_socket;
        server_socket.set_address(INADDR_ANY);

        in_port_t port = atoi(argv[1]);
        if (std::endian::native == std::endian::little)
            port = ntohs(port);

        server_socket.set_port(port);
        server_socket.Bind();

        server_socket.Listen(max_backlog_queue);
    }
    catch (const char *exception_message)
    {
        std::cout << "Exception: " << exception_message << '\n';
        return EXIT_FAILURE;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cout << "Unknown exception." << '\n';
        return EXIT_FAILURE;
    }

    return 0;
}