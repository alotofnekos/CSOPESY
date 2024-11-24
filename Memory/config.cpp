#include "headerFiles/config.h"

config::config() {}

bool config::initializeConfig() {
    std::ifstream file("config.txt");

    if (!file)
    {
        std::cerr << "Error: could not open config.txt" << std::endl;
        return false; 
    }

    std::string line; 

    while (std::getline(file, line)) 
    {
        std::istringstream iss(line);
        std::string param; 
        iss >> param; 

        if (param == "num-cpu")
        {
            iss >> num_cpu;
            if (num_cpu < 1 || num_cpu > 128)
            {
                std::cerr << "Error: num-cpu must be between 1 and 128" << std::endl;
                return false;
            }
            
        } else if (param == "scheduler")
        {
            iss >> scheduler;
            if (scheduler.front() == '"' && scheduler.back() == '"') 
            {
                scheduler = scheduler.substr(1, scheduler.size() - 2);
            }
        } else if (param == "quantum-cycles")
        {
            iss >> quantum_cycles;
            if (quantum_cycles < 1 || quantum_cycles > 4294967296)
            {
                std::cerr << "Error: quantum-cycles must be between 1 and 2^32 (4294967296)" << std::endl;
                return false;
            }
            
        } else if (param == "batch-process-freq")
        {
            iss >> batch_process_freq;
            if (batch_process_freq < 1 || batch_process_freq > 4294967296)
            {
                std::cerr << "Error: batch-process-freq must be between 1 and 2^32 (4294967296)" << std::endl;
                return false;
            }
            
        } else if (param == "min-ins")
        {
            iss >> min_ins;
            if (min_ins < 1 || min_ins > 4294967296)
            {
                std::cerr << "Error: min-ins must be between 1 and 2^32 (4294967296)" << std::endl;
                return false;
            }
            
        } else if (param == "max-ins")
        {
            iss >> max_ins;
            if (max_ins < 1 || max_ins > 4294967296)
            {
                std::cerr << "Error: max-ins must be between 1 and 2^32 (4294967296)" << std::endl;
                return false;
            }
            
        } else if (param == "delays-per-exec")
        {
            iss >> delays_per_exec;
            if (delays_per_exec < 0 || delays_per_exec > 4294967296)
            {
                std::cerr << "Error: delays-per-exec must be between 0 and 2^32 (4294967296)" << std::endl;
                return false;
            }
            
        } else if (param == "max-overall-mem")
        {
            iss >> max_overall_memory;
        } else if (param == "mem-per-frame")
        {
            iss >> memory_per_frame;
        } else if (param == "mem-per-proc")
        {
            iss >> memory_per_process; 
        } else
        {
            std::cerr << "Error: unknown parameter" << std::endl;
            return false; 
        }
    }
    
    file.close();
    return true; 
}

void config::displayConfig() const {
    std::cout << "\nLoaded Configuration:" << std::endl;
    std::cout << "num-cpu: " << num_cpu << std::endl;
    std::cout << "scheduler: " << scheduler << std::endl;
	std::cout << "quantum-cycles: " << quantum_cycles << std::endl;
    std::cout << "batch-process-freq: " << batch_process_freq << std::endl;
    std::cout << "min-ins: " << min_ins << std::endl;
    std::cout << "max-ins: " << max_ins << std::endl;
    std::cout << "delays-per-exec: " << delays_per_exec << std::endl;
    std::cout << "max-overall-mem: " << max_overall_memory << std::endl;
    std::cout << "mem-per-frame: " << memory_per_frame << std::endl;
    std::cout << "mem-per-proc: " << memory_per_process << std::endl;
    std::cout << std::endl;
}

int config::getNumCPU() const {
    return num_cpu;
}

std::string config::getScheduler() const {
    return scheduler; 
}

int config::getQuantumCycles() const {
    return quantum_cycles;
}

int config::getBatchProcessFreq() const {
    return batch_process_freq;
}

int config::getMinIns() const {
    return min_ins;
}

int config::getMaxIns() const {
    return max_ins;
}

int config::getDelaysPerExec() const {
    return delays_per_exec;
}

int config::getMaxOverallMemory() const {
    return max_overall_memory;
}

int config::getMemoryPerFrame() const {
    return memory_per_frame;
}

int config::getMemoryPerProcess() const {
    return memory_per_process;
}