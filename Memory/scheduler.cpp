#include "headerFiles/scheduler.h"
#include "headerFiles/process_block.h"
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <cmath>

scheduler::scheduler(config config, std::vector<process_block*> *processes) : memory(config.getMaxOverallMemory(), config.getMemoryPerFrame(), config.getMinMemoryPerProcess()) {
    num_cpu = config.getNumCPU(); 
    schedulerType = config.getScheduler();
    quantum_cycles = config.getQuantumCycles();
    batch_process_freq = config.getBatchProcessFreq();
    min_ins = config.getMinIns();
    max_ins = config.getMaxIns();
    delays_per_exec = config.getDelaysPerExec();
    min_mem_per_proc = config.getMinMemoryPerProcess();
    max_mem_per_proc = config.getMaxMemoryPerProcess();
    memory_per_frame = config.getMemoryPerFrame();
    max_overall_memory = config.getMaxOverallMemory();
    this->processes = processes;

    cores.resize(num_cpu);
    for (int i = 0; i < num_cpu; i++)
    {
        cores[i].index = i;
        cores[i].thread = nullptr;
        cores[i].process_block = nullptr;
        cores[i].assigned = false; 
    }
}

void scheduler::initializeCores() {
    if (schedulerType == "fcfs")
    {
        for (int i = 0; i < num_cpu; i++)
        {
            cores[i].thread = new std::thread(&scheduler::FCFS, this, i);
            cores[i].thread->detach();
        }
    } else if (schedulerType == "rr")
    {
        for (int i = 0; i < num_cpu; i++)
        {
            cores[i].thread = new std::thread(&scheduler::RR, this, i);
            cores[i].thread->detach();
        }
    }
}

void scheduler::queueProcess(process_block *process_block) {
    {
        std::lock_guard<std::mutex> lock(mtx);
        readyQueue.push(process_block);
    }
    cond.notify_one();
}

void scheduler::setGenerateProcesses(bool status) {
    generateProcesses = status; 
}

int scheduler::setRandMemorySize() {
    int memory;
    int lowerBound = ceil(log2(min_mem_per_proc));  
    int upperBound = floor(log2(max_mem_per_proc)); 

    // Generate all powers of 2 in the range [min, max]
    std::vector<int> powersOfTwo;
    for (int i = lowerBound; i <= upperBound; ++i) {
        powersOfTwo.push_back(1 << i); // 1 << i is the same as 2^i
    }
    if (powersOfTwo.empty()) {
        std::cerr << "No power of 2 found in range." << std::endl;
        return -1; // Handle the case where no valid power of 2 is found
    }

    // Randomly select a power of 2
    int randomIndex = rand() % powersOfTwo.size();
    return powersOfTwo[randomIndex];
}

void scheduler::generateProcessesFunc() {
    int counter = 0;

    while (generateProcesses)
    {
        auto *proc = new process_block("Process_" + std::to_string(counter));
        counter++; 
        proc->setTotalInstructions(rand() % (max_ins - min_ins + 1) + min_ins);
        proc->setWaiting(true); 
        proc->setMemorySize(setRandMemorySize());
        {
            std::lock_guard<std::mutex> lock(mtx);
            processes->push_back(proc);
        }

        queueProcess(proc);

        std::this_thread::sleep_for(std::chrono::milliseconds(batch_process_freq));
    }
}

void scheduler::startGenerateProcessesThread() {
    generateProcesses = true; 
    std::thread generatorThread(&scheduler::generateProcessesFunc, this);
    generatorThread.detach();
}

void scheduler::FCFS(int index) {

}

void scheduler::RR(int index) {
    process_block *proc = nullptr;

    while (true)
    {
        if (proc == nullptr)
        {
            std::unique_lock<std::mutex> lock(mtx);
            if (readyQueue.empty()) {
                cores[index].idleTicks++; 
            }

            cond.wait(lock, [this] { return !readyQueue.empty(); });

            proc = readyQueue.front();
            if (proc == nullptr)
            {
                continue; 
            }
            readyQueue.pop();
        }
        
        {
            std::lock_guard<std::mutex> lock(memoryMTX);
            if (!memory.searchProc(proc->getName()) && !memory.allocateMemory(proc->getName(), memory.memory_per_process)) 
            {
                queueProcess(proc);
                proc = nullptr;
                continue;
            }
            
        }

        cores[index].process_block = proc; 
        cores[index].assigned = true; 

        proc->setCore(index);
        proc->setRunning(true);
        proc->setWaiting(false); 

        int unexecutedInstructions = proc->getTotalInstructions() - proc->getExecutedInstructions();
        int quantum = std::min(quantum_cycles, unexecutedInstructions); 

        if(proc->startTime == 0) {
            proc->startTime = std::time(nullptr);
        }

        for (int i = 0; i < quantum; i++)
        {
            proc->setExecutedInstructions(proc->getExecutedInstructions() + 1);
            cores[index].activeTicks++;
            cores[index].idleTicks= cores[index].idleTicks+delays_per_exec-1;
            std::this_thread::sleep_for(std::chrono::milliseconds(delays_per_exec * 500));
        }
        
        if (proc->getExecutedInstructions() == proc->getTotalInstructions())
        {
            proc->endTime = std::time(nullptr);
            proc->setRunning(false);
            proc->setDone(true);
            cores[index].process_block = nullptr;
            cores[index].assigned = false;
            memory.deallocateMemory(proc->getName()); 
            doneProcesses.push_back(proc);
            proc = nullptr;
        } 
        else
        {
            if (!readyQueue.empty())
            {
                proc->setRunning(false);
                proc->setWaiting(true);
                cores[index].process_block = nullptr;
                cores[index].assigned = false;
                queueProcess(proc); 
                proc = nullptr; 
            }   
        }
    }
}

void scheduler::generateReport() {
    int round = 0;
    while (generateProcesses)
    {
        std::stringstream format;
        format << "memory_stamp_" << round << ".txt";
        {
            std::lock_guard<std::mutex> lock(memoryMTX);
            memory.generateReport(format.str());
        }
        round += quantum_cycles;
        std::this_thread::sleep_for(std::chrono::milliseconds(delays_per_exec + 1 * 500));
    }
}

void scheduler::generateReportThread() {
    std::thread reportThread(&scheduler::generateReport, this);
    reportThread.detach();
}

std::vector<core> *scheduler::getCores() {
    return &cores; 
}

int scheduler::getMaxOverallMemory() {
    return max_overall_memory; 
}
