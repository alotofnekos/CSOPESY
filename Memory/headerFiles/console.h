#pragma once
#include "process_block.h"
#include "scheduler.h"
#include "Screen.h"
#include "ScreenManager.h"
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
    ScreenManager manager;
public:
    console(/* args */);

    void displayBanner(); 
    void printReport(bool toFile);
    void printProcessSMI(bool toFile);
    void vmstat(bool toFile);
    void interpreter(const std::string &command);
    void start();
    static std::string timestamp() {
        std::time_t now = std::time(0);
        char buf[80];
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
        return std::string(buf);
    }

};

