#include <data_handler.h>

#define DEBUG(x) std::cout << x << std::endl;

namespace data_handler
{

    DB db;
    using std::cout;

    milliseconds GetCurrTime()
    {
        return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    }
    
    bool isValidNum(string& str)
    {
        return str.find_first_not_of("0123456789") == string::npos;
    }

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

    void MetaData::setValue(string& val)
    {
        m_Value = val;
    }

    void MetaData::disableTTL()
    {
        m_hasTTL = false;
    }

    void MetaData::enableTTL()
    {
        m_hasTTL = true;
    }

    bool MetaData::hasTTL()
    {
        return m_hasTTL;
    }

    milliseconds MetaData::getExpiryTime() const
    {
        return m_ExpiryTime;
    }

    void MetaData::setExpiryTime(const milliseconds& ms)
    {
        m_ExpiryTime = ms;
    }

    bool DB::hasExpired(string& key)
    {
        milliseconds currTime = GetCurrTime();
        if(dbKeys[key].hasTTL() && currTime >= dbKeys[key].getExpiryTime())
        {
            return true;
        }

        return false;
    }

    string DB::HandleSET(vector<string>& tokens)
    {
        if(tokens.size() < 3)
        {
            return "(error) ERR wrong number of arguments for 'set' command";
        }

        bool hasEX = false, hasPX = false, hasNX = false, hasXX = false, hasKeepTTL = false;
        bool isValid = true;
        seconds s_expiryDur;
        milliseconds ms_expiryDur; 

        for(int idx = 3; idx < tokens.size(); idx++)
        {
            if(tokens[idx] == "NX")
            {
                hasNX = true;
            }

            else if(tokens[idx] == "XX")
            {
                hasXX = true;
            }

            else if(tokens[idx] == "EX" || tokens[idx] == "PX")
            {
                if(idx == tokens.size() - 1)
                {
                    isValid = false; break;
                }

                if(!isValidNum(tokens[idx + 1]))
                {
                    isValid = false; break;
                }

                if(tokens[idx] == "EX")
                {
                    hasEX = true;
                    seconds s_expiryDur = seconds(stoi(tokens[idx + 1]));
                    if(s_expiryDur == seconds::zero())
                    {
                        return "(error) ERR value is not an integer or out of range";
                    }

                    ms_expiryDur = duration_cast<milliseconds>(s_expiryDur);
                    idx += 1;
                }
                else
                {
                    hasPX = true;
                    ms_expiryDur = milliseconds(stoi(tokens[idx + 1]));
                    if(ms_expiryDur == seconds::zero())
                    {
                        return "(error) ERR value is not an integer or out of range";
                    }
                    idx += 1;
                }
            }

            else if(tokens[idx] == "KEEPTTL")
            {
                hasKeepTTL = true;
            }

            else
            {
                isValid = false;
                break;
            }
        }

        if((hasEX && hasPX) || (hasEX && hasKeepTTL) | (hasPX && hasKeepTTL))
        {
            isValid = false;
        }

        if(hasNX && hasXX)
        {
            isValid = false;
        }
        
        if(!isValid)
        {
            return "(error) ERR syntax error";
        }

        string& key = tokens[1];
        string& val = tokens[2];
        std::scoped_lock lock(m_lock);

        if(hasNX)
        {
            if(dbKeys.find(key) != dbKeys.end() && !hasExpired(key))
            {
                return "(nil)";
            }
        }

        if(hasXX)
        {
            if(dbKeys.find(key) == dbKeys.end() || hasExpired(key))
            {
                return "(nil)";
            }
        }

        dbKeys[key].setValue(val);

        if(!hasKeepTTL)
        {
            dbKeys[key].disableTTL();
        }

        if(hasEX || hasPX)
        {
            ms_expiryDur += GetCurrTime();
            dbKeys[key].enableTTL();
            dbKeys[key].setExpiryTime(ms_expiryDur);
        }
        
        return "ok";
    }

