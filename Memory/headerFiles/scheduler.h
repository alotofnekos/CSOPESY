#pragma once
#include "config.h"
#include "process.h"
#include "memory.h"
#include <mutex> 
#include <thread> 
#include <condition_variable> 
#include <queue> 
#include <vector> 

struct core {
    int index; 
    std::thread *thread; 
    process *process; 
    bool assigned; 
}; 

class scheduler
{
private:
    std::queue<process*> readyQueue; 
    std::vector<process*> *processes; 
    std::vector<core> cores;
    
    int num_cpu;
    std::string schedulerType;
    int quantum_cycles;
    int batch_process_freq;
    int min_ins;
    int max_ins;
    int delays_per_exec;

    bool generateProcesses = false; 
    std::vector<process*> doneProcesses; 
    std::mutex mtx;
    std::mutex memoryMTX; 
    std::condition_variable cond; 
    memory memory; 

public:
    scheduler(config config, std::vector<process*> *processes);

    void initializeCores();
    void queueProcess(process *process);
    void setGenerateProcesses(bool status);
    void generateProcessesFunc(); 
    void startGenerateProcessesThread();
    void FCFS(int index);
    void RR(int index);
    void generateReport();
    std::vector<core> *getCores();
};
