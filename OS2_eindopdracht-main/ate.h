
#pragma once
#include <stdio.h>
#include <fstream>
#include <string>
#include "SaveFileWriter.cpp"

class ate
{
private:
	int ThreadsMax;
	int bassIntensity, trebleIntensity;
	SafeFileWriter* writer;
	std::string inFileName;
public:
	//ate();
	ate(int p, int bass, int treble, std::string inFileName, std::string outFileName);
	int blok(int maxThreads, int blokAdress);
	void bassCoefficients(int intintensity, double* b0, double* b1, double* b2, double* a1, double* a2);
	void trebleCoefficients(int intintensity, double* b0, double* b1, double* b2, double* a1, double* a2);
	void bassFilter(int bass, const int16_t inputArray[], int16_t outputArray[]);
	void trebleFilter(int treble, const int16_t inputArray[], int16_t outputArray[]);
	int16_t returnWithinRange(int input);
	void calculateBlock(int bass, int treble, const int16_t inputArray[], int16_t outputArray[]);
	void writeIntoFile(int blokAdress, const std::vector<int16_t>& data);
	//int execute(int p, int bass, int treble, std::string inFileName, std::string outFileName);
};

