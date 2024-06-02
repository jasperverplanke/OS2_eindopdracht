#include "ate.h"
#include <string>
#include <iostream>
//eindopdracht.exe -p:5 -b:0 -t:-3 "you_and_i.pcm" "mysong"
bool ends_with(const std::string& value, const std::string& ending) {
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

int main(int argc, char* argv[]) {
    if (argc != 6) {

        std::cerr << "Usage: " << argv[0] << " -p:<number of threads> -b:<bass intensity> -t:<treble intensity> <input file> <output file>\n";
        return 1;
    }

    int maxThreads = 0;
    int bassIntensity = 0;
    int trebleIntensity = 0;
    std::string inFileName;
    std::string outFileName;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.substr(0, 3) == "-p:") {

            maxThreads = std::stoi(arg.substr(3));
            if (maxThreads > 8 || maxThreads < 1) {
                std::cerr << "Usage: " << argv[0] << "wrong amount of threads positive max 8\n";
                return 1;
            }
        }
        else if (arg.substr(0, 3) == "-b:") {
            bassIntensity = std::stoi(arg.substr(3));
            if (bassIntensity > 6 || bassIntensity < -6) {
                std::cerr << "Usage: " << argv[0] << "wrong amount of bass -6 to 6\n";
                return 1;
            }
        }
        else if (arg.substr(0, 3) == "-t:") {
            trebleIntensity = std::stoi(arg.substr(3));
            if (trebleIntensity > 6 || trebleIntensity < -6) {
                std::cerr << "Usage: " << argv[0] << "wrong amount of treble -6 to 6\n";
                return 1;
            }
        }
        else {
            if (inFileName.empty()) {
                inFileName = arg;
                if (!ends_with(inFileName, ".pcm")) {
                    std::cerr << "Error: Input file must have a .pcm extension\n";
                    return 1;
                }
            }
            else if (outFileName.empty()) {
                outFileName = arg;
                if (!ends_with(outFileName, ".pcm")) {
                    std::cerr << "Error: Output file must have a .pcm extension\n";
                    return 1;
                }
            }
            else {
                std::cerr << "Too many arguments.\n";
                return 1;
            }
        }
    }

    if (maxThreads <= 0 || inFileName.empty() || outFileName.empty()) {
        std::cerr << "Invalid arguments.\n";
        return 1;
    }

    ate myAte(maxThreads, bassIntensity, trebleIntensity, inFileName, outFileName);

    return 0;
}