    string DB::HandleGET(vector<string>& tokens)
    {
        if(tokens.size() != 2)
        {
            return "(error) ERR wrong number of arguments for 'get' command";
        }

        std::scoped_lock lock(m_lock);
        if(dbKeys.find(tokens[1]) == dbKeys.end() || hasExpired(tokens[1]))
        {
            return "(nil)";
        }

        return dbKeys[tokens[1]].getValue();
    }

    string DB::HandleDEL(vector<string>& tokens)
    {
        if(tokens.size() < 2)
        {
            return "(error) ERR wrong number of arguments for 'del' command";
        }
        std::scoped_lock lock(m_lock);

        int delCount = 0;
        for(int idx = 1; idx < tokens.size(); idx++)
        {
            if(dbKeys.find(tokens[idx]) != dbKeys.end() && !hasExpired(tokens[idx]))
            {
                dbKeys.erase(tokens[idx]);
                delCount += 1;
            }
        }

        return string("(integer) " + std::to_string(delCount));
    }

    string DB::HandleTTL(vector<string>& tokens)
    {
        if(tokens.size() < 2)
        {
            return "(error) ERR wrong number of arguments for 'ttl' command";
        }

        std::scoped_lock lock(m_lock);
        if(dbKeys.find(tokens[1]) == dbKeys.end())
        {
            return "(integer) -2";
        }
        if(!dbKeys[tokens[1]].hasTTL())
        {
            return "(integer) -1";
        }

        if(GetCurrTime() >= dbKeys[tokens[1]].getExpiryTime())
        {
             return "(integer) -2";
        }

        auto secs = duration_cast<seconds>(dbKeys[tokens[1]].getExpiryTime() - GetCurrTime());

        return std::to_string(secs.count());
    }

    string DB::HandleEXPIRE(vector<string>& tokens)
    {
        if(tokens.size() < 3)
        {
            return "(error) ERR wrong number of arguments for 'expire' command";
        }

        bool hasNX = false, hasXX = false, hasGT = false, hasLT = false;

        for(int idx = 3; idx < tokens.size(); idx++)
        {
            if(tokens[idx] == "NX")
            {
                hasNX = true;
            }

            if(tokens[idx] == "XX")
            {
                hasXX = true;
            }

            if(tokens[idx] == "GT")
            {
                hasGT = true;
            }

            if(tokens[idx] == "LT")
            {
                hasLT = true;
            }
        }

        if(hasNX && (hasXX || hasGT || hasLT))
        {
            return "(error) ERR NX and XX, GT or LT options at the same time are not compatible";
        }

        if(hasGT && hasLT)
        {
            return "(error) ERR GT and LT options at the same time are not compatible";
        }

        if(!isValidNum(tokens[2]))
        {
            return "(error) ERR syntax error";
        }

        milliseconds ms_expiryDur = duration_cast<milliseconds>(seconds(stoi(tokens[2])));

        if(ms_expiryDur == milliseconds::zero())
        {
            return "(error) ERR syntax error";
        }
        
        ms_expiryDur += GetCurrTime();
        string& key = tokens[1];
        std::scoped_lock lock(m_lock);

        if(dbKeys.find(key) == dbKeys.end() || hasExpired(key))
        {
            return "(integer) 0";
        }

        if(hasNX)
        {
            if(dbKeys[key].hasTTL())
            {
                return "(integer) 0";
            }
        }

        if(hasXX)
        {
            if(!dbKeys[key].hasTTL())
            {
                return "(integer) 0";
            }
        }

        if(hasGT)
        {
            if(!dbKeys[key].hasTTL() || (ms_expiryDur <= dbKeys[key].getExpiryTime()))
            {
                return "(integer) 0";
            }
        }

        if(hasLT)
        {
            if(dbKeys[key].hasTTL() && (ms_expiryDur >= dbKeys[key].getExpiryTime()))
            {
                return "(integer) 0";
            }
        }

        dbKeys[key].enableTTL();
        dbKeys[key].setExpiryTime(ms_expiryDur);

        return "(integer) 1";
    }

    string DB::HandleKEYS(vector<string>& tokens)
    {
        return "KEYS";
    }
}