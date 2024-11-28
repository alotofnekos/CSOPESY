#include "headerFiles/config.h"
#include "headerFiles/console.h"
#include "headerFiles/process_block.h"
#include "headerFiles/scheduler.h"
#include <cstdlib>
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

void console::printReport() {
    int activeCores = 0;
    int inactiveCores = 0; 

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

    std::cout << "CPU utiliziation: " << util << "%";
    std::cout << "\nCores used: " << activeCores;
    std::cout << "\nCores available: " << inactiveCores << "\n";
    std::cout << "-----------------------------------------------------------\n";

    std::cout << "\n\nRunning Processes: \n";
    
    for (const auto &core : *cores)
    {
        if (core.assigned == true)
        {
            std::cout << core.process_block->getName() << "\t(" << std::put_time(std::localtime(&core.process_block->startTime),"%Y-%m-%d %H:%M:%S") << ")\t Core: " << core.process_block->getCore() << "\t " << core.process_block->getExecutedInstructions() << "/" << core.process_block->getTotalInstructions() << "\n";
        }
    }
    
    std::cout << "\n\nFinished Processes: \n";

    for (const auto &process_block : *processes)
    {
        if (process_block->getDone() == true)
        {
            std::cout << process_block->getName() << "\t (" << std::put_time(std::localtime(&process_block->startTime),"%Y-%m-%d %H:%M:%S") << ")\t Status: Finished" << "\t" << process_block->getExecutedInstructions() << "/" << process_block->getTotalInstructions() << "\n";
        }    
    }
    std::cout << "-----------------------------------------------------------\n";
}

void console::printProcessSMI() {
    std::cout << "-----------------------------------------------------------\n";
    std::cout << "||    PROCESS-SMI V01.00         DRIVER VERSION: 1.00    ||\n";
    std::cout << "-----------------------------------------------------------\n";
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
    std::cout << "CPU utiliziation: " << util << "%";
    std::cout << "\nMemory Usage: " << totalMemoryUsed <<"MiB / " << consoleScheduler->getMaxOverallMemory() <<"MiB";
    std::cout << "\nMemory Utilization: " << memUtil << "%\n";
    std::cout << "-----------------------------------------------------------\n";

    std::cout << "\nRunning Processes and Memory Usage: \n";

    std::cout << "-----------------------------------------------------------\n";
    for (const auto& core : *cores)
    {
        if (core.assigned == true)
        {
            std::cout << core.process_block->getName() << "\t" << core.process_block->getMemorySize() << "MiB\n";
        }
    }
}

void console::vmstat() {
    std::cout << "-----------------------------------------------------------\n";
    std::cout << "||               VIRTUAL MEMORY STATISTICS               ||\n";
    std::cout << "-----------------------------------------------------------\n";
    int idleCPUTicks = 0;
    int activeCPUTicks = 0;
    int totalMemoryUsed = consoleScheduler->getTotalMemUsed();
    for (const auto& core : *cores)
    {
        idleCPUTicks = idleCPUTicks + core.idleTicks;
        activeCPUTicks = activeCPUTicks + core.activeTicks;
    }
    std::cout << "\nTotal Memory: " << consoleScheduler->getMaxOverallMemory() << "MiB";
    std::cout << "\nUsed Memory: " << totalMemoryUsed << "MiB";
    std::cout << "\nFree Memory: " << (consoleScheduler->getMaxOverallMemory()) - totalMemoryUsed << "MiB";
    std::cout << "\nIdle CPU Ticks: " << idleCPUTicks;
    std::cout << "\nActive CPU Ticks: " << activeCPUTicks;
    std::cout << "\nTotal CPU Ticks: " << activeCPUTicks + idleCPUTicks;
    std::cout << "\nNum Paged in: " << activeCPUTicks - activeCPUTicks << "";
    std::cout << "\nNum Paged Out: " << activeCPUTicks - activeCPUTicks << "";
    std::cout << "\n-----------------------------------------------------------\n";

}

void console::interpreter(const std::string &command) {
    if (!initialized && command != "initialize" && command != "exit")
    {
        if (command == "clear")
        {
            system("cls");
            displayBanner(); 
        } else if (command == "exit")
        {
            exit(0); 
        } else
        {
            std::cout << "Initialize first.\n";
        }
        return;
    } else if (command == "initialize")
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

    } else if (initialized)
    {
        if (command == "scheduler-test")
        {
            consoleScheduler->setGenerateProcesses(true); 
            consoleScheduler->startGenerateProcessesThread(); 
            consoleScheduler->generateReportThread();
            std::cout << "Scheduler Test started. Generating Processes...\n";
        } else if (command == "scheduler-stop")
        {
            consoleScheduler->setGenerateProcesses(false); 
            std::cout << "Stopped scheduler test.\n";
        } else if (command == "clear")
        {
            system("cls");
            displayBanner();
        } else if (command == "screen -ls")
        {
            printReport();
        }else if (command == "process-smi")
        { 
            printProcessSMI();
        }
        else if (command == "vmstat") {

            vmstat();

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
