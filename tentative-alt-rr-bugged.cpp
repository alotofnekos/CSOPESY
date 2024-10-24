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
	
	                    // If the next core has a processes, take it
		                    if (processQueues[nextCore].size() > 0) {
		                        currentProcess = processQueues[nextCore][0]; 
		                        processQueues[nextCore].erase(processQueues[nextCore].begin());
		                        break;
		                    }
	                	}
	                }
	                
	                // If no processes found after checking all cores, exit the loop
	                if(!currentProcess){
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
            }

            // After execution, check if the process is finished
            if (currentProcess && currentProcess->hasFinished()) {
                std::lock_guard<std::mutex> lock(mtx);
            //    processQueues[core].erase(processQueues[core].begin());
                currentProcess->finalize();  // Finalize the process
             //   finishedProcesses.push(currentProcess);  // Add to finished queue
                
                
            } else {
                // If not finished, re-insert it into the next core's queue for further execution
                std::lock_guard<std::mutex> lock(mtx);
                              
                int nextCore  = (core+(allProcesses.size() % numCores));
							
				if (nextCore >= numCores){
						nextCore = nextCore-numCores;
				}
					
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
    RRScheduler scheduler(5,40); 
    	std::random_device rd;  // Obtain a random number from hardware
    	std::mt19937 eng(rd()); // Seed the generator
    	std::uniform_int_distribution<> distr(100, 200);
    	for (int i = 0; i < 14; ++i) { 
	    int numInstructions = distr(eng);
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
