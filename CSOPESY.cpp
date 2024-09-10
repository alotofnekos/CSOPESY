#include <iostream>
#include <cstdlib>

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

int interpreter(std::string command){
	int isExit = 0;
    if (command == "clear") {
        system("cls"); 
        banner();
    } else if (command == "initialize") {
        std::cout << command << " command recognized. Doing something." << std::endl;
    } else if (command == "screen") {
        std::cout << command << " command recognized. Doing something." << std::endl;
    } else if (command == "scheduler-test") {
        std::cout << command << " command recognized. Doing something." << std::endl;
    } else if (command == "scheduler-stop") {
        std::cout << command << " command recognized. Doing something." << std::endl;
    } else if (command == "report-util") {
        std::cout << command << " command recognized. Doing something." << std::endl;
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
