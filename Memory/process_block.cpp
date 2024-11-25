#include "headerFiles/process_block.h"

process_block::process_block(std::string name) : name(std::move(name)) {};

std::string process_block::getName() const {
    return name; 
}

int process_block::getExecutedInstructions() const {
    return executedInstructions; 
}

void process_block::setExecutedInstructions(int instructions) {
    executedInstructions = instructions;
}

int process_block::getTotalInstructions() const {
    return totalInstructions;
}

void process_block::setTotalInstructions(int instructions) {
    totalInstructions = instructions;
} 

int  process_block::getMemorySize() const {
    return memorySize;
}
void process_block::setMemorySize(int memory) {
    memorySize = memory;
}

int process_block::getCore() const {
    return core; 
}

void process_block::setCore(int core) {
    this->core = core;
}

bool process_block::getDone() const {
    return isDone;
}

void process_block::setDone(bool status) {
    isDone = status;
}

bool process_block::getRunning() const {
    return isRunning;
}

void process_block::setRunning(bool status) {
    isRunning = status; 
}

bool process_block::getWaiting() const {
    return isWaiting; 
}

void process_block::setWaiting(bool status) {
    isWaiting = status; 
}
