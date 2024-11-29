#include "headerFiles/memory.h"

#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <mutex>

memory::memory(int max_overall_memory, int memory_per_frame, int memory_per_process) 
    : max_overall_memory(max_overall_memory), 
      memory_per_frame(memory_per_frame), 
      memory_per_process(memory_per_process),
      frameTable(max_overall_memory / memory_per_frame) // Initialize frameTable
{
    // Initialize the freeFramesList in the constructor based on frameTable
    freeFramesList.reserve(frameTable.size()); // Reserve space in vector for performance optimization
    for (int i = 0; i < frameTable.size(); i++) {
        freeFramesList.push_back(i); // Initially, all frames are free, so add all indices to freeFramesList
    }

    memoryBlocks.push_back({0, max_overall_memory - 1, ""}); // Initial memory block
}

int memory::findFit(int size) {
    for (int i = 0; i < memoryBlocks.size(); i++)
    {
        if (memoryBlocks[i].proc.empty() && memoryBlocks[i].endAddress - memoryBlocks[i].startAddress + 1 >= size) 
        {
            return i;
        }
        
    }
    return -1; 
}

bool memory::allocateMemory(const std::string &proc, int size) {
    int i = findFit(size);

    if (i == -1)
    {
        return false;
    }
    
    memoryBlock &freeBlock = memoryBlocks[i]; 
    if (freeBlock.endAddress - freeBlock.startAddress + 1 == size)
    {
        freeBlock.proc = proc; 
        freeBlock.timestamp = std::time(nullptr);
    } 
    else
    {
        memoryBlock insertion = {freeBlock.startAddress, freeBlock.startAddress + size - 1, proc};
        freeBlock.timestamp = std::time(nullptr);
        freeBlock.startAddress += size; 
        memoryBlocks.insert(memoryBlocks.begin() + i, insertion);
    }
    return true; 
}

void memory::deallocateMemory(const std::string &proc) {
    for (auto i = memoryBlocks.begin(); i < memoryBlocks.end(); i++)
    {
        if (i->proc == proc)
        {
            i->proc = ""; 
            if (i != memoryBlocks.begin() && (i - 1)->proc.empty()) 
            {
                i->startAddress = (i - 1)->startAddress;
                memoryBlocks.erase(i - 1);
                --i; 
            }

            if (i != memoryBlocks.end() - 1 && (i + 1)->proc.empty())
            {
                i->endAddress = (i + 1)->endAddress;
                memoryBlocks.erase(i + 1);
            }
            break;
        }
    }
}

bool memory::searchProc(const std::string &proc) {
    for (const auto &block : memoryBlocks)
    {
        if (block.proc == proc)
        {
            return true; 
        }
    }
    return false; 
}

int memory::getExternalFragmentation() {
    int x = 0;
    for (const auto &block : memoryBlocks)
    {
        if (block.proc.empty())
        {
            x += block.endAddress - block.startAddress + 1;
        }
    }
    return x; 
}

void memory::printMemory() {
    std::time_t t = std::time(nullptr);
    std::cout << "Timestamp: " << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S") << std::endl; 
    std::cout << "Number of processes in memory: " << std::count_if(memoryBlocks.begin(), memoryBlocks.end(), [](const memoryBlock &block) {return !block.proc.empty();}) << std::endl;
    std::cout << "Total external fragmentation: " << getExternalFragmentation() << " KB" << std::endl; 
    std::cout << "Memory Layout: " << std::endl; 
    std::cout << "----start---- = 0" << std::endl;
    for (const auto &block : memoryBlocks)
    {
        if (block.proc.empty())
        {
            std::cout << "\n"; 
        }
        else
        {
            std::cout << block.startAddress << std::endl;
            std::cout << block.proc << std::endl;
            std::cout << block.endAddress << std::endl;
        }        
    }
    std::cout << "----end---- = " << max_overall_memory << std::endl;
}

