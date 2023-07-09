#include <data_handler.h>

#define DEBUG(x) std::cout << x << std::endl;

namespace data_handler
{
    using std::cout;
    std::string ProcessCommand(char* command)
    {
        std::stringstream ss(command);
        std::vector<std::string> tokens;
        std::string temp;

        while(ss >> temp)
        {
            tokens.push_back(temp);
        }

        if(tokens.front() == "SET")
        {

        }

        else if(tokens.front() == "GET")
        {

        }

        else if(tokens.front() == "DEL")
        {

        }

        else if(tokens.front() == "TTL")
        {

        }

        else if(tokens.front() == "EXPIRE")
        {

        }

        else
        {
            DEBUG("Invalid command");
            temp = "ERR unknown command ";
            temp += "'" + tokens.front() + "'";
        }


        return temp;
    }
}