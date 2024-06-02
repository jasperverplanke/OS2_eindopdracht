#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>

class SafeFileWriter {
public:
    SafeFileWriter(const std::string& filename) : file(filename, std::ios::binary | std::ios::out) {
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file");
        }
    }

    // Function to write data at a specific position in the file
    void writeAtPosition(int position, const std::string& data) {
        std::lock_guard<std::mutex> lock(fileMutex);
        file.seekp(position, std::ios::beg);
        file.write(data.c_str(), data.size());
        file.flush();
    }

private:
    std::ofstream file;
    std::mutex fileMutex;
};