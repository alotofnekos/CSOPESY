#include <iostream>
#include <string>
#include <cstdlib> 
#include <vector>
#include <chrono>
#include <algorithm>

class Process {
private:
    std::string name;
    int id;
    int totalInstructions;
    int remainingInstructions;

public:
    Process(const std::string& processName, int processId, int numInstructions)
        : name(processName), id(processId), totalInstructions(numInstructions), remainingInstructions(numInstructions) {}

    // Execute one instruction of the process
    void executeInstruction(int coreId) {
        if (remainingInstructions > 0) {
            auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            char timestamp[100];
            std::strftime(timestamp, sizeof(timestamp), "%m/%d/%Y %I:%M:%S%p", std::localtime(&now));
            
            std::cout << "(" << timestamp << ") Core:" << coreId << " \"Hello world from " << name << "!\"\n";
            remainingInstructions--;
        } else {
            std::cout << "Process " << id << ": " << name << " has already finished.\n";
        }
    }

    // Get the remaining number of instructions
    int getRemainingInstructions() const {
        return remainingInstructions;
    }

    // Check if the process has finished
    bool hasFinished() const {
        return remainingInstructions == 0;
    }

    // Get the total number of instructions
    int getTotalInstructions() const {
        return totalInstructions;
    }
};

class FCFSScheduler {
private:
    int numCores;
    std::vector<std::vector<Process>> processQueues; 

public:
    FCFSScheduler(int cores) : numCores(cores), processQueues(cores) {}

    // Add a process to the scheduler
    void addProcess(const Process& process, int core) {
        if (core >= 0 && core < numCores) {
            processQueues[core].push_back(process);
        } else {
            std::cerr << "Invalid core specified for process addition.\n";
        }
    }

    // Sort the process queues based on remaining instructions (FCFS)
    void sortProcessQueues() {
        for (auto& queue : processQueues) {
            std::sort(queue.begin(), queue.end(), [](const Process& a, const Process& b) {
                return a.getRemainingInstructions() > b.getRemainingInstructions();
            });
        }
    }

    // Run the scheduler
    void runScheduler() {
        while (!processQueues[0].empty()) {
            for (int core = 0; core < numCores; ++core) {
                if (!processQueues[core].empty()) {
                    Process currentProcess = processQueues[core].back();
                    processQueues[core].pop_back();
                    while (!currentProcess.hasFinished()) {
                        currentProcess.executeInstruction(core);
                    }
                    std::cout << "Process " << currentProcess.getRemainingInstructions()
                              << " completed on Core " << core + 1 << ".\n";
                }
            }
        }
    }
};

int main() {
    FCFSScheduler scheduler(4); 
    for (int i = 0; i < 10; ++i) { 
        int numInstructions = 100; 
        Process process("Process " + std::to_string(i + 1), i + 1, numInstructions);
        scheduler.addProcess(process, i % 4); 
    }

    scheduler.runScheduler();

    std::cout << "All processes have finished!\n";

    return 0;
}
