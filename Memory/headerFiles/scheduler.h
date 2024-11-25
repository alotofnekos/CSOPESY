#pragma once
#include "config.h"
#include "process_block.h"
#include "memory.h"
#include <mutex> 
#include <thread> 
#include <condition_variable> 
#include <queue> 
#include <vector> 

struct core {
    int index; 
    std::thread *thread; 
    process_block *process_block;
    bool assigned; 
}; 

class scheduler
{
private:
    std::queue<process_block*> readyQueue;
    std::vector<process_block*> *processes;
    std::vector<core> cores;
    
    int num_cpu;
    std::string schedulerType;
    int quantum_cycles;
    int batch_process_freq;
    int min_ins;
    int max_ins;
    int delays_per_exec;
    int min_mem_per_proc;
    int max_mem_per_proc;
    int memory_per_frame;
    int max_overall_memory;

    bool generateProcesses = false; 
    std::vector<process_block*> doneProcesses;
    std::mutex mtx;
    std::mutex memoryMTX; 
    std::condition_variable cond; 
    memory memory; 

public:
    scheduler(config config, std::vector<process_block*> * process_block);

    void initializeCores();
    void queueProcess(process_block *process_block);
    void setGenerateProcesses(bool status);
    void generateProcessesFunc(); 
    void startGenerateProcessesThread();
    int setRandMemorySize();
    void FCFS(int index);
    void RR(int index);
    void generateReport();
    std::vector<core> *getCores();
};
