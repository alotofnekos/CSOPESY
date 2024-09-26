#include <iostream>
#include <cstdlib>
#include <conio.h>
#include <stdio.h>
#include <ctime>
#include <string>   
#include <sstream>
#include <vector>
#include <iomanip>
#include <thread> 
#include <chrono> 

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

void non_blocking() {
    int ch;  
    while ((ch = _getch()) != 'q') { 
        char keyPressed = static_cast<char>(ch);
        std::cout << "Key pressed: " << keyPressed << std::endl;
    }
}

class Screen {
private:
    std::string borderStyle;  
    std::string name;         
    std::string createDateTime; 

    std::string timestamp() const {
        time_t currentTime;
        time(&currentTime);
        struct tm *localTime = localtime(&currentTime);
        char formattedTime[100];

        strftime(formattedTime, sizeof(formattedTime), "%m/%d/%Y, %I:%M:%S %p", localTime);
        return std::string(formattedTime);  
    }

public:
    Screen(const std::string &borderStyle_, const std::string &name_)
    : borderStyle(borderStyle_), name(name_), createDateTime(timestamp()) {}


    void printScreen() const {
    	std::this_thread::sleep_for(std::chrono::milliseconds(1000)); 
    	system("cls");
        std::string content = " Screen Name: " + name + " \n Date Created: " + createDateTime;
        size_t width = std::max(content.length(), static_cast<size_t>(2));

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
    Screen* activeScreen = nullptr;
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
    void setActiveScreen(const std::string &name) {
        for (Screen &screen : screens) {
            if (screen.getName() == name) {
                activeScreen = &screen;
                std::cout << "Switched to screen: " << name << std::endl;
                return;
            }
        }
        std::cout << "Screen not found. Remaining on the main screen." << std::endl;
    }

    void addScreen(const std::string &name) {
        std::string borderStyle = getBorderStyle(screens.size()); 
        Screen newScreen(borderStyle, name); 
        screens.push_back(newScreen); 
        callScreen(name); 
    }

    void callScreen(const std::string &name) {
        for (size_t i = 0; i < screens.size(); ++i){ 
	            if (screens[i].getName() == name) { 
                screens[i].printScreen(); 
                setActiveScreen(name);
                return;
            }
        }
        std::cout << "Screen named \"" << name << "\" not found." << std::endl; 
    }

    void exitActiveScreen() {
        if (activeScreen != nullptr) {
            std::cout << "Exiting screen: " << activeScreen->getName() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(2000)); 
            system("cls");
            banner();
            activeScreen = nullptr;  
        } else {
            std::cout << "You are already on the main screen!" << std::endl;
        }
    }
    
    bool screenExists(const std::string &name) const {
        for (size_t i = 0; i < screens.size(); ++i) {
            if (screens[i].getName() == name) {
                return true; 
            }
        }
        return false;
    }

    bool isOnMainScreen() const {
        return activeScreen == nullptr;  
    }
};

int interpreter(std::string command, ScreenManager& manager) {
    int isExit = 0;

    if (manager.isOnMainScreen()) {  
        // Main Screen
        if (command == "clear") {
            system("cls");
            banner();
        } else if (command == "scheduler-test") {
           std::cout << command << " command recognized. Doing something." << std::endl;
        } else if (command == "scheduler-stop") {
            std::cout << command << " command recognized. Doing something." << std::endl;
        } else if (command == "report-util") {
           std::cout << command << " command recognized. Doing something." << std::endl;
        } else if (command == "non-blocking") {
            std::cout << "Entering non-blocking mode..." << std::endl;
            non_blocking();
        // Create Screen Command 
        } else if (command.find("screen -s") == 0) { 
            std::string screenName = command.substr(10);  

            if (!screenName.empty()) {
                if (manager.screenExists(screenName)) {
                    std::cout << "Screen already exists. Switching to " << screenName << std::endl;
                    manager.callScreen(screenName);
                } else {
                    std::cout << "Creating new screen: " << screenName << std::endl;
                    manager.addScreen(screenName);
                }
            } else {
                std::cout << "Error: No screen name provided!" << std::endl;
            }
        // Switch Screen Command 
        } else if (command.rfind("screen -r", 0) == 0) {  
            std::string screenName = command.substr(10);

            if (!screenName.empty()) {
                if (manager.screenExists(screenName)) {
                    std::cout << "Switching to screen: " << screenName << std::endl;
                    manager.callScreen(screenName);
                } else {
                    std::cout << "Error: Screen '" << screenName << "' does not exist." << std::endl;
                }
            } else {
                std::cout << "Error: No screen name provided!" << std::endl;
            }
        // Screen
        } else if (command == "screen") {
           std::cout << command << " command recognized. Doing something." << std::endl;
           std::cout << "Displaying syntax for " << command << std::endl;
           std::cout << command << "-r <name> Opens a screen if it exists" << std::endl;
           std::cout << command << "-s <name> Opens a screen if it exists and creates it if it doesn't exist yet" << std::endl;
        // End Process Command
        }else if (command == "exit") {  
            isExit = 1;

        } else {
            std::cout << command << " is not recognized!" << std::endl;
        }
    // Non-Main Screen Commands
    } else {  
        if (command == "exit") {
            manager.exitActiveScreen();
        } else {
            std::cout << "You are in a screen. Use 'exit' to return to the main menu." << std::endl;
        }
    }

    return isExit;
}

int main() {
    banner();
    int isExit = 0;
    ScreenManager manager;
    std::string command;

    do {
        std::cout << "Enter a command: ";
        std::getline(std::cin, command);
        isExit = interpreter(command, manager);
    } while (isExit != 1);

    exit(0);
}
