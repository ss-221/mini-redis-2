#include <data_handler.h>

#define DEBUG(x) std::cout << x << std::endl;

namespace data_handler
{

    DB db;
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
            temp = db.HandleSET(tokens);
        }

        else if(tokens.front() == "GET")
        {
            temp = db.HandleGET(tokens);
        }

        else if(tokens.front() == "DEL")
        {
            temp = db.HandleDEL(tokens);
        }

        else if(tokens.front() == "TTL")
        {
            temp = db.HandleTTL(tokens);
        }

        else if(tokens.front() == "EXPIRE")
        {
            temp = db.HandleEXPIRE(tokens);
        }

        else if(tokens.front() == "KEYS")
        {
            temp = db.HandleKEYS(tokens);
        }

        else
        {
            DEBUG("Invalid command");
            temp = "ERR unknown command ";
            temp += "'" + tokens.front() + "'";
        }

        return temp;
    }

    string MetaData::getValue() const
    {
        return m_Value;
    }

    string DB::HandleSET(vector<string>& tokens)
    {
        return "SET";
    }

    string DB::HandleGET(vector<string>& tokens)
    {
        std::scoped_lock lock(m_lock);
        if(dbKeys.find(tokens.front()) == dbKeys.end())
        {
            return "(nil)";
        }

        return dbKeys[tokens.front()].getValue();
    }

    string DB::HandleDEL(vector<string>& tokens)
    {
        return "DEL";
    }

    string DB::HandleTTL(vector<string>& tokens)
    {
        return "TTL";
    }

    string DB::HandleEXPIRE(vector<string>& tokens)
    {
        return "EXPIRE";
    }

    string DB::HandleKEYS(vector<string>& tokens)
    {
        return "KEYS";
    }
}