#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <memory>
#include <iomanip> 
#include <queue> 
#include <algorithm>
#include <random>
//#include <pthread>

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

public:
    Process(const std::string& processName, int processId, int numInstructions)
        : name(processName), id(processId), totalInstructions(numInstructions), remainingInstructions(numInstructions), executedInstructions(0) {
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

            // Simulate processing time
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            char timestamp[100];
            std::strftime(timestamp, sizeof(timestamp), "%m/%d/%Y %I:%M:%S%p", std::localtime(&now));

            remainingInstructions--;
            executedInstructions++; // Increment executed instructions
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

    // Get the remaining number of instructions
    int getRemainingInstructions() const {
        std::lock_guard<std::mutex> lock(mtx); 
        return remainingInstructions;
    }

    // Get executed instructions
    int getExecutedInstructions() const {
        std::lock_guard<std::mutex> lock(mtx);
        return executedInstructions;
    }

    // Check if the process has finished
    bool hasFinished() const {
        std::lock_guard<std::mutex> lock(mtx); 
        return remainingInstructions == 0;
    }

    // Get the name of the process
    std::string getName() const {
        return name;
    }

    // Get the timestamp of process execution start
    std::string getStartExecutionTime() const {
        return startExecutionTime;
    }

    // Get the timestamp of process execution end
    std::string getEndExecutionTime() const {
        return endExecutionTime; 
    }
    
	int getTotalInstructions() const {
        return totalInstructions; 
    }
};

class RRScheduler {
private:
    int numCores;
    int quantumTime;  // Time quantum for round-robin scheduling
    std::vector<std::vector<std::shared_ptr<Process>>> processQueues;  // Processes per core
    std::vector<std::shared_ptr<Process>> allProcesses;  // All processes
    std::queue<std::shared_ptr<Process>> finishedProcesses;  // Finished processes
    std::mutex mtx;  // Mutex for thread safety
    int nextProcessIndex = 0;
    int totalProcess = 0;

public:
    RRScheduler(int cores, int quantum) : numCores(cores), quantumTime(quantum), processQueues(cores) {}

    // Add a process to the scheduler
    void addProcess(const std::shared_ptr<Process>& process) {
        static int nextCore = 0;
        std::lock_guard<std::mutex> lock(mtx);
        
        processQueues[nextCore].push_back(process); 
        allProcesses.push_back(process);  
        nextCore = (nextCore + 1) % numCores;  
        totalProcess++;
    }

    // Function for a single core to execute its processes
    void runCore(int core) {
        while (true) {
            std::shared_ptr<Process> currentProcess = nullptr;
            {
                std::lock_guard<std::mutex> lock(mtx);
                // Check if the current core has processes
                if (processQueues[core].empty()) {
                    // If the current core is empty, loop through other cores
                    for (int i = 0; i < numCores; ++i) {
                        int nextCore = (core + i + 1) % numCores;  

                        // If the next core has at least two processes, take the second one
                        if (processQueues[nextCore].size() > 1) {
                            currentProcess = processQueues[nextCore][1]; 
                            processQueues[nextCore].erase(processQueues[nextCore].begin() + 1);
                            break; 
                        }
                    }

                    // If no second processes found after checking all cores, steal the next immediate process
                    if (!currentProcess) {
                        for (int i = 0; i < numCores; ++i) {
                            int nextCore = (core + i + 1) % numCores; 

                            // If the next core has a process, take it
                            if (processQueues[nextCore].size() > 0) {
                                currentProcess = processQueues[nextCore][0]; 
                                processQueues[nextCore].erase(processQueues[nextCore].begin());
                                break;
                            }
                        }
                    }

                    // If no processes found after checking all cores, exit the loop
                    if (!currentProcess) {
                        break;
                    }
                } else {
                    // If the current core is not empty, get the next process
                    currentProcess = processQueues[core].front();  
                    processQueues[core].erase(processQueues[core].begin()); 
                }
            }

            // Execute the process for the given quantum time
            if (currentProcess && !currentProcess->hasFinished()) {
                currentProcess->executeForTimeSlice(quantumTime, core);

                std::lock_guard<std::mutex> lock(mtx);
                processQueues[(core + 1) % numCores].push_back(currentProcess);
            }

            // After execution, check if the process is finished
            if (currentProcess && currentProcess->hasFinished()) {
                std::lock_guard<std::mutex> lock(mtx);
                currentProcess->finalize();  // Finalize the process
            } else {
                // If not finished, re-insert it into the next core's queue for further execution
                std::lock_guard<std::mutex> lock(mtx);
                int nextCore = (core + 1) % numCores;
                processQueues[nextCore].push_back(currentProcess);
            }
        }
    }

