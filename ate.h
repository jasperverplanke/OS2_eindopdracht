
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

public:
	ate();
	ate(int p, int bass, int treble, std::string inFileName, std::string outFileName);
	int blok(int maxThreads, int blokAdress);
	void bassCoefficients(int intintensity, double* b0, double* b1, double* b2, double* a1, double* a2);
	void trebleCoefficients(int intintensity, double* b0, double* b1, double* b2, double* a1, double* a2);
	//int execute(int p, int bass, int treble, std::string inFileName, std::string outFileName);
};

