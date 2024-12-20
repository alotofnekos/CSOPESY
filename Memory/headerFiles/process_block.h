#pragma once
#include <iostream>
#include <set> 
#include <ctime> 

class process_block
{
private:
    std::string name; 
    int core = -1; 
    int executedInstructions=0; 
    int totalInstructions; 
    int memorySize;

    bool isDone = false;
    bool isRunning = false;
    bool isWaiting = false; 

public:
    std::vector<int> occupiedFrameIndex; 
    process_block(std::string name);

    std::time_t startTime = 0; 
    std::time_t endTime = 0; 

    std::string getName() const; 

    int getExecutedInstructions() const; 
    void setExecutedInstructions(int instructions);

    int getTotalInstructions() const; 
    void setTotalInstructions(int instructions); 

    int getMemorySize() const;
    void setMemorySize(int instructions);

    int getCore() const; 
    void setCore(int core); 

    bool getDone() const; 
    void setDone(bool status); 

    bool getRunning() const; 
    void setRunning(bool status);

    bool getWaiting() const; 
    void setWaiting(bool status);
};
