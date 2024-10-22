#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <memory>
#include <fstream>
#include <iomanip> 
#include <queue> 
#include <algorithm>

class Process {
private:
    std::string name;
    int id;
    int totalInstructions;
    int remainingInstructions;
    int executedInstructions; 
    std::string startExecutionTime; 
    std::string endExecutionTime; 
    std::ofstream logFile; 
    mutable std::mutex mtx; 

public:
    Process(const std::string& processName, int processId, int numInstructions)
        : name(processName), id(processId), totalInstructions(numInstructions), remainingInstructions(numInstructions), executedInstructions(0) {
        logFile.open("Process_" + std::to_string(processId) + ".txt"); // Open file for logging
        logFile << "Process Name: " << name << "\nLogs:\n\n";
    }

    ~Process() {
        if (logFile.is_open()) {
            logFile.close(); 
        }
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

            logFile << "(" << timestamp << ") Core:" << coreId << " \"Hello world from " << name << "!\"\n"; // Log to file
            logFile.flush(); 
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
    int cycle = 0;
    int nextProcessIndex = 0;
    int coreIndex1 = 0;
    int coreIndex2 = 0;
	int totalProcess=0;
public:
    RRScheduler(int cores, int quantum) : numCores(cores), quantumTime(quantum), processQueues(cores) {}

    // Add a process to the scheduler
	void addProcess(const std::shared_ptr<Process>& process) {
	    static int nextCore = 0;
		  	
	    std::lock_guard<std::mutex> lock(mtx);
	    
	    processQueues[nextCore].push_back(process);
	    
	    allProcesses.push_back(process); 
	    
		nextCore = (nextCore+1) % numCores; 
		totalProcess++;
	}


    // Function for a single core to execute its processes concurrently with Round-Robin scheduling
	void runCore(int core) {
		
        while (true) {
            std::shared_ptr<Process> currentProcess = nullptr;
            {
                std::lock_guard<std::mutex> lock(mtx);  
                if (processQueues[core].empty()) break; 
                currentProcess = processQueues[core].front();  
            }

            if (currentProcess && !currentProcess->hasFinished()) {
                currentProcess->executeForTimeSlice(quantumTime, core);
                cycle++;
            }

            // If the process is finished, remove it from the queue
            if (currentProcess && currentProcess->hasFinished()) {
                {
                    std::lock_guard<std::mutex> lock(mtx);  
                    currentProcess->finalize(); 
                    finishedProcesses.push(currentProcess); 
                    processQueues[core].erase(processQueues[core].begin());  
                }
            }
            else {
			        std::lock_guard<std::mutex> lock(mtx);	
			        
			   		auto currentProcess = allProcesses[nextProcessIndex];
			   		auto core = allProcesses[coreIndex1];
			    
			   		processQueues[coreIndex1].erase(processQueues[coreIndex1].begin()); 
			
			   		coreIndex2 = (coreIndex1+(allProcesses.size()% numCores));
			
					if (coreIndex2 >= numCores){
						coreIndex2 = coreIndex2-numCores;
						}
					
					processQueues[coreIndex2].push_back(currentProcess);
						
					coreIndex1 = (coreIndex1+1) % numCores;
					
					if (nextProcessIndex < (totalProcess-1)){
						nextProcessIndex++; 	
						}
					else {
				 		nextProcessIndex = 0;
					}        
			 }
        }
    }

    // Run the scheduler concurrently across all cores
    void runScheduler() {
        std::vector<std::thread> coreThreads;
        

        for (int core = 0; core < numCores; ++core) {
            coreThreads.push_back(std::thread([this, core]() { runCore(core); }));  // Start thread for each core
        
        }

        for (auto& thread : coreThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    // Function to print the status of processes
    void printProcessStatus() {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "--------------------------------------------------\n";
        std::cout << "Running Processes:\n\n";

        // Iterate through all processes to print Running Processes
        for (const auto& process : allProcesses) {
            if (process && !process->hasFinished()) {
                int coreId = -1;
                for (int core = 0; core < numCores; ++core) {
                    auto it = std::find_if(processQueues[core].begin(), processQueues[core].end(),
                                           [&process](const std::shared_ptr<Process>& p) {
                                               return p.get() == process.get();
                                           });                              

                    if (it != processQueues[core].end()) {
                        coreId = core;
                        break;
                    }
                }

                if (!process->getStartExecutionTime().empty()) {
                    std::cout << process->getName() << " (" 
                              << process->getStartExecutionTime() << ") " 
                              << "Core: " << coreId << " " 
                              << process->getExecutedInstructions() << "/" 
                              << process->getTotalInstructions() << "\n";
                } else {
                    std::cout << process->getName() << " NOT STARTED " 
                              << process->getExecutedInstructions() << "/" 
                              << process->getTotalInstructions() << "\n";
                }
            }
        }

        std::cout << "\nFinished Processes:\n\n";
        for (const auto& process : allProcesses) {
            if (process && process->hasFinished()) {
                std::cout << process->getName() << " (" 
                          << process->getEndExecutionTime() << ") "
                          << "Finished " << process->getExecutedInstructions() << "/" 
                          << process->getRemainingInstructions() + process->getExecutedInstructions() << "\n";
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
                printProcessStatus();
            } else if (command == "exit") {
                exit(0);
            }
        }
    }
};


// Main function 
int main() {
    RRScheduler scheduler(5,20); 
    	for (int i = 0; i < 14; ++i) { 
	    int numInstructions = 100; 
	    auto process = std::make_shared<Process>("Process " + std::to_string(i + 1), i + 1, numInstructions);
	    scheduler.addProcess(process);  
	}

    std::thread commandThread([&scheduler]() { scheduler.handleCommands(); }); 
    scheduler.runScheduler();

    // Join command thread before exiting
    if (commandThread.joinable()) {
        commandThread.join();
    }

    return 0;
}
