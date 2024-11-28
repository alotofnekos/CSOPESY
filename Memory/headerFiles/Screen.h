#pragma once
#include "process_block.h"
#include <string>
#include <iostream> // For std::cout
#include <ctime>
#include <iomanip>
#include <sstream>

class Screen {
private:
    std::string borderStyle;
    std::string name;
    std::string creationDate;
    process_block* process; // Pointer to the process associated with the screen

public:
    Screen(const std::string& borderStyle, const std::string& name, process_block* proc);
    void create() const;
    std::string getName() const;
};
