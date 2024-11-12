#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <mutex>
#include <chrono>
#include <thread>
#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>

const int mem_per_proc = 1024; // temp var, will be replaced by config file 
const int quantumTime = 5; // temp var, will be replaced by config file 

struct MemoryBlock {
    int startAddress;
    int size;

    MemoryBlock(int start, int sz) : startAddress(start), size(sz) {}
};

class Process;

class MemoryAllocator {
private:
    std::vector<MemoryBlock> freeBlocks;
    std::mutex mtx;
    std::queue<Process*> readyQueue;

public:
    MemoryAllocator(int totalMemory) {
        freeBlocks.emplace_back(0, totalMemory);
    }

    int allocate(int size) {
        std::lock_guard<std::mutex> lock(mtx);
        for (auto it = freeBlocks.begin(); it != freeBlocks.end(); ++it) {
            if (it->size >= size) {
                int startAddress = it->startAddress;
                it->startAddress += size;
                it->size -= size;
                if (it->size == 0) {
                    freeBlocks.erase(it);
                }
                return startAddress;
            }
        }
        throw std::runtime_error("Not enough memory to allocate");
    }

    void deallocate(int startAddress, int size) {
        std::lock_guard<std::mutex> lock(mtx);
        freeBlocks.emplace_back(startAddress, size);
        std::sort(freeBlocks.begin(), freeBlocks.end(), [](const MemoryBlock& a, const MemoryBlock& b) {
            return a.startAddress < b.startAddress;
        });
        mergeFreeBlocks();
    }

    void addToReadyQueue(Process* process) {
        std::lock_guard<std::mutex> lock(mtx);
        readyQueue.push(process);
    }

    Process* getNextProcessFromReadyQueue() {
        std::lock_guard<std::mutex> lock(mtx);
        if (readyQueue.empty()) {
            return nullptr;
        }
        Process* process = readyQueue.front();
        readyQueue.pop();
        return process;
    }

    int getNumberOfProcessesInMemory() {
        std::lock_guard<std::mutex> lock(mtx);
        return readyQueue.size();
    }

    int getTotalExternalFragmentation() {
        std::lock_guard<std::mutex> lock(mtx);
        int fragmentation = 0;
        for (const auto& block : freeBlocks) {
            fragmentation += block.size;
        }
        return fragmentation;
    }

    std::string getMemoryASCII() {
        std::lock_guard<std::mutex> lock(mtx);
        std::string memoryASCII;
        int totalMemory = 0;
        for (const auto& block : freeBlocks) {
            totalMemory += block.size;
        }
        for (const auto& block : freeBlocks) {
            memoryASCII += std::string(block.size, 'F');
        }
        return memoryASCII;
    }

private:
    void mergeFreeBlocks() {
        for (auto it = freeBlocks.begin(); it != freeBlocks.end() - 1; ++it) {
            auto next = it + 1;
            if (it->startAddress + it->size == next->startAddress) {
                it->size += next->size;
                freeBlocks.erase(next);
                --it;
            }
        }
    }
};

class Process {
private:
    std::string name;
    int id;
    int totalInstructions;
    int remainingInstructions;
    int executedInstructions;
    std::string startExecutionTime;
    std::string endExecutionTime;
    mutable std::mutex mtx;
    int lastKnownCore = -1;
    int memoryStartAddress = -1;
    int memorySize = 0;
    MemoryAllocator& allocator;

public:
    Process(const std::string& processName, int processId, int numInstructions, MemoryAllocator& memAllocator)
        : name(processName), id(processId), totalInstructions(numInstructions), remainingInstructions(numInstructions), executedInstructions(0), memorySize(mem_per_proc), allocator(memAllocator) {
        try {
            memoryStartAddress = allocator.allocate(memorySize);
        } catch (const std::runtime_error& e) {
            // Handle memory full condition
            allocator.addToReadyQueue(this);
        }
    }

    ~Process() {
        allocator.deallocate(memoryStartAddress, memorySize);
    }

    // Execute one instruction of the process
    void executeInstruction(int coreId) {
        std::lock_guard<std::mutex> lock(mtx);
        if (remainingInstructions > 0) {
            // Set the start execution time if it hasn't been set yet
            if (startExecutionTime.empty()) {
                auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                char buffer[100];
                std::strftime(buffer, sizeof(buffer), "%m/%d/%Y %I:%M:%S%p", std::localtime(&now));
                startExecutionTime = buffer; // Store the execution start time
            }
            // CPU Cycle
            std::this_thread::sleep_for(std::chrono::milliseconds(quantumTime));
            remainingInstructions--;
            executedInstructions++;
        }
    }

