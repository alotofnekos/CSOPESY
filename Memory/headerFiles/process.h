#pragma once
#include <iostream>
#include <set> 
#include <ctime> 

class process
{
private:
    std::string name; 
    int core = -1; 
    int executedInstructions; 
    int totalInstructions; 

    bool isDone = false;
    bool isRunning = false;
    bool isWaiting = false; 

public:
    process(std::string name);

    std::time_t startTime = 0; 
    std::time_t endTime = 0; 

    std::string getName() const; 

    int getExecutedInstructions() const; 
    void setExecutedInstructions(int instructions);

    int getTotalInstructions() const; 
    void setTotalInstructions(int instructions); 

    int getCore() const; 
    void setCore(int core); 

    bool getDone() const; 
    void setDone(bool status); 

    bool getRunning() const; 
    void setRunning(bool status);

    bool getWaiting() const; 
    void setWaiting(bool status);
};
