#include <client.h>

void PrintErrorMsg(const char *exception_message)
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

int InitClient(int port)
{
    int socketID = socket(AF_INET, SOCK_STREAM, 0);

    if(socketID < 0)
    {
        PrintErrorMsg("Socket creation failed");
        return EXIT_FAILURE;
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);   

    if(inet_pton(AF_INET, "localhost", &serverAddress.sin_addr) < 0)
    {
        PrintErrorMsg("Failed to convert the address");
        return EXIT_FAILURE;
    }

    if(connect(socketID, (struct sockaddr *) (&serverAddress), sizeof(serverAddress)) < 0)
    {
        PrintErrorMsg("Failed to connect to the server");
        return EXIT_FAILURE;
    }

    else
    {
        printf("Successfully connected to the server\n");
    }

    char msgBuffer[max_buffer_size] = {0};

    while(std::cin.getline(msgBuffer, max_buffer_size))
    {
        if(send(socketID, msgBuffer, strlen(msgBuffer), 0) < 0)
        {
            printf("Failed to send");
        }
        else
        {
            std::cout << "Sent: " << msgBuffer << std::endl;
        }
        if(strcmp(msgBuffer, EXIT_CODE) == 0)
        {
            printf("Received EXIT.");
            break;
        }
    }

    return EXIT_SUCCESS;

}