#pragma once
#include "Screen.h"
#include "process_block.h"
#include <string>
#include <vector>
#include <iostream> // For std::cout

class ScreenManager {
private:
    std::vector<Screen> screens;
    std::string getBorderStyle(size_t index) const;

public:
    void addScreen(const std::string& name, process_block* process);
    void callScreen(const std::string& name) const;
    bool screenExists(const std::string& name) const;
};
