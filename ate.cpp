#include "ate.h"
#include <math.h>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

//ate::execute() {

//}


void ate::bassCoefficients(int intensity, double* b0, double* b1, double* b2, double* a1, double* a2) 
{
	double frequency = 330; double qFactor = 0.5; double gain = intensity; double sampleRate = 44100; double pi = 4.0 * atan(1);
	double a = pow(10.0, gain / 40); 
	double w0 = 2 * pi * frequency / sampleRate; 
	double alpha = sin(w0) / (2.0 * qFactor); 
	double a0 = (a + 1) + (a - 1) * cos(w0) + 2.0 * sqrt(a) * alpha;
	*a1 = -(-2.0 * ((a - 1) + (a + 1) * cos(w0))) / a0;
	*a2 = -((a + 1) + (a - 1) * cos(w0) - 2.0 * sqrt(a) * alpha) / a0;
	*b0 = (a * ((a + 1) - (a - 1) * cos(w0) + 2.0 * sqrt(a) * alpha)) / a0;
	*b1 = (2 * a * ((a - 1) - (a + 1) * cos(w0))) / a0;
	*b2 = (a * ((a + 1) - (a - 1) * cos(w0) - 2.0 * sqrt(a) * alpha)) / a0;
} 

void ate::trebleCoefficients(int intensity, double* b0, double* b1, double* b2, double* a1, double* a2)
{ double frequency = 3300; double qFactor = 0.5; double gain = intensity; double sampleRate = 44100;
	double pi = 4.0 * atan(1); double a = pow(10.0, gain / 40); double  w0 = 2 * pi * frequency / sampleRate;
	double alpha = sin(w0) / (2.0 * qFactor); double a0 = (a + 1) - (a - 1) * cos(w0) + 2.0 * sqrt(a) * alpha;
	*a1 = -(2.0 * ((a - 1) - (a + 1) * cos(w0))) / a0; *a2 = -((a + 1) - (a - 1) * cos(w0) - 2.0 * sqrt(a) * alpha) / a0;
	*b0 = (a * ((a + 1) + (a - 1) * cos(w0) + 2.0 * sqrt(a) * alpha)) / a0;
	*b1 = (-2.0 * a * ((a - 1) + (a + 1) * cos(w0))) / a0; *b2 = (a * ((a + 1) + (a - 1) * cos(w0) - 2.0 * sqrt(a) * alpha)) / a0; 
}

int ate::blok(int maxThreads, int blokAdress) {
	//std::cout << "Thread " << std::this_thread::get_id() << " is working.\n";
	//std::thread bass(ate::bassCoefficients, 3);
	//std::thread treble(ate::trebleCoefficients, 3/*vars */);
	//bass.join();
	//bass.join();

	// now save;
	writer->writeAtPosition(blokAdress, "data: je bent een plopkoek");
	return 0;
}

ate::ate(int maxThreads, int bass, int treble, std::string inFileName, std::string outFileName) {
	bassIntensity = bass;
	trebleIntensity = treble;
	ThreadsMax = maxThreads;
	SafeFileWriter writer(outFileName);
	this->writer = &writer;
	int use;
	int blokRunning = 0;
	std::streampos fileSize;
	std::ifstream file(inFileName, std::ios::binary);

	if (file) {
		file.seekg(0, std::ios::end);// Seek to the end of the file
		fileSize = file.tellg();// Get the current position in the file (which is the size of the file)		
		file.close();// Close the file
		std::cout << "The size of the file is: " << fileSize << " bytes" << std::endl;// Output the file size

	}
	else {
		std::cerr << "Error opening file: " << inFileName << std::endl;
	}



	//std::ofstream outputFile(inFileName);
	//if (!outputFile.is_open()) { // Check if the file was opened successfully
	//	std::cerr << "Error: Could not open the file " << inFileName << std::endl;
	//	//return 1;
	//}
	std::vector<std::thread> threads;
	std::vector<int> threadsPerThread;
	int currentUse = 0;
	int initiatedAdress = 0;
	bool end = false;
	while (initiatedAdress < fileSize) { // length of file
		//select blok(i)
		
		if (ThreadsMax - currentUse > 0 ) {
			
			//std::cout << currentUse << " +3 " << " < " << ThreadsMax << std::endl;
			
			use = ThreadsMax - currentUse;
			if (use > 2) {
				use = 2;
			}
			blokRunning += 1;
			threads.emplace_back(&ate::blok, this, use, initiatedAdress);
			threadsPerThread.emplace_back(use);
			currentUse += use +1;
			
			initiatedAdress += 2028;
			std::cout << initiatedAdress << " / " << fileSize <<  "    " << blokRunning << " using " << use << std::endl;
		}
		else {
			/*/for (auto& t : threads) {
				if (t.joinable()) {
					t.join();
					std::cout << "Thread id = " << threads << std::this_thread::get_id() << " is closed.\n";
					//end = true;

				}
			}*/
			for (size_t i = 0; i < threads.size(); ++i) {
				if (threads[i].joinable()) {
					threads[i].join();
					//std::cout << "Thread is closed with use = " << threadsPerThread[i] << ".\n";
					threads.erase(threads.begin() + i);
					currentUse -= threadsPerThread[i] + 1;
					threadsPerThread.erase(threadsPerThread.begin() + i);
					blokRunning -= 1;

				}
			}
		}
		
	}
	while (threads.size() != 0) {

		for (size_t i = 0; i < threads.size(); ++i) {
			if (threads[i].joinable()) {
				threads[i].join();
				//std::cout << "Thread is closed with use = " << threadsPerThread[i] << ".\n";
				threads.erase(threads.begin() + i);
				currentUse -= threadsPerThread[i] + 1;
				threadsPerThread.erase(threadsPerThread.begin() + i);
				blokRunning -= 1;

			}
		}
	}
	std::cout << blokRunning << " using " << use << std::endl;
	//outputFile.close();

	//return 0;
}