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

        std::transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), ::toupper);

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

    void MetaData::disableTTL()
    {
        m_hasTTL = false;
    }

    void MetaData::enableTTL()
    {
        m_hasTTL = true;;
    }

    bool MetaData::hasTTL()
    {
        return m_hasTTL;
    }

    milliseconds MetaData::getExpiryTime()
    {
        return m_ExpiryTime;
    }

    string DB::HandleSET(vector<string>& tokens)
    {
        if(tokens.size() < 3)
        {
            return "(error) ERR wrong number of arguments for 'set' command";
        }

        return "SET";
    }

    string DB::HandleGET(vector<string>& tokens)
    {
        if(tokens.size() < 2)
        {
            return "(error) ERR wrong number of arguments for 'set' command";
        }

        std::scoped_lock lock(m_lock);
        if(dbKeys.find(tokens[1]) == dbKeys.end())
        {
            return "(nil)";
        }

        return dbKeys[tokens[1]].getValue();
    }

    string DB::HandleDEL(vector<string>& tokens)
    {
        return "DEL";
    }

    string DB::HandleTTL(vector<string>& tokens)
    {
        if(tokens.size() < 2)
        {
            return "(error) ERR wrong number of arguments for 'ttl' command";
        }

        std::scoped_lock(m_lock);
        if(dbKeys.find(tokens.front()) == dbKeys.end())
        {
            return "(integer) -2";
        }
        if(!dbKeys[tokens[1]].hasTTL())
        {
            return "(integer) -1";
        }

        auto secs = duration_cast<seconds>(dbKeys[tokens[1]].getExpiryTime());

        return std::to_string(secs.count());
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