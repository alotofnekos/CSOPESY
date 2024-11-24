#pragma once
#include "process.h"
#include "scheduler.h"
#include <string> 
#include <vector> 

class console
{
private:
    std::vector<process*> *processes = new std::vector<process*>;    
    std::vector<core> *cores; 
    scheduler *consoleScheduler; 
    bool initialized = false; 
public:
    console(/* args */);

    void displayBanner(); 
    void printReport();
    void interpreter(const std::string &command);
    void start();

};

