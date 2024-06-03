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
	double frequency = 330; double qFactor = 0.5;
	double gain = intensity;
	double sampleRate = 44100;
	double pi = 4.0 * atan(1);
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
{ double frequency = 3300;
	double qFactor = 0.5;
	double gain = intensity;
	double sampleRate = 44100;
	double pi = 4.0 * atan(1);
	double a = pow(10.0, gain / 40); 
	double  w0 = 2 * pi * frequency / sampleRate;
	double alpha = sin(w0) / (2.0 * qFactor); 
	double a0 = (a + 1) - (a - 1) * cos(w0) + 2.0 * sqrt(a) * alpha;
	*a1 = -(2.0 * ((a - 1) - (a + 1) * cos(w0))) / a0;
	*a2 = -((a + 1) - (a - 1) * cos(w0) - 2.0 * sqrt(a) * alpha) / a0;
	*b0 = (a * ((a + 1) + (a - 1) * cos(w0) + 2.0 * sqrt(a) * alpha)) / a0;
	*b1 = (-2.0 * a * ((a - 1) + (a + 1) * cos(w0))) / a0;
	*b2 = (a * ((a + 1) + (a - 1) * cos(w0) - 2.0 * sqrt(a) * alpha)) / a0; 
}

//Creates the bass filter
void ate::bassFilter(int bass, const int16_t inputArray[], int16_t outputArray[])
{
	double b0, b1, b2, a1, a2;
	bassCoefficients(bass, &b0, &b1, &b2, &a1, &a2);

	//Calculate the first two since both do not have 2 previous values
	outputArray[0] = returnWithinRange((b0 * inputArray[0]));
	outputArray[1] = returnWithinRange(b0 * inputArray[1] + b1 * inputArray[0] + a1 * outputArray[0]);
	//For loop to walk through the block
	for (int n = 2; n < 1024; n++)
	{
		outputArray[n] = returnWithinRange(b0 * inputArray[n] + b1 * inputArray[n - 1] + b2 * inputArray[n - 2] + a1 * outputArray[n - 1] + a2 * outputArray[n - 2]);
	}
}

//Creates the treble filter
void ate::trebleFilter(int treble, const int16_t inputArray[], int16_t outputArray[])
{
	double b0, b1, b2, a1, a2;
	trebleCoefficients(treble, &b0, &b1, &b2, &a1, &a2);

	//Calculate the first two since both do not have 2 previous values
	outputArray[0] = returnWithinRange((b0 * inputArray[0]));
	outputArray[1] = returnWithinRange((b0 * inputArray[1] + b1 * inputArray[0] + a1 * outputArray[0]));
	//For loop to walk through the block
	for (int n = 2; n < 1024; n++)
	{
		outputArray[n] = returnWithinRange((b0 * inputArray[n] + b1 * inputArray[n - 1] + b2 * inputArray[n - 2] + a1 * outputArray[n - 1] + a2 * outputArray[n - 2]));
	}
}

//Returns the input withing the singed shorts range
int16_t ate::returnWithinRange(int input)
{
	if (input > 32767)
		input = 32767;
	else if (input < -32768)
		input = -32768;

	return static_cast<int16_t>(input);
}

//Calculate the block
void ate::calculateBlock(int bass, int treble, const int16_t inputArray[], int16_t outputArray[])
{
	int16_t bassOutputArray[1024];
	int16_t trebleOutputArray[1024];
	bassFilter(bass, inputArray, bassOutputArray);
	trebleFilter(treble, inputArray, trebleOutputArray);

	//Combine both filters together to form the result
	for (int n = 0; n < 1024; n++)
	{
		// Make sure the sum is within the max range
		int combinedValue = (bassOutputArray[n] + trebleOutputArray[n])/2;
		outputArray[n] = returnWithinRange(combinedValue);
	}
}

void ate::writeIntoFile(int blokAdress, const std::vector<int16_t>& data) {
	std::ofstream outFile("i_and_you.pcm", std::ios::binary | std::ios::out);
	if (outFile) {
		outFile.seekp(blokAdress, std::ios::beg);
		outFile.write(reinterpret_cast<const char*>(data.data()), 1024 * sizeof(int16_t));
		outFile.close();
	}
	else {
		std::cerr << "Error opening output file: " << "i_and_you.pcm" << std::endl;
	}
}

int ate::blok(int maxThreads, int blokAdress) {
	std::ifstream file(inFileName, std::ios::binary);
	//char* buffer = new char[200];
	std::vector<int16_t> buffer(1024);

	if (file) {
		
		file.seekg(blokAdress, std::ios::beg);
		
		file.read(reinterpret_cast<char*>(buffer.data()), 1024 * sizeof(int16_t));
		file.close();// Close the file

	}
	else {
		std::cerr << "Error opening file: " << inFileName << std::endl;
	}

	int16_t bufferInt[1024] = { 0 }; // Initialize bufferInt with zeros

	// Copy data from buffer to bufferInt
	for (size_t i = 0; i < buffer.size(); ++i) {
		bufferInt[i] = buffer[i];
	}
	int16_t output[1024];
	calculateBlock(bassIntensity, trebleIntensity, bufferInt, output);
	// now save;
	writer->writeAtPosition(blokAdress, output);
	//delete[] buffer;
	return 0;
}

ate::ate(int maxThreads, int bass, int treble, std::string inFileName, std::string outFileName) {
	bassIntensity = bass;
	trebleIntensity = treble;
	ThreadsMax = maxThreads;
	this->inFileName = inFileName;
	SafeFileWriter writer(outFileName);
	this->writer = &writer;
	//int use = 0;
	//int blokRunning = 0;
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


	
	std::vector<std::thread> threads;
	//std::vector<int> threadsPerThread;
	int currentUse = 0;
	int initiatedAdress = 0;
	bool end = false;
	int totalUse = 0;
	while (initiatedAdress < fileSize) { // length of file
		//select blok(i)
		
		if (ThreadsMax - currentUse > 0 ) {
			
		
			//blokRunning += 1;
			threads.emplace_back(&ate::blok, this, 1, initiatedAdress);
			//threadsPerThread.emplace_back(use);
			currentUse += 1;
			
			initiatedAdress += 2048;
			totalUse++;
			//std::cout << initiatedAdress << " / " << fileSize <<  "    " << blokRunning << " using " << use << std::endl;
		}
		else {
		
			for (size_t i = 0; i < threads.size(); ++i) {
				if (threads[i].joinable()) {
					threads[i].join();
					//std::cout << "Thread is closed with use = " << threadsPerThread[i] << ".\n";
					threads.erase(threads.begin() + i);
					currentUse -= 1;
					//threadsPerThread.erase(threadsPerThread.begin() + i);
					//blokRunning -= 1;

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
				currentUse -= 1;
				//threadsPerThread.erase(threadsPerThread.begin() + i);
				//blokRunning -= 1;

			}
		}
	}
	std::cout << currentUse << "<-current threads  runs->" << totalUse << std::endl;
	//outputFile.close();

	//return 0;
}