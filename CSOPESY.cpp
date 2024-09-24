#include <iostream>
#include <cstdlib>
#include <conio.h>
#include <stdio.h>
#include <ctime>
#include <string>   
#include <sstream>
#include <vector>
#include <iomanip>

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
        std::cout << command << " command recognized. Doing something." << std::endl;
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
	do {
    	std::cout << "Enter a command: ";
    	std::getline(std::cin, command);
    	isExit = interpreter(command, manager);
	} while (isExit!=1);

    exit (0);
}
