#pragma once
#include <string>
#include <fstream>
#include <iostream>

namespace Log
{
#define LOG_PATH "C:/Users/sherlock/Desktop/log.txt"
    
    void inline Debug(const std::string& msg)
    {
        std::ofstream f {};
        f.open(LOG_PATH, std::ios_base::app);
        f << "[DEBUG]" << msg << std::endl;
        f.close();
    }
}