    void executeForTimeSlice(int quantumTime, int coreId) {
        int remainingTime = std::min(quantumTime, getRemainingInstructions());

        for (int i = 0; i < remainingTime; ++i) {
            executeInstruction(coreId);
        }

        if (hasFinished()) {
            finalize();
        }
    }

    // Finalize the process, set end time
    void finalize() {
        std::lock_guard<std::mutex> lock(mtx);
        if (remainingInstructions == 0 && endExecutionTime.empty()) {
            auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            char buffer[100];
            std::strftime(buffer, sizeof(buffer), "%m/%d/%Y %I:%M:%S%p", std::localtime(&now));
            endExecutionTime = buffer; // Store the execution end time
        }
    }

    bool hasFinished() const {
        return remainingInstructions == 0;
    }

    int getRemainingInstructions() const {
        return remainingInstructions;
    }

    // Get the timestamp of process execution start
    std::string getStartExecutionTime() const {
        return startExecutionTime;
    }

    // Get the timestamp of process execution end
    std::string getEndExecutionTime() const {
        return endExecutionTime;
    }
};

class Scheduler {
private:
    int numCores;
    std::vector<std::vector<Process*>> processQueues;
    std::vector<std::thread> coreThreads;
    std::queue<Process*> finishedProcesses;
    MemoryAllocator& allocator;
    std::mutex mtx;

public:
    Scheduler(int cores, MemoryAllocator& memAllocator) : numCores(cores), allocator(memAllocator) {
        processQueues.resize(numCores);
    }

    void addProcess(Process* process) {
        std::lock_guard<std::mutex> lock(mtx);
        processQueues[0].push_back(process);
    }

    void runCore(int core) {
        while (true) {
            Process* currentProcess = nullptr;
            {
                std::lock_guard<std::mutex> lock(mtx);
                if (!processQueues[core].empty()) {
                    currentProcess = processQueues[core].front();
                    processQueues[core].erase(processQueues[core].begin());
                } else {
                    currentProcess = allocator.getNextProcessFromReadyQueue();
                }
            }

            if (!currentProcess) {
                std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Sleep a CPU cycle so that it never closes
                continue;
            }

            // Execute the process for the given quantum time
            if (currentProcess && !currentProcess->hasFinished()) {
                currentProcess->executeForTimeSlice(quantumTime, core);
            }

            // After execution, check if the process is finished
            if (currentProcess && currentProcess->hasFinished()) {
                std::lock_guard<std::mutex> lock(mtx);
                currentProcess->finalize();  // Finalize the process
                finishedProcesses.push(currentProcess);  // Add to finished queue
            } else {
                // If not finished, re-insert it into the next core's queue for further execution
                std::lock_guard<std::mutex> lock(mtx);
             	                  
                int nextCore  = (core+(allProcesses.size() % numCores));
							
				if (nextCore >= numCores){
						nextCore = nextCore-numCores;
				}
					
            	processQueues[nextCore].push_back(currentProcess);
            }

            // Produce a text file with the required information
            produceReport();
        }
    }

    void runScheduler() {
        for (int core = 0; core < numCores; ++core) {
            coreThreads.push_back(std::thread([this, core]() { runCore(core); }));
        }

        for (auto& thread : coreThreads) {
            thread.join();
        }
    }

    void produceReport() {
        std::lock_guard<std::mutex> lock(mtx);
        std::ofstream reportFile("report.txt", std::ios::app);
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        char buffer[100];
        std::strftime(buffer, sizeof(buffer), "%m/%d/%Y %I:%M:%S%p", std::localtime(&now));
        reportFile << "Timestamp: " << buffer << "\n";
        reportFile << "Number of processes in memory: " << allocator.getNumberOfProcessesInMemory() << "\n";
        reportFile << "Total external fragmentation in KB: " << allocator.getTotalExternalFragmentation() << "\n";
        reportFile.close();
    }
};

int main() {
    int totalMemory = 10240; // temp var, will be replaced by config file 
    MemoryAllocator allocator(totalMemory);
    Scheduler scheduler(4, allocator); // 4 cores

    // Create and add processes to the scheduler
    for (int i = 0; i < 10; ++i) {
        Process* process = new Process("Process" + std::to_string(i), i, 20, allocator);
        scheduler.addProcess(process);
    }

    // Run the scheduler
    scheduler.runScheduler();

    return 0;
}
