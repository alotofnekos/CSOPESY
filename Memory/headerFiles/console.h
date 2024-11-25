#pragma once
#include "process_block.h"
#include "scheduler.h"
#include <string> 
#include <vector> 

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
    void interpreter(const std::string &command);
    void start();

};

