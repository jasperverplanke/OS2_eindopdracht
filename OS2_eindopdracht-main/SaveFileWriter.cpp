#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>
#include <array>
#include <cstring>

class SafeFileWriter {
public:
    SafeFileWriter(const std::string& filename) : file(filename, std::ios::binary | std::ios::out) {
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file");
        }
    }

    // Function to write data at a specific position in the file
    void writeAtPosition(int position, const int16_t* buffer) {
        std::lock_guard<std::mutex> lock(fileMutex);
        file.seekp(position, std::ios::beg);
        std::array<char, 2048> charArray;
        std::memcpy(charArray.data(), buffer, 2048);
        file.write(charArray.data(), 2048);
        file.flush();
    }

private:
    std::ofstream file;
    std::mutex fileMutex;
};