#include <iostream>
#include <cstdlib>
#include <conio.h>
#include <stdio.h>
#include <ctime>
#include <string>   
#include <sstream>

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

void screen(const std::string& screenName) {
    std::string command = "start cmd /k \"echo Process name " + screenName + " opened at: " + timestamp() +
                          " && echo Type 'exit' to close this screen.";
    system(command.c_str());
}

int interpreter(std::string command){
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
            std::cout << "Opening " << screenName << std::endl;
            screen(screenName); 
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
   	std::string command;
	do {
    	std::cout << "Enter a command: ";
    	std::getline(std::cin, command);
    	isExit = interpreter(command);
	} while (isExit!=1);

    exit (0);
}
