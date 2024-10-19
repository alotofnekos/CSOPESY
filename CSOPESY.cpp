#include <iostream>
#include <cstdlib>
#include <conio.h>
#include <stdio.h>
#include <ctime>
#include <string>   
#include <sstream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <fstream>

int num_cpu;
std::string scheduler;
int quantum_cycles;
int batch_process_freq;
int min_ins;
int max_ins;
int delays_per_exec;

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

int interpreter(std::string command, ScreenManager& manager){

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
        std::cout << command << " command recognized. Doing something." << std::endl;
    } else if (command == "scheduler-stop") {
        std::cout << command << " command recognized. Doing something." << std::endl;
    } else if (command == "report-util") {
        std::cout << command << " command recognized. Doing something." << std::endl;
    } else if (command == "non-blocking") {
        std::cout << command << " command recognized. Doing something." << std::endl;
        non_blocking();
    } else if (command == "exit") {
        isExit=1;
    } else {
        std::cout << command << " is not recognized!" << std::endl;
    }
	return isExit;
}

int main() {
    banner();
    int isExit =0;
    ScreenManager manager;
   	std::string command;
   	bool initialized = false;

	do {
    	std::cout << "Enter a command: ";
    	std::getline(std::cin, command);
    	isExit = interpreter(command, manager);
	} while (isExit!=1);

    exit (0);
}
