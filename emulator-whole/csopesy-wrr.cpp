#include <iostream>
#include <cstdlib>
#include <conio.h>
#include <stdio.h>
#include <ctime>
#include <string>   
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <mutex>
#include <memory>
#include <queue> 
#include <algorithm>
#include <random>
#include <atomic>

int num_cpu;
std::string scheduler;
int quantum_cycles;
int batch_process_freq;
int min_ins;
int max_ins;
int delays_per_exec;
int processCount=0;

std::atomic<bool> schedulerRunning(true);
std::atomic<bool> processGenerationActive(false); 

void banner(){
	std::cout <<"\n";
    std::cout << "  /$$$$$$  /$$$$$$   /$$$$$$  /$$$$$$$  /$$$$$$$$  /$$$$$$  /$$     /$$           \n";
    std::cout << " /$$__  $$/$$__  $$ /$$__  $$| $$__  $$| $$_____/ /$$__  $$|  $$   /$$/     _                ___       _.--.          \n";
    std::cout << "| $$  \\__/ $$  \\__/| $$  \\ $$| $$  \\ $$| $$      | $$  \\__/ \\  $$ /$$/      \\`.|\\..----...-'`   `-._.-'_.-'`          \n";
    std::cout << "| $$     |  $$$$$$ | $$  | $$| $$$$$$$/| $$$$$   |  $$$$$$   \\  $$$$/       /  ' `         ,       __.--'            \n";
    std::cout << "| $$      \\____  $$| $$  | $$| $$____/ | $$__/    \\____  $$   \\  $$/       )/' _/     \\   `-_,   /                 \n";
    std::cout << "| $$    $$/$$  \\ $$| $$  | $$| $$      | $$       /$$  \\ $$    | $$        `-\"`\"\\_  ,_.-;_.-  \\_ ',                \n";
    std::cout << "|  $$$$$$/  $$$$$$/|  $$$$$$/| $$      | $$$$$$$$|  $$$$$$/    | $$            _.-'_./   {_.'   ; /               \n";
    std::cout << " \\______/ \\______/  \\______/ |__/      |________/ \\______/     |__/           {_.-``-'         {_/               \n";
	std::cout << "type 'exit' to quit, or 'clear' to clear the screen\n";
}

void non_blocking()
{
    char ch;

    std::cout << "Press keys to display the character. Press 'q' to quit.\n";
    while ((ch = _getch()) != 'q') {
        std::cout << "Key pressed: " << ch << std::endl;
    }
	std::cout << "q pressed, exiting non-blocking mode... "<< std::endl; 
}

std::string timestamp() {
    time_t timestamp;
    time(&timestamp);
    struct tm *localTime = localtime(&timestamp);
    char formattedTime[100];

    strftime(formattedTime, sizeof(formattedTime), "%m/%d/%Y, %I:%M:%S %p", localTime);

    return std::string(formattedTime);  
}

void displayConfig() {
    std::cout << "\nLoaded Configuration:" << std::endl;
    std::cout << "num-cpu: " << num_cpu << std::endl;
    std::cout << "scheduler: " << scheduler << std::endl;
	std::cout << "quantum-cycles: " << quantum_cycles << std::endl;
    std::cout << "batch-process-freq: " << batch_process_freq << std::endl;
    std::cout << "min-ins: " << min_ins << std::endl;
    std::cout << "max-ins: " << max_ins << std::endl;
    std::cout << "delays-per-exec: " << delays_per_exec << std::endl;
    std::cout << std::endl;
}

