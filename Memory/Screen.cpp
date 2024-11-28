#include "Screen.h"

Screen::Screen(const std::string& borderStyle, const std::string& name, process_block* proc)
    : borderStyle(borderStyle), name(name), process(proc) {
    std::time_t now = std::time(nullptr);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S");
    creationDate = oss.str();
}

void Screen::create() const {
    std::string content = " Screen Name: " + name + "\n Date Created: " + creationDate;
    content += "\n Process Name: " + process->getName();

    if ((process->getTotalInstructions()) == (process->getExecutedInstructions())) {
        content += "\n Finished";
    }
    else {
        content += "\n Total Instructions: " + std::to_string(process->getTotalInstructions())
            + "\n Executed Instructions: " + std::to_string(process->getExecutedInstructions())
            + "\n Memory Used: " + std::to_string(process->getMemorySize()) + " MiB ";
    }

    size_t width = 40;
    // Borders
    std::cout << std::string(width, borderStyle[0]) << std::endl;
    std::cout << content << std::endl;
    std::cout << std::string(width, borderStyle[0]) << std::endl;
}

std::string Screen::getName() const {
    return name;
}
