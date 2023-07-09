#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <mutex>

namespace data_handler
{
    using namespace std::chrono;
    using std::string;
    using std::vector;
    std::string ProcessCommand(char*);
    
    class MetaData
    {
        public:
        MetaData() = default;
        string getValue() const;

        private:
            string m_Value;
            milliseconds m_ExpiryTime;
    };

    class DB
    {
        public:
            string HandleSET(vector<string>&);
            string HandleGET(vector<string>&);
            string HandleDEL(vector<string>&);
            string HandleTTL(vector<string>&);
            string HandleEXPIRE(vector<string>&);
            string HandleKEYS(vector<string>&);
        private:
            std::mutex m_lock;
            std::unordered_map<string, MetaData> dbKeys;
    };


}

#endif