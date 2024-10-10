#include <iostream>
#include <string>
#include <cstdlib> // for rand() function
#include <vector>
#include <algorithm>

//Copy pasted from the handouts!
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
    void executeInstruction() {
        if (remainingInstructions > 0) {
            std::cout << "Executing instruction for Process " << id << ": " << name << "\n";
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
};

class FCFSScheduler {
private:
    int numCores;
    std::vector<std::vector<Process>> processQueues; // One queue for each core

public:
    FCFSScheduler(int cores) : numCores(cores), processQueues(cores) {}

    // Add a process to the scheduler
    void addProcess(const Process& process, int core = 0) {
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
                        currentProcess.executeInstruction();
                    }
                    std::cout << "Process " << currentProcess.getRemainingInstructions()
                              << " completed on Core " << core + 1 << ".\n";
                }
            }
        }
    }
};

int main() {
    // Create a sample process
    Process myProcess("Sample Process", 1, 10);

    // Execute instructions until the process finishes
    while (!myProcess.hasFinished()) {
        myProcess.executeInstruction();
    }

    std::cout << "Process has finished!\n";

    return 0;
}
