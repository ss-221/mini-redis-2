#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <algorithm>

namespace data_handler
{
    using namespace std::chrono;
    using std::string;
    using std::vector;
    std::string ProcessCommand(char*);
    milliseconds GetCurrTime();
    bool isValidNum(string&);
    
    class MetaData
    {
        public:
        MetaData() = default;
        string getValue() const;
        void setValue(string& );
        milliseconds getExpiryTime() const;
        void setExpiryTime(const milliseconds&);
        void enableTTL();
        void disableTTL();
        bool hasTTL();

        private:
            string m_Value;
            milliseconds m_ExpiryTime;
            bool m_hasTTL;
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
            bool hasExpired(string&);
        private:
            std::mutex m_lock;
            std::unordered_map<string, MetaData> dbKeys;
    };


}

#endif