#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>
#include "process_block.h"

class memory
{
private:
    struct memoryBlock
    {
        int startAddress; 
        int endAddress; 
        std::string proc; 
        time_t timestamp;
    };

    struct frame
    {
        std::string proc;
        std::time_t timestamp;

        frame() : proc(""), timestamp(0) {}
    };
    
    std::vector<memoryBlock> memoryBlocks;
public:
    memory(int max_overall_memory, int memory_per_frame, int memory_per_process);
    int findFit(int size); 
    bool allocateMemory(const std::string &proc, int size);
    void deallocateMemory(const std::string &proc); 
    bool searchProc(const std::string &proc); 
    int getExternalFragmentation();  
    void printMemory(); 
    void generateReport(const std::string &file); 
    std::string removeOldestProcess();
    int getTotalMemoryUsed() const;

    bool allocateFrames(const std::string &proc, int pages);
    void deallocateFrames(const std::string &proc);
    void populateFreeFramesList();
    bool searchProcFrames(const std::string &proc);
    std::string removeOldestProcessFrame(); 
    void generateReportFrames();

    int max_overall_memory;
    int memory_per_frame;
    int memory_per_process; 

    std::vector<frame> frameTable;
    std::vector<int> freeFramesList; 
};