void memory::generateReport(const std::string &file) {
    std::string path = "./memory_stamps";

    if (!std::filesystem::exists(path)) 
    {
        std::filesystem::create_directory(path);
    }

    std::string filePath = path + "/" + file; 
    std::ofstream report(filePath); 

    time_t t = std::time(nullptr);
    report << "Timestamp: " << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S") << std::endl; 
    report << "Number of processes in memory: " << std::count_if(memoryBlocks.begin(), memoryBlocks.end(), [](const memoryBlock &block) {return !block.proc.empty();}) << std::endl;
    report << "Total external fragmentation: " << getExternalFragmentation() << " KB" << std::endl; 
    report<< "Memory Layout: " << std::endl; 
    report<< "----start---- = 0" << std::endl;
    for (const auto &block : memoryBlocks)
    {
        if (block.proc.empty())
        {
            report << "\n"; 
        }
        else
        {
            report << block.startAddress << std::endl;
            report << block.proc << std::endl;
            report<< block.endAddress << std::endl;
        }        
    }
    report << "----end---- = " << max_overall_memory << std::endl;
    report.close(); 
}

std::string memory::removeOldestProcess() {
    auto oldest = std::min_element(memoryBlocks.begin(), memoryBlocks.end(),
        [](const memoryBlock &a, const memoryBlock &b) {
            return !a.proc.empty() && (b.proc.empty() || a.timestamp < b.timestamp);
        });

    std::string evictedProc = oldest->proc;

    deallocateMemory(evictedProc);

    return evictedProc;
}

int memory::getTotalMemoryUsed() const {
    int totalMemoryUsed = 0;
    for (const auto& block : memoryBlocks) {
        if (!block.proc.empty()) {
            totalMemoryUsed += block.endAddress - block.startAddress + 1;
        }
    }
    return totalMemoryUsed;
}

void memory::populateFreeFramesList() {
    freeFramesList.clear(); // Clear the list before adding fresh values
    for (int i = 0; i < frameTable.size(); i++) {
        if (frameTable[i].proc == "") {
            freeFramesList.push_back(i); // Add only free frames
        }
    }
}

bool memory::allocateFrames(const std::string &proc, int pages) {
    // generateReportFrames();
    if (pages > freeFramesList.size()) {
        return false; // If there are not enough free frames
    }
    for (int i = 0; i < pages; i++) {
        // Assign the frame
        frameTable[freeFramesList[i]].proc = proc;
        frameTable[freeFramesList[i]].timestamp = std::time(nullptr);
        pagesIn++; 
    }
    // Remove allocated frames from the free list
    freeFramesList.erase(freeFramesList.begin(), freeFramesList.begin() + pages);
    return true;
}

void memory::deallocateFrames(const std::string &proc) {
    for (auto &frame : frameTable) {
        if (frame.proc == proc) {
            frame.proc = ""; // Free the frame
            pagesOut++;
        }
    }
    populateFreeFramesList(); // Repopulate the free frames list after deallocation
}

bool memory::searchProcFrames(const std::string &proc) {
    for (const auto &frame : frameTable) {
        if (frame.proc == proc) {
            return true;
        }
    }
    return false;
}

std::string memory::removeOldestProcessFrame() {
    auto oldest = std::min_element(frameTable.begin(), frameTable.end(),
        [](const frame &a, const frame &b) {
            return !a.proc.empty() && (b.proc.empty() || a.timestamp < b.timestamp);
        });

    std::string evictedProc = oldest->proc;
    deallocateFrames(evictedProc);
    return evictedProc;
}

void memory::generateReportFrames() {
    time_t t = std::time(nullptr);
    std::cout << "Timestamp: " << std::put_time(std::localtime(&t), "%Y-%m-%d %H:%M:%S") << std::endl; 
    std::cout << "Number of processes in memory: " << std::count_if(frameTable.begin(), frameTable.end(), [](const frame &frame) { return !frame.proc.empty(); }) << std::endl;
    std::cout<< "Total external fragmentation: " << getExternalFragmentation() << " KB" << std::endl; 
    std::cout << "Memory Layout: " << std::endl; 
    std::cout << "----start---- = 0" << std::endl;

    for (const auto &frame : frameTable)
    {
        if (frame.proc.empty()) {
            std::cout << "Free Frame" << std::endl;
        } else {
            std::cout << frame.proc << " Page " << std::endl;  // Include the process 
        }
    }

    std::cout << "----end---- = " << max_overall_memory << std::endl;
}

