#pragma once 
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

class config
{
private:
    int num_cpu;
    std::string scheduler;
    int quantum_cycles;
    int batch_process_freq;
    int min_ins;
    int max_ins;
    int delays_per_exec;
    int max_overall_memory;
    int memory_per_frame;
    int memory_per_process;

public:
    config();
    bool initializeConfig(); 
    void displayConfig() const;

    int getNumCPU() const;
    std::string getScheduler() const;
    int getQuantumCycles() const;
    int getBatchProcessFreq() const;
    int getMinIns() const;
    int getMaxIns() const;
    int getDelaysPerExec() const;
    int getMaxOverallMemory() const;
    int getMemoryPerFrame() const;
    int getMemoryPerProcess() const;
};