    // Run the scheduler across all cores
    void runScheduler() {
        std::vector<std::thread> coreThreads;

        for (int core = 0; core < numCores; ++core) {
            coreThreads.push_back(std::thread([this, core]() { runCore(core); }));  // Start thread for each core
        }

        for (auto& thread : coreThreads) {
            if (thread.joinable()) {
                thread.join();  // Wait for all cores to finish
            }
        }
    }

    // Function to print the status of processes
    void printProcessStatus() {
    std::lock_guard<std::mutex> lock(mtx);
    
    struct ProcessInfo {
        std::string name;
        std::string startTime;
        std::string endTime;
        int coreId;
        int executedInstructions;
        int totalInstructions;
        bool isRunning;
        bool isFinished;
    };
    
    std::vector<ProcessInfo> processSnapshot;

    // Capture current state of processes and core assignments
    for (const auto& process : allProcesses) {
        if (!process) continue;

        ProcessInfo info;
        info.name = process->getName();
        info.executedInstructions = process->getExecutedInstructions();
        info.totalInstructions = process->getTotalInstructions();
        info.isRunning = !process->hasFinished();
        info.isFinished = process->hasFinished();
        info.coreId = -1; // Default to no core assigned

        if (info.isRunning && !process->getStartExecutionTime().empty()) {
            info.startTime = process->getStartExecutionTime();
            
            // Find which core the process is assigned to
            for (int core = 0; core < numCores; ++core) {
                auto it = std::find(processQueues[core].begin(), processQueues[core].end(), process);
                if (it != processQueues[core].end()) {
                    info.coreId = core; // Found the core assigned
                    break;
                }
            }
        } else if (info.isFinished) {
            info.endTime = process->getEndExecutionTime();
        }

        processSnapshot.push_back(info);
    }

    // Print the captured snapshot
    std::cout << "--------------------------------------------------\n";
    std::cout << "Running Processes:\n\n";
    
    for (const auto& info : processSnapshot) {
        if (info.isRunning) {
            std::cout << info.name << " (" << info.startTime << ") "
                      << (info.coreId == -1 ? "Core: " : "Core: " + std::to_string(info.coreId) + " ")
                      << info.executedInstructions << "/" << info.totalInstructions << "\n";
        }
    }

    std::cout << "\nFinished Processes:\n\n";
    for (const auto& info : processSnapshot) {
        if (info.isFinished) {
            std::cout << info.name << " (" << info.endTime << ") "
                      << "Finished " << info.executedInstructions << "/" << info.totalInstructions << "\n";
        }
    }
    
    std::cout << "--------------------------------------------------\n";
}

    // Function to handle command input
    void handleCommands() {
        std::string command;
        while (true) {
            std::cout << "Enter Command: ";
            std::getline(std::cin, command);
            if (command == "screen -ls") {
                printProcessStatus();  // Print immediately when command is entered
            } else if (command == "exit") {
                exit(0);
            }
        }
    }

    // Main function for the scheduler execution and command handling
    void start() {
        std::thread commandThread([&]() { handleCommands(); });  // Command handler runs in a separate thread
        runScheduler();  // This blocks until the scheduler finishes

        if (commandThread.joinable()) {
            commandThread.join();  // Wait for the command handler to finish
        }
    }
};

// Main function 
int main() {
    RRScheduler scheduler(5, 40); 
    std::random_device rd;
    std::mt19937 eng(rd()); 
    std::uniform_int_distribution<> distr(100, 200);

    for (int i = 0; i < 14; ++i) { 
        int numInstructions = distr(eng);
        auto process = std::make_shared<Process>("Process " + std::to_string(i + 1), i + 1, numInstructions);
        scheduler.addProcess(process);  
    }

    scheduler.start();  // Start the scheduler and command handling

    return 0;
}

