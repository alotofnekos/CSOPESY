#include "headerFiles/process.h"

process::process(std::string name) : name(std::move(name)) {};

std::string process::getName() const {
    return name; 
}

int process::getExecutedInstructions() const {
    return executedInstructions; 
}

void process::setExecutedInstructions(int instructions) {
    executedInstructions = instructions;
}

int process::getTotalInstructions() const {
    return totalInstructions;
}

void process::setTotalInstructions(int instructions) {
    totalInstructions = instructions;
} 

int process::getCore() const {
    return core; 
}

void process::setCore(int core) {
    this->core = core;
}

bool process::getDone() const {
    return isDone;
}

void process::setDone(bool status) {
    isDone = status;
}

bool process::getRunning() const {
    return isRunning;
}

void process::setRunning(bool status) {
    isRunning = status; 
}

bool process::getWaiting() const {
    return isWaiting; 
}

void process::setWaiting(bool status) {
    isWaiting = status; 
}