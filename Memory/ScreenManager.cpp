#include "ScreenManager.h"

// Get the border style based on index
std::string ScreenManager::getBorderStyle(size_t index) const {
    switch (index % 5) {
    case 0: return "#";
    case 1: return "*";
    case 2: return "~";
    case 3: return "x";
    case 4: return "+";
    default: return "#";
    }
}

// Add a screen to the manager
void ScreenManager::addScreen(const std::string& name, process_block* process) {
    std::string borderStyle = getBorderStyle(screens.size());
    Screen newScreen(borderStyle, name, process);
    screens.push_back(newScreen);
}

// Call a specific screen by name
void ScreenManager::callScreen(const std::string& name) const {
    for (size_t i = 0; i < screens.size(); ++i) {
        if (screens[i].getName() == name) {
            screens[i].create();
            return;
        }
    }
    std::cout << "Screen named \"" << name << "\" not found." << std::endl;
}

// Check if a screen exists by name
bool ScreenManager::screenExists(const std::string& name) const {
    for (size_t i = 0; i < screens.size(); ++i) {
        if (screens[i].getName() == name) {
            return true;
        }
    }
    return false;
}