bool initializeConfig() {
    std::ifstream configFile("config.txt");
    if (!configFile) {
        std::cerr << "Error: Could not open config.txt" << std::endl;
        return false;
    }
    std::string line;
    while (std::getline(configFile, line)) {
        std::istringstream iss(line);
        std::string param;
        iss >> param;

        if (param == "num-cpu") {
            iss >> num_cpu;
            if (num_cpu < 1 || num_cpu > 128) {
                std::cerr << "Error: num-cpu must be between 1 and 128" << std::endl;
                return false;
            }
        } else if (param == "scheduler") {
            iss >> scheduler;
            if (scheduler != "fcfs" && scheduler != "rr") {
                std::cerr << "Error: Invalid scheduler. Must be 'fcfs' or 'rr'" << std::endl;
                return false;
            }
        } else if (param == "quantum-cycles") {
            iss >> quantum_cycles;
            if (quantum_cycles < 1 || quantum_cycles > 4294967296) {
                std::cerr << "Error: quantum-cycles must be between 1 and 2^32 (4294967296)" << std::endl;
                return false;
            }
        } else if (param == "batch-process-freq") {
            iss >> batch_process_freq;
            if (batch_process_freq < 1 || batch_process_freq > 4294967296) {
                std::cerr << "Error: batch-process-freq must be between 1 and 2^32 (4294967296)" << std::endl;
                return false;
            }
        } else if (param == "min-ins") {
            iss >> min_ins;
            if (min_ins < 1 || min_ins > 4294967296) {
                std::cerr << "Error: min-ins must be between 1 and 2^32 (4294967296)" << std::endl;
                return false;
            }
        } else if (param == "max-ins") {
            iss >> max_ins;
            if (max_ins < 1 || max_ins > 4294967296) {
                std::cerr << "Error: max-ins must be between 1 and 2^32 (4294967296)" << std::endl;
                return false;
            }
        } else if (param == "delays-per-exec") {
            iss >> delays_per_exec;
            if (delays_per_exec < 0 || delays_per_exec > 4294967296) {
                std::cerr << "Error: delays-per-exec must be between 0 and 2^32 (4294967296)" << std::endl;
                return false;
            }
        } else {
            std::cerr << "Error: Unknown parameter in config file" << std::endl;
            return false;
        }
    }

    configFile.close();
    return true;
}

class Screen {
private:
    std::string borderStyle;  
    std::string name;         
    std::string creationDate; 

public:
    Screen(const std::string &borderStyle, const std::string &name)
        : borderStyle(borderStyle), name(name), creationDate(timestamp()) {}

    void create() const {
        std::string content = " Screen Name: " + name + " \n Date Created: " + creationDate;
        size_t width = std::max(static_cast<size_t>(content.length()), static_cast<size_t>(2));
        //borders
        std::cout << std::string(width, borderStyle[0]) << std::endl;
        std::cout << content << std::endl;
        std::cout << std::string(width, borderStyle[0]) << std::endl;
    }
    std::string getName() const {
        return name;
    }
};

class ScreenManager {
private:
    std::vector<Screen> screens; 
    std::string getBorderStyle(size_t index) const {
        switch (index % 5) { 
            case 0: return "#";
            case 1: return "*";
            case 2: return "~";
            case 3: return "x";
            case 4: return "+";
            default: return "#"; 
        }
    }

public:
    
    void addScreen(const std::string &name) {
        std::string borderStyle = getBorderStyle(screens.size()); 
        Screen newScreen(borderStyle, name); 
        screens.push_back(newScreen); 
    }

    void callScreen(const std::string &name) const {
        for (size_t i = 0; i < screens.size(); ++i){ 
	            if (screens[i].getName() == name) { 
                screens[i].create(); 
                return;
            }
        }
        std::cout << "Screen named \"" << name << "\" not found." << std::endl; 
    }
    
    bool screenExists(const std::string &name) const {
        for (size_t i = 0; i < screens.size(); ++i) {
            if (screens[i].getName() == name) {
                return true; 
            }
        }
        return false;
    }
    
    
};

//Process class for use of FCFS and RR
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
};

bool initialize_configs(std::string command){
	bool initialized =false;
	if (command == "initialize") {
        if (initializeConfig()) {
            std::cout << "Initialization successful!" << std::endl;
            displayConfig();  
            // Random number generator setup
    		//std::random_device rd;  // Obtain a random number from hardware
    		//std::mt19937 eng(rd()); // Seed the generator
    		//std::uniform_int_distribution<> distr(min_ins, max_ins); // Define the range
            //RRScheduler scheduler(num_cpu,quantum_cycles); 
	    	//for (int i = 0; i < 10; ++i) { 
		    //	int numInstructions = distr(eng);
			//	auto process = std::make_shared<Process>("Process " + std::to_string(i + 1), i + 1, numInstructions);
			//   scheduler.addProcess(process);  
			//}
			//std::thread commandThread([&scheduler]() { scheduler.handleCommands(); }); 
    		//scheduler.runScheduler();
            initialized = true;
        } else {
            std::cerr << "Initialization failed!" << std::endl;
    	}
	}
    else{
    	std::cerr << "Type initialize to setup the parameters for this emulator" << std::endl;
	}
	return initialized;
}

