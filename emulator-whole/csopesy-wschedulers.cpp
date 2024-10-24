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
std::string schedulerType;
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
    std::cout << "scheduler: " << schedulerType << std::endl;
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
            iss >> schedulerType;
            if (schedulerType != "fcfs" && schedulerType != "rr") {
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

bool initialize_configs(std::string command){
	bool initialized =false;
	if (command == "initialize") {
        if (initializeConfig()) {
            std::cout << "Initialization successful!" << std::endl;
            displayConfig();  
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
    mutable std::mutex mtx; 

public:
    Process(const std::string& processName, int processId, int numInstructions)
        : name(processName), id(processId), totalInstructions(numInstructions), remainingInstructions(numInstructions), executedInstructions(0) {
    }

    ~Process() {
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
            
            if (delays_per_exec == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            } else {
                std::this_thread::sleep_for(std::chrono::milliseconds(100 * delays_per_exec));
            }
            
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
    
    // Gets Id
    int getId() const {
        return id;
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

class Scheduler {
public:
    virtual ~Scheduler() = default;
    virtual void addProcess(const std::shared_ptr<Process>& process) = 0;
    virtual void runScheduler() = 0;
	virtual void printProcessStatus(const bool toFile) =0;
    virtual Process* findProcessByName(const std::string& name) = 0;
};

class FCFSScheduler : public Scheduler {
private:
    int numCores;
    std::vector<std::vector<std::shared_ptr<Process>>> processQueues;  
    std::vector<std::shared_ptr<Process>> allProcesses; 
    std::queue<std::shared_ptr<Process>> finishedProcesses; 
    std::mutex mtx;  

public:
    FCFSScheduler(int cores) : numCores(cores), processQueues(cores) {}

    // Add a process to the scheduler
	void addProcess(const std::shared_ptr<Process>& process) {
           	std::lock_guard<std::mutex> lock(mtx); 
           	allProcesses.push_back(process); 
			//int core = (allProcesses.size()-1)%numCores; 
            //processQueues[core].push_back(process); 
            int minCore = 0; 
		    size_t minQueueSize = processQueues[0].size();  
		
		    for (int i = 1; i < numCores; ++i) {
		        if (processQueues[i].size() < minQueueSize) {
		            minCore = i;  
		            minQueueSize = processQueues[i].size();  
		        }
		    }
		
		    processQueues[minCore].push_back(process);
    }


    // Function for a single core to execute its processes concurrently
    void runCore(int core) {
        while (true) {
            std::shared_ptr<Process> currentProcess = nullptr;
            {
                std::lock_guard<std::mutex> lock(mtx);  
                if (processQueues[core].empty()) {
	                std::this_thread::sleep_for(std::chrono::milliseconds(100)); //sleep a CPU cycle so that it never closes
	                continue; 
            	}
                currentProcess = processQueues[core].front();  
            }

            if (currentProcess && !currentProcess->hasFinished()) {
                currentProcess->executeInstruction(core);  
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
        }
    }

    // Run the scheduler concurrently across all cores
    void runScheduler() override{
        std::vector<std::thread> coreThreads;

        for (int core = 0; core < numCores; ++core) {
            coreThreads.push_back(std::thread([this, core]() { runCore(core); }));  
        }

        for (auto& thread : coreThreads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
    
	void printProcessStatus(bool toFile) override{
	    std::lock_guard<std::mutex> lock(mtx);
	    
	    std::ofstream logFile;
	    if (toFile) {
	        logFile.open("csopesylog.txt", std::ios::app);  // Open in append mode
	        if (!logFile.is_open()) {
	            std::cerr << "Error opening csopesylog.txt for logging." << std::endl;
	            return;
	        }
	    }
	    std::ostream& out = toFile ? logFile : std::cout;  // Use logFile or std::cout based on the flag
	    // Calculate CPU utilization as a percentage
		int coresUsed = 0;
		std::vector<bool> coreOccupied(numCores, false);  
		

		for (const auto& process : allProcesses) {
		    if (process && !process->hasFinished() && !process->getStartExecutionTime().empty()) { 
		        for (int core = 0; core < numCores; ++core) {
		            if (std::find(processQueues[core].begin(), processQueues[core].end(), process) != processQueues[core].end()) {
		                if (!coreOccupied[core]) {  
		                    coreOccupied[core] = true;
		                    coresUsed++;
		                }
		                break; 
		            }
		        }
		    }
}
		
		double cpuUtilization = (static_cast<double>(coresUsed) / numCores) * 100;  
	    // Log CPU utilization and core usage
	    out << "CPU Utilization: " << cpuUtilization << "%\n";
	    out << "Cores Used: " << coresUsed << "/" << numCores << "\n";
	    out << "Free Cores: " << numCores - coresUsed << "\n";
	    out << "--------------------------------------------------\n";
	    out << "Running Processes:\n\n";
	
	    // Iterate through all processes to print Running Processes
	    for (const auto& process : allProcesses) {
	        if (process && !process->hasFinished()) {
	            int coreId = -1;
	            for (int core = 0; core < numCores; ++core) {
	                if (std::find(processQueues[core].begin(), processQueues[core].end(), process) != processQueues[core].end()) {
	                    coreId = core; 
	                    break;
	                }
	            }
	
	            if (!process->getStartExecutionTime().empty()) {
	                out << process->getName() << " (" 
	                    << process->getStartExecutionTime() << ") " 
	                    << "Core: " << coreId << " " 
	                    << process->getExecutedInstructions() << "/" 
	                    << process->getTotalInstructions() << "\n";
	            } else {
	                out << process->getName() << " NOT STARTED " 
	                    << process->getExecutedInstructions() << "/" 
	                    << process->getTotalInstructions() << "\n";
	            }
	        }
	    }
	
	    out << "\nFinished Processes:\n\n";
	    for (const auto& process : allProcesses) {
	        if (process && process->hasFinished()) {
	            out << process->getName() << " (" 
	                << process->getEndExecutionTime() << ") "
	                << "Finished " << process->getExecutedInstructions() << "/" 
	                << process->getTotalInstructions() << "\n";
	        }
	    }
	
	    out << "--------------------------------------------------\n";
	
	    if (toFile) {
	        logFile.close(); 
	    }
	}
    
    Process* findProcessByName(const std::string& name) override{
        for (const auto& process : allProcesses) { 
            if (process->getName() == name) {
                return process.get();
            }
        }
        return nullptr; // Not found
    }

};

class RRScheduler : public Scheduler{
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
                if (processQueues[core].empty()) {
	                std::this_thread::sleep_for(std::chrono::milliseconds(100)); //sleep a CPU cycle so that it never closes
	                continue; 
            	} 
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
    void runScheduler() override{
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
	void printProcessStatus(bool toFile) override{
	   	std::lock_guard<std::mutex> lock(mtx);  
	   	std::ofstream logFile;
	    if (toFile) {
	        logFile.open("csopesylog.txt", std::ios::app);  // Open in append mode
	        if (!logFile.is_open()) {
	            std::cerr << "Error opening csopesylog.txt for logging." << std::endl;
	            return;
	        }
	    }
	    std::ostream& out = toFile ? logFile : std::cout;
		int coresUsed = 0;
		std::vector<bool> coreOccupied(numCores, false);  
		

		for (const auto& process : allProcesses) {
		    if (process && !process->hasFinished() && !process->getStartExecutionTime().empty()) {  
		        for (int core = 0; core < numCores; ++core) {
		            if (std::find(processQueues[core].begin(), processQueues[core].end(), process) != processQueues[core].end()) {
		                if (!coreOccupied[core]) {  
		                    coreOccupied[core] = true;
		                    coresUsed++;
		                }
		                break;  
		            }
		        }
		    }
		}
		
		double cpuUtilization = (static_cast<double>(coresUsed) / numCores) * 100;  
	
	    // Log CPU utilization and core usage
	    out << "CPU Utilization: " << cpuUtilization << "%\n";
	    out << "Cores Used: " << coresUsed << "/" << numCores << "\n";
	    out << "Free Cores: " << coresUsed-numCores << "\n";

	    out << "--------------------------------------------------\n";
	    out << "Running Processes:\n\n";
	
	    // Iterate through all processes to print Running Processes
	    for (const auto& process : allProcesses){
	        if (process && !process->hasFinished()) { 
	            int coreId = -1; 
	            for (int core = 0; core < numCores; ++core) {
	                if (std::find(processQueues[core].begin(), processQueues[core].end(), process) != processQueues[core].end()) {
	                    coreId = core; 
	                    break;
	                }
	            }
	
	            // Print core ID only if the process has started executing
	            if (!process->getStartExecutionTime().empty()) {
	                out << process->getName() << " (" 
	                          << process->getStartExecutionTime() << ") " 
	                          << "Core: " << coreId << " " 
	                          << process->getExecutedInstructions() << "/" 
	                          << process->getRemainingInstructions() + process->getExecutedInstructions() << "\n";
	            } else {
	                out << process->getName() << " NOT STARTED " 
	                          << process->getExecutedInstructions() << "/" 
	                          << process->getRemainingInstructions() + process->getExecutedInstructions() << "\n";
	            }
	        }
    	}

	    std::cout << "\nFinished Processes:\n\n";
	    
	    for (const auto& process : allProcesses) {
	        if (process && process->hasFinished()) { 
	            out << process->getName() << " (" 
	                      << process->getEndExecutionTime() << ") "
	                      << "Finished " << process->getExecutedInstructions() << "/" 
	                      << process->getRemainingInstructions() + process->getExecutedInstructions() << "\n";
	        }
	    }
	
	    out << "--------------------------------------------------\n";
	    if (toFile) {
	        logFile.close(); 
	    }
    }    

	Process* findProcessByName(const std::string& name) override{
        for (const auto& process : allProcesses) { 
            if (process->getName() == name) {
                return process.get();
            }
        }
        return nullptr; // Not found
    }
};

class Screen {
private:
    std::string borderStyle;  
    std::string name;         
    std::string creationDate; 
    Process* process; // Pointer to the process associated with the screen

public:
    Screen(const std::string &borderStyle, const std::string &name, Process* proc)
        : borderStyle(borderStyle), name(name), creationDate(timestamp()), process(proc) {}

    void create() const {
        std::string content = " Screen Name: " + name + "\n Date Created: " + creationDate;
            content += "\n Process ID: " + std::to_string(process->getId());
            if((process->getTotalInstructions())==(process->getExecutedInstructions())){
            	content += "\n Finished";
			}
			else{
				content +="\n Total Instructions: " + std::to_string(process->getTotalInstructions())
                       + "\n Executed Instructions: " + std::to_string(process->getExecutedInstructions());
			}
        size_t width = 40;
        // Borders
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
    
    void addScreen(const std::string &name,Process* process) {
        std::string borderStyle = getBorderStyle(screens.size()); 
        Screen newScreen(borderStyle, name, process); 
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

void generateProcesses(Scheduler& scheduler) {
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(min_ins, max_ins);
    
    while (processGenerationActive) {
    	processCount++;
        std::this_thread::sleep_for(std::chrono::milliseconds(batch_process_freq*100)); //100 ms is a cpu cycle
        int numInstructions = distr(eng);
        auto process = std::make_shared<Process>("Process " + std::to_string(processCount), processCount, numInstructions);
        scheduler.addProcess(process);
        

        //std::cout << "Generated new process: " << process->getName() << " with " << numInstructions << " instructions.\n";
    }
}


int interpreter(std::string command, ScreenManager& manager,Scheduler& scheduler,std::thread& processGenerationThread){
	
	int isExit = 0;
    if (command == "clear") {
        system("cls"); 
        banner();
    } else if (command == "screen -ls") {
       		scheduler.printProcessStatus(false);
    } else if (command.rfind("screen -s", 0) == 0) {
        size_t startPos = command.find("-s") + 3;  // +3 to skip "-s " part
    		std::string screenName = command.substr(startPos);  
        if (!screenName.empty()) {
        	Process* proc = scheduler.findProcessByName(screenName);
        	if (proc&&!(proc->hasFinished())) {
        		system("cls"); 
		        if (manager.screenExists(screenName)) {
		            std::cout << "Screen already exists. Opening " << screenName << std::endl;
		            
		        } else {
		            std::cout << "Creating new screen: " << screenName << std::endl;
		            manager.addScreen(screenName, proc);
		            }
		                   
	            std::string processCommand;
	            bool exitScreen = false;
	
	            while (!exitScreen) {
	                std::cout << "Process Screen - " << screenName << "\nEnter a command: ";
	                std::getline(std::cin, processCommand);
	
	                if (processCommand == "process-smi") {
	                    manager.callScreen(screenName);
	                } else if (processCommand == "exit") {
	                    std::cout << "Returning to main menu...\n";
	                   	system("cls");
	                    banner();
	                    exitScreen = true;
	                } else {
	                    std::cout << "Invalid command. Try 'process-smi' or 'exit'.\n";
	            	}
            	}
            } else {
                std::cout << "No process found with the name: " << screenName << ", or it might be finished already!"<<std::endl;
            }
        } else {
            std::cout << "Error: No screen name provided!" << std::endl;
        }
    }else if (command.rfind("screen -r", 0) == 0) {
        size_t startPos = command.find("-r") + 3;  // +3 to skip "-r " part
    		std::string screenName = command.substr(startPos);  
        if (!screenName.empty()) {
        	Process* proc = scheduler.findProcessByName(screenName); 
        	if (manager.screenExists(screenName)&&!(proc->hasFinished())) {
        			system("cls");
		            std::cout << "Opening " << screenName << std::endl;
		            std::string processCommand;
	            	bool exitScreen = false;
	           		while (!exitScreen) {
		                std::cout << "Process Screen - " << screenName << "\nEnter a command: ";
		                std::getline(std::cin, processCommand);
		
		                if (processCommand == "process-smi") {
		                    manager.callScreen(screenName);
		                } else if (processCommand == "exit") {
		                    std::cout << "Returning to main menu...\n";
		                   	system("cls");
		                    banner();
		                    exitScreen = true;
		                } else {
		                    std::cout << "Invalid command. Try 'process-smi' or 'exit'.\n";
		            	}
	            	}
		    }
		    else{
		    	std::cout << screenName << " not found or has finished execution." << std::endl;
			}
		}
	}	else if (command.rfind("screen", 0) == 0) {
    	std::cout << "Syntax: screen -s <name>" << std::endl;
	} else if (command == "scheduler-test") {
        std::cout << "Scheduler test started.\n";
        if (!processGenerationActive) {
            processGenerationActive = true;
            processGenerationThread = std::thread(generateProcesses, std::ref(scheduler));
        }
    } else if (command == "scheduler-stop") {
        std::cout << "Stopping scheduler test.\n";

        // Stop the process generation
        processGenerationActive = false;
        if (processGenerationThread.joinable()) {
            processGenerationThread.join();
        }
    } else if (command == "report-util") {
        scheduler.printProcessStatus(true);
        std::cout << "Report generated at csopesylog.txt in the same folder as the emulator!" << std::endl;
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

void runScheduler(Scheduler& scheduler) {
    // While the schedulerRunning flag is true, run the scheduler
    while (schedulerRunning) {
        scheduler.runScheduler();  
    }
}


int main() {
    banner(); 
    
    int isExit = 0;
    ScreenManager manager; 
    std::string command;
    bool initialized = false;
    std::unique_ptr<Scheduler> scheduler;
	std::thread processGenerationThread;
    do {
        std::cout << "Enter a command: ";
        std::getline(std::cin, command);
        initialized = initialize_configs(command);  
    } while (!initialized);

    // After initialization, create the scheduler
    if (schedulerType == "rr") {
        scheduler = std::make_unique<RRScheduler>(num_cpu, quantum_cycles);
    } else if (schedulerType == "fcfs") {
        scheduler = std::make_unique<FCFSScheduler>(num_cpu);
    } else {
        std::cerr << "Invalid scheduler type specified." << std::endl;
        return 1; 
    }

    // Start the scheduler in a separate thread
    std::thread schedulerThread([&scheduler]() {
        while (schedulerRunning) {
            scheduler->runScheduler(); 
        }
    });

    do {
        std::cout << "Enter a command: ";
        std::getline(std::cin, command);
        isExit = interpreter(command, manager, *scheduler, processGenerationThread);
        if (isExit == 1) {
            schedulerRunning = false; 
        }

    } while (isExit != 1);

    if (schedulerThread.joinable()) {
        schedulerThread.join();
    }

    return 0; 
}
