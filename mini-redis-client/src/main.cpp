#include <client.h>

int main(int argc, char *argv[])
{
    if(argc == 1)
    {
        std::cout << "Please call mini-redis-client with a port.\nEx: mini-redis-client 8080\n" << std::endl;
        return 0;
    }

    try
    {
        if(InitClient(atoi(argv[1])) == EXIT_FAILURE)
        {
            std::cout << "Failed to start the client. Exiting.\n" << std::endl;
            return EXIT_FAILURE;
        }
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