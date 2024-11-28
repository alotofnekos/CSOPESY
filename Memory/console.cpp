#include "headerFiles/config.h"
#include "headerFiles/console.h"
#include "headerFiles/process_block.h"
#include "headerFiles/scheduler.h"
#include <cstdlib>
#include <ctime>
#include <iomanip>


console::console() : consoleScheduler(nullptr), initialized(false) {}

void console::displayBanner() {
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

class Screen {
private:
    std::string borderStyle;
    std::string name;
    std::string creationDate;
    process_block* process; // Pointer to the process associated with the screen

public:
    Screen(const std::string& borderStyle, const std::string& name, process_block* proc)
        : borderStyle(borderStyle), name(name), creationDate(console::timestamp()), process(proc) {
    }

    void create() const {
        std::string content = " Screen Name: " + name + "\n Date Created: " + creationDate;
        content += "\n Process Name: " + process->getName();

        if ((process->getTotalInstructions()) == (process->getExecutedInstructions())) {
            content += "\n Finished";
        }
        else {
            content += "\n Total Instructions: " + std::to_string(process->getTotalInstructions())
                + "\n Executed Instructions: " + std::to_string(process->getExecutedInstructions())
                +"\n Memory Used: " + std::to_string(process->getMemorySize()) + " MiB ";
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

    void addScreen(const std::string& name, process_block* process) {
        std::string borderStyle = getBorderStyle(screens.size());
        Screen newScreen(borderStyle, name, process);
        screens.push_back(newScreen);
    }

    void callScreen(const std::string& name) const {
        for (size_t i = 0; i < screens.size(); ++i) {
            if (screens[i].getName() == name) {
                screens[i].create();
                return;
            }
        }
        std::cout << "Screen named \"" << name << "\" not found." << std::endl;
    }

    bool screenExists(const std::string& name) const {
        for (size_t i = 0; i < screens.size(); ++i) {
            if (screens[i].getName() == name) {
                return true;
            }
        }
        return false;
    }


};

void console::printReport(bool toFile) {
    int activeCores = 0;
    int inactiveCores = 0; 
    std::ofstream logFile;
    if (toFile) {
        logFile.open("csopesylog.txt", std::ios::app);  // Open in append mode
        if (!logFile.is_open()) {
            std::cerr << "Error opening csopesylog.txt for logging." << std::endl;
            return;
        }
    }
    std::ostream& out = toFile ? logFile : std::cout;

    for (const auto &core : *cores)
    {
        if (core.assigned == false)
        {
            inactiveCores++;
        } else
        {
            activeCores++;
        }
    }
    
    int util = (static_cast<double>(activeCores) / (activeCores + inactiveCores)) * 100; 

    out << "CPU utiliziation: " << util << "%";
    out << "\nCores used: " << activeCores;
    out << "\nCores available: " << inactiveCores << "\n";
    out << "-----------------------------------------------------------\n";

    out << "\n\nRunning Processes: \n";
    
    for (const auto &core : *cores)
    {
        if (core.assigned == true)
        {
            out << core.process_block->getName() << "\t(" << std::put_time(std::localtime(&core.process_block->startTime),"%Y-%m-%d %H:%M:%S") << ")\t Core: " << core.process_block->getCore() << "\t " << core.process_block->getExecutedInstructions() << "/" << core.process_block->getTotalInstructions() << "\n";
        }
    }
    
    out << "\n\nFinished Processes: \n";

    for (const auto &process_block : *processes)
    {
        if (process_block->getDone() == true)
        {
            out << process_block->getName() << "\t (" << std::put_time(std::localtime(&process_block->startTime),"%Y-%m-%d %H:%M:%S") << ")\t Status: Finished" << "\t" << process_block->getExecutedInstructions() << "/" << process_block->getTotalInstructions() << "\n";
        }    
    }
    out << "-----------------------------------------------------------\n";

    if (toFile) {
        logFile.close();
    }
}

void console::printProcessSMI(bool toFile) {
    std::ofstream logFile;
    if (toFile) {
        logFile.open("csopesylog.txt", std::ios::app);  // Open in append mode
        if (!logFile.is_open()) {
            std::cerr << "Error opening csopesylog.txt for logging." << std::endl;
            return;
        }
    }
    std::ostream& out = toFile ? logFile : std::cout;
    out << "-----------------------------------------------------------\n";
    out << "||    PROCESS-SMI V01.00         DRIVER VERSION: 1.00    ||\n";
    out << "-----------------------------------------------------------\n";
    int activeCores = 0;
    int inactiveCores = 0;
    int totalMemoryUsed = consoleScheduler->getTotalMemUsed();
    for (const auto& core : *cores)
    {
        if (core.assigned == false)
        {
            inactiveCores++;
        }
        else
        {
            activeCores++;
        }
    }

    int util = (static_cast<double>(activeCores) / (activeCores + inactiveCores)) * 100;
    int memUtil = (static_cast<double>(totalMemoryUsed) / (consoleScheduler->getMaxOverallMemory())) * 100;
    out << "CPU utiliziation: " << util << "%";
    out << "\nMemory Usage: " << totalMemoryUsed <<"MiB / " << consoleScheduler->getMaxOverallMemory() <<"MiB";
    out << "\nMemory Utilization: " << memUtil << "%\n";
    out << "-----------------------------------------------------------\n";

    out << "\nRunning Processes and Memory Usage: \n";

    out << "-----------------------------------------------------------\n";
    for (const auto& core : *cores)
    {
        if (core.assigned == true)
        {
            out << core.process_block->getName() << "\t" << core.process_block->getMemorySize() << "MiB\n";
        }
    }
    out << "-----------------------------------------------------------\n";
    if (toFile) {
        logFile.close();
    }
}

void console::vmstat(bool toFile) {
    std::ofstream logFile;
    if (toFile) {
        logFile.open("csopesylog.txt", std::ios::app);  // Open in append mode
        if (!logFile.is_open()) {
            std::cerr << "Error opening csopesylog.txt for logging." << std::endl;
            return;
        }
    }
    std::ostream& out = toFile ? logFile : std::cout;
    out << "-----------------------------------------------------------\n";
    out << "||               VIRTUAL MEMORY STATISTICS               ||\n";
    out << "-----------------------------------------------------------\n";
    int idleCPUTicks = 0;
    int activeCPUTicks = 0;
    int totalMemoryUsed = consoleScheduler->getTotalMemUsed();
    for (const auto& core : *cores)
    {
        idleCPUTicks = idleCPUTicks + core.idleTicks;
        activeCPUTicks = activeCPUTicks + core.activeTicks;
    }
    out << "\nTotal Memory: " << consoleScheduler->getMaxOverallMemory() << "MiB";
    out << "\nUsed Memory: " << totalMemoryUsed << "MiB";
    out << "\nFree Memory: " << (consoleScheduler->getMaxOverallMemory()) - totalMemoryUsed << "MiB";
    out << "\nIdle CPU Ticks: " << idleCPUTicks;
    out << "\nActive CPU Ticks: " << activeCPUTicks;
    out << "\nTotal CPU Ticks: " << activeCPUTicks + idleCPUTicks;
    out << "\nNum Paged in: " << activeCPUTicks - activeCPUTicks << "";
    out << "\nNum Paged Out: " << activeCPUTicks - activeCPUTicks << "";
    out << "\n-----------------------------------------------------------\n";
    if (toFile) {
        logFile.close();
    }
}

void console::interpreter(const std::string& command) {
    ScreenManager manager;
    if (!initialized && command != "initialize" && command != "exit")
    {
        if (command == "clear")
        {
            system("cls");
            displayBanner();
        }
        else if (command == "exit")
        {
            exit(0);
        }
        else
        {
            std::cout << "Initialize first.\n";
        }
        return;
    }
    else if (command == "initialize")
    {
        config config;
        if (config.initializeConfig() == true) {
            consoleScheduler = new scheduler(config, processes);
            config.displayConfig();
            initialized = true;
            this->cores = consoleScheduler->getCores();
            consoleScheduler->initializeCores();
        }
        else {
            std::cout << "Initialization failed. Check config.txt.\n";
        }

    }
    else if (initialized)
    {
        if (command == "scheduler-test")
        {
            consoleScheduler->setGenerateProcesses(true);
            consoleScheduler->startGenerateProcessesThread();
            consoleScheduler->generateReportThread();
            std::cout << "Scheduler Test started. Generating Processes...\n";
        }
        else if (command == "scheduler-stop")
        {
            consoleScheduler->setGenerateProcesses(false);
            std::cout << "Stopped scheduler test.\n";
        }
        else if (command == "clear")
        {
            system("cls");
            displayBanner();
        }
        else if (command == "screen -ls")
        {
            printReport(false);
        }
        else if (command == "report-util")
        {
            printReport(true);
            printProcessSMI(true);
            vmstat(true);
            std::cout << "Report generated at csopesylog.txt in the same folder as the emulator!" << std::endl;

        }
        else if (command.rfind("screen -s", 0) == 0) {
            size_t startPos = command.find("-s") + 3;  // +3 to skip "-s " part
            std::string screenName = command.substr(startPos);
            if (!screenName.empty()) {
                process_block* proc = consoleScheduler->findProcessByName(screenName);
                if (proc && !(proc->getDone())) {
                    system("cls");
                    if (manager.screenExists(screenName)) {
                        std::cout << "Screen already exists. Opening " << screenName << std::endl;

                    }
                    else {
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
                        }
                        else if (processCommand == "exit") {
                            std::cout << "Returning to main menu...\n";
                            system("cls");
                            displayBanner();
                            exitScreen = true;
                        }
                        else {
                            std::cout << "Invalid command. Try 'process-smi' or 'exit'.\n";
                        }
                    }
                }
                else {
                    std::cout << "No process found with the name: " << screenName << ", or it might be finished already!" << std::endl;
                }
            }
            else {
                std::cout << "Error: No screen name provided!" << std::endl;
            }
        }
        else if (command.rfind("screen -r", 0) == 0) {
            size_t startPos = command.find("-r") + 3;  // +3 to skip "-r " part
            std::string screenName = command.substr(startPos);
            if (!screenName.empty()) {
                process_block* proc = consoleScheduler->findProcessByName(screenName);
                if (manager.screenExists(screenName) && !(proc->getDone())) {
                    system("cls");
                    std::cout << "Opening " << screenName << std::endl;
                    std::string processCommand;
                    bool exitScreen = false;
                    while (!exitScreen) {
                        std::cout << "Process Screen - " << screenName << "\nEnter a command: ";
                        std::getline(std::cin, processCommand);

                        if (processCommand == "process-smi") {
                            manager.callScreen(screenName);
                        }
                        else if (processCommand == "exit") {
                            std::cout << "Returning to main menu...\n";
                            system("cls");
                            displayBanner();
                            exitScreen = true;
                        }
                        else {
                            std::cout << "Invalid command. Try 'process-smi' or 'exit'.\n";
                        }
                    }
                }
                else {
                    std::cout << screenName << " not found or has finished execution." << std::endl;
                }
            }
        }
        else if (command.rfind("screen", 0) == 0) {
            std::cout << "Syntax: screen -s <name>" << std::endl;
        }   
        else if (command == "process-smi")
        { 
            printProcessSMI(false);
        }
        else if (command == "vmstat") {

            vmstat(false);

        }
        else if (command == "exit")
        {
            exit(0);
        } else 
        {
            std::cout << "Unknown command. Try again.\n";
        }
    }
}

void console::start() {
    displayBanner(); 
    std::string command; 
    
    while (true)
    {
        std::cout << "\nEnter command: ";
        std::getline(std::cin, command); 
        interpreter(command); 
    }
     
}




int main(){
    console console; 
    console.start();
    return 0; 
}
