#pragma once
#include "process_block.h"
#include "scheduler.h"
#include <string> 
#include <vector> 
#include <ctime>
class console
{
private:
    std::vector<process_block*> *processes = new std::vector<process_block*>;    
    std::vector<core> *cores; 
    scheduler *consoleScheduler; 
    bool initialized = false; 
public:
    console(/* args */);

    void displayBanner(); 
    void printReport();
    void printProcessSMI();
    void vmstat();
    void interpreter(const std::string &command);
    void start();
    static std::string timestamp() {
        std::time_t now = std::time(0);
        char buf[80];
        struct tm timeinfo;
        localtime_s(&timeinfo, &now);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
        return std::string(buf);
    }

};