void generateProcesses(RRScheduler& scheduler, int frequency) {
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(min_ins, max_ins);

    int processCount = 1;
    while (processGenerationActive) {
        std::this_thread::sleep_for(std::chrono::milliseconds(frequency));
        int numInstructions = distr(eng);
        auto process = std::make_shared<Process>("Process " + std::to_string(processCount), processCount, numInstructions);
        scheduler.addProcess(process);
        processCount++;

        std::cout << "Generated new process: " << process->getName() << " with " << numInstructions << " instructions.\n";
    }
}


int interpreter(std::string command, ScreenManager& manager,auto& scheduler,std::thread& processGenerationThread){
	
	int isExit = 0;
    if (command == "clear") {
        system("cls"); 
        banner();
    } else if (command == "initialize") {
        if (initializeConfig()) {
            std::cout << "Initialization successful!" << std::endl;
            displayConfig();  
        } else {
            std::cerr << "Initialization failed!" << std::endl;
        }
    } else if (command == "screen -ls") {
       		scheduler.printProcessStatus();
    } else if (command.rfind("screen -s", 0) == 0) {
        std::istringstream iss(command);
        std::string cmd, dash_s, screenName;
        iss >> cmd >> dash_s >> screenName;
        if (!screenName.empty()) {
            if (manager.screenExists(screenName)) {
                std::cout << "Screen already exists. Opening " << screenName << std::endl;
                manager.callScreen(screenName);
            } else {
                std::cout << "Creating new screen: " << screenName << std::endl;
                manager.addScreen(screenName);
                manager.callScreen(screenName);
            }
        } else {
            std::cout << "Error: No screen name provided!" << std::endl;
        }
    } else if (command.rfind("screen", 0) == 0) {
    	std::cout << "Syntax: screen -s <name>" << std::endl;
	} else if (command == "scheduler-test") {
        std::cout << "Scheduler test started.\n";
        if (!processGenerationActive) {
            processGenerationActive = true;
            processGenerationThread = std::thread(generateProcesses, std::ref(scheduler), batch_process_freq);
        }
    } else if (command == "scheduler-stop") {
        std::cout << "Stopping scheduler test.\n";

        // Stop the process generation
        processGenerationActive = false;
        if (processGenerationThread.joinable()) {
            processGenerationThread.join();
        }
    } else if (command == "report-util") {
        std::cout << command << " command recognized. Doing something." << std::endl;
    } else if (command == "non-blocking") {
        std::cout << command << " command recognized. Doing something." << std::endl;
        non_blocking();
    } else if (command == "exit") {
        isExit=1;
        schedulerRunning = false;
        processGenerationActive = false;
    } else {
        std::cout << command << " is not recognized!" << std::endl;
    }
	return isExit;
}

void runScheduler(RRScheduler& scheduler) {
    // While the schedulerRunning flag is true, run the scheduler
    while (schedulerRunning) {
        scheduler.runScheduler();  
    }
}

void runInterpreter(RRScheduler& scheduler, ScreenManager& manager,std::thread& processGenerationThread) {
    std::string command;
    while (true) {
        std::getline(std::cin, command);
        if (interpreter(command, manager, scheduler,processGenerationThread)) {
            break;
        }
    }
}

int main() {
    banner(); 
    
    int isExit = 0;
    ScreenManager manager; 
    std::string command;
    bool initialized = false;

    // Initialize configurations before starting the scheduler and interpreter
    do {
        std::cout << "Enter a command: ";
        std::getline(std::cin, command);
        initialized = initialize_configs(command);  
    } while (!initialized);

    // After initialization, create the scheduler
    RRScheduler scheduler(num_cpu, quantum_cycles);
    // And the thread for process generation
	std::thread processGenerationThread;
    // Start the scheduler in a separate thread
    std::thread schedulerThread([&scheduler]() {
        while (schedulerRunning) {
            scheduler.runScheduler();
        }
    });
    do {
        std::cout << "Enter a command: ";
        std::getline(std::cin, command);
        isExit = interpreter(command, manager, scheduler,processGenerationThread);
        if (isExit == 1) {
            schedulerRunning = false;
        }

    } while (isExit != 1);


    if (schedulerThread.joinable()) {
        schedulerThread.join();
    }

    return 0; 
}


