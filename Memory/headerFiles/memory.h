#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <ctime>

class memory
{
private:
    struct memoryBlock
    {
        int startAddress; 
        int endAddress; 
        std::string proc; 
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
    int getTotalMemoryUsed() const;

    int max_overall_memory;
    int memory_per_frame;
    int memory_per_process; 